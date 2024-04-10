#include <sre/Inspector.hpp>
#include <iostream>
#include "BirdGame.hpp"
#include "GameObject.hpp"
#include "sre/RenderPass.hpp"
#include "SpriteComponent.hpp"
#include "BackgroundComponent.hpp"

#include "SpriteAnimationComponent.hpp"
#include "Box2D/Dynamics/Contacts/b2Contact.h"
#include "PhysicsComponent.hpp"
#include "BirdController.hpp"

using namespace std;
using namespace sre;

const glm::vec2 BirdGame::windowSize(700,400);

BirdGame* BirdGame::instance = nullptr;

BirdGame::BirdGame()
        :debugDraw(physicsScale)
{
    instance = this;
    r.setWindowSize(windowSize);
    r.init()
      .withSdlInitFlags(SDL_INIT_EVERYTHING)
      .withSdlWindowFlags(SDL_WINDOW_OPENGL);

    init();

    // setup callback functions
    r.keyEvent = [&](SDL_Event& e){
        onKey(e);
    };
    
    camera.setWindowCoordinates();
    r.frameUpdate = [&](float deltaTime){
        update(deltaTime);
    };
    r.frameRender = [&](){
        render();
    };
    // start game loop
    r.startEventLoop();
}

void BirdGame::init() {
    if (world != nullptr){ // deregister call backlistener to avoid getting callbacks when recreating the world
        world->SetContactListener(nullptr);
    }
    
    sceneObjects.clear();
    physicsComponentLookup.clear();
    initPhysics();
    spriteAtlas = SpriteAtlas::create("space_taxi.json","space_taxi.png");
    
    auto birdObj = createGameObject();
    birdObj->name = "Bird";
    auto so = birdObj->addComponent<SpriteComponent>();
    auto sprite = spriteAtlas->get("taxi_right.png");
    sprite.setScale(glm::vec2({0.75,0.75}));
    
    birdObj->setPosition({windowSize.x / 2, windowSize.y / 2});
    so->setSprite(sprite);
    auto anim = birdObj->addComponent<SpriteAnimationComponent>();
    auto phys = birdObj->addComponent<PhysicsComponent>();
    phys->initCircle(b2_dynamicBody, 10/physicsScale, {birdObj->getPosition().x/physicsScale,birdObj->getPosition().y/physicsScale}, 1);
    auto birdC = birdObj->addComponent<BirdController>();
    
    vector<Sprite> spriteAnim({spriteAtlas->get("taxi_right.png"),spriteAtlas->get("taxi_right.png"),spriteAtlas->get("taxi_right.png"),spriteAtlas->get("taxi_right.png")});
    for(auto & s : spriteAnim){
        s.setScale(glm::vec2({0.75,0.75}));
    }
    anim->setSprites(spriteAnim);

    background1Component.init("background_space.png");

    auto ceil_obj = createGameObject();
    ceil_obj->name = "Ceiling";
    auto ceil_phys = ceil_obj->addComponent<PhysicsComponent>();
}

void BirdGame::update(float time) {
    int customerCounter = 0;
    int pendingValue = 0;
    
    if (gameState == GameState::Running){
        updatePhysics();
    }
    
    // Delete coins
    for (int i=0;i<sceneObjects.size();i++){
        if(sceneObjects[i]->hitFloor)
        {
            sceneObjects.erase(sceneObjects.begin()+i);
        }
        
        if (sceneObjects[i]->shouldDelete == true)
        {
            sceneObjects.erase(sceneObjects.begin()+i);
        }
        else if (sceneObjects[i]->name == "PickupPerson")
        {
            customerCounter++;
        }
        else if (sceneObjects[i]->name == "Bird")
        {
            pendingValue += sceneObjects[i]->getComponent<BirdController>()->pendingCustomerValue;
        }
    }
    
    canAdvanceToNextLevel = customerCounter == 0 && pendingValue == 0;
    
    for (int i=0;i<sceneObjects.size();i++){
        sceneObjects[i]->update(time);
    }

    Timer += time;
}

void BirdGame::render() {
    auto renderPass = RenderPass::create()
            .withCamera(camera)
            .build();


    //auto pos = camera->getGameObject()->getPosition();
    background1Component.renderBackground(renderPass, windowSize.x * 0.5f);

    auto spriteBatchBuilder = SpriteBatch::create();

    for (auto &go : sceneObjects) {
        go->renderSprite(spriteBatchBuilder);
    }

    if (gameState == GameState::Ready) {
        auto sprite = spriteAtlas->get("get_ready.png");
        sprite.setPosition(glm::vec2(windowSize * 0.5f));
        spriteBatchBuilder.addSprite(sprite);

    } else if (gameState == GameState::GameOver) {
        auto sprite = spriteAtlas->get("game_over.png");
        //sprite.setPosition(pos);
        spriteBatchBuilder.addSprite(sprite);
    }

    //auto backgroundSprite = spriteAtlas->get("background_space.png");
    //backgroundSprite.setPosition(glm::vec2(windowSize*0.5f));
    //spriteBatchBuilder.addSprite(backgroundSprite);
    ImGui::SetNextWindowPos(ImVec2(Renderer::instance->getWindowSize().x/2-100, .0f), ImGuiSetCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 80), ImGuiSetCond_Always);
    ImGui::Begin("", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
    ImGui::LabelText("Customers waiting", "%i", pendingValue); // updating in
    ImGui::LabelText("Income", "%i$", money); // the
    ImGui::LabelText("Timer","%.2f sec", Timer);
    ImGui::End();

    auto sb = spriteBatchBuilder.build();
    renderPass.draw(sb);

    //background2Component.renderBackground(rp, -pos.x*0.2f);

    if (doDebugDraw) {
        world->DrawDebugData();
        renderPass.drawLines(debugDraw.getLines());
        debugDraw.clear();
    }
}

void BirdGame::onKey(SDL_Event &event) {
    for (auto & gameObject: sceneObjects) {
        for (auto & c : gameObject->getComponents()){
            bool consumed = c->onKey(event);
            if (consumed){
                return;
            }
        }
    }

    if (event.type == SDL_KEYDOWN){
        

        switch (event.key.keysym.sym){
            case SDLK_d:
                // press 'd' for physics debug
                doDebugDraw = !doDebugDraw;
                if (doDebugDraw){
                    world->SetDebugDraw(&debugDraw);
                } else {
                    world->SetDebugDraw(nullptr);
                }
                break;
            case SDLK_r:
                init();
                break;
            case SDLK_SPACE:
                if (gameState == GameState::GameOver) {
                    init();
                    gameState = GameState::Ready;
                } else if (gameState == GameState::Ready) {
                    gameState = GameState::Running;
                }
                break;
        }
    }
}

std::shared_ptr<GameObject> BirdGame::createGameObject() {
    auto obj = shared_ptr<GameObject>(new GameObject());
    sceneObjects.push_back(obj);
    return obj;
}

void BirdGame::updatePhysics() {
    const float32 timeStep = 1.0f / 60.0f;
    const int positionIterations = 2;
    const int velocityIterations = 6;
    world->Step(timeStep, velocityIterations, positionIterations);
    const float32 birdVelocity = 0.f; //0.05f;
    const float32 coinVelocity = 0.1f;
    
    for (auto phys : physicsComponentLookup){
        if (phys.second->rbType == b2_staticBody) continue;
        auto position = phys.second->body->GetPosition();
        float angle = phys.second->body->GetAngle();
        auto gameObject = phys.second->getGameObject();
        
        if(gameObject->name=="Bird")
        {
            std::shared_ptr<PhysicsComponent> physics = gameObject->getComponent<PhysicsComponent>();
            glm::vec2 v = physics->getLinearVelocity();
            v.x += birdVelocity;
            glm::vec2 vel = {birdVelocity, 0.f};
        }
        if(gameObject->name=="Ceiling" || gameObject->name=="Floor")
        {
            std::shared_ptr<PhysicsComponent> physics = gameObject->getComponent<PhysicsComponent>();
            glm::vec2 v = physics->getLinearVelocity();
            glm::vec2 vel = {birdVelocity, 0.f};
            physics->setLinearVelocity(vel);
        }
        
        gameObject->setPosition(glm::vec2(position.x*physicsScale, position.y*physicsScale));
        gameObject->setRotation(angle);
    }
}

void BirdGame::initPhysics() {
    float gravity = -0.5; // 9.8 m/s2
    delete world;
    world = new b2World(b2Vec2(0,gravity));
    world->SetContactListener(this);

    if (doDebugDraw){
        world->SetDebugDraw(&debugDraw);
    }
}

void BirdGame::BeginContact(b2Contact *contact) {
    b2ContactListener::BeginContact(contact);
    handleContact(contact, true);
}

void BirdGame::EndContact(b2Contact *contact) {
    b2ContactListener::EndContact(contact);
    handleContact(contact, false);
}

void BirdGame::deregisterPhysicsComponent(PhysicsComponent *r) {
    auto iter = physicsComponentLookup.find(r->fixture);
    if (iter != physicsComponentLookup.end()){
        physicsComponentLookup.erase(iter);
    } else {
        assert(false); // cannot find physics object
    }
}

void BirdGame::registerPhysicsComponent(PhysicsComponent *r) {
    physicsComponentLookup[r->fixture] = r;
}

void BirdGame::handleContact(b2Contact *contact, bool begin) {
    auto fixA = contact->GetFixtureA();
    auto fixB = contact->GetFixtureB();
    auto physA = physicsComponentLookup.find(fixA);
    auto physB = physicsComponentLookup.find(fixB);
    if (physA !=physicsComponentLookup.end() && physB != physicsComponentLookup.end()){
        auto & aComponents = physA->second->getGameObject()->getComponents();
        auto & bComponents = physB->second->getGameObject()->getComponents();
        for (auto & c : aComponents){
            if (begin){
                c->onCollisionStart(physB->second);
                if((c->getGameObject()->name=="Ceiling"||c->getGameObject()->name=="Floor"||c->getGameObject()->name=="Wall top"||c->getGameObject()->name=="Wall bottom") && physB->second->getGameObject()->name == "Bird")
                {
                    // Printing names
                    std::cout << "physB: "<< physB->second->getGameObject()->name << std::endl;
                    std::cout << "physA: "<< physA->second->getGameObject()->name << std::endl;
                    setGameState(GameState::GameOver);
                }
                if((c->getGameObject()->name=="Bird") && physB->second->getGameObject()->name == "Coin")
                {
                    auto physics = c->getGameObject()->getComponent<PhysicsComponent>();
                    std::cout << "Coin hit bird!" << std::endl;
                    physB->second->getGameObject()->hitFloor = true;
                }
                
            } else {
                c->onCollisionEnd(physB->second);
            }
        }
        for (auto & c : bComponents){
            if (begin){
                c->onCollisionStart(physA->second);
            } else {
                c->onCollisionEnd(physA->second);
            }
        }
    }
}

void BirdGame::setGameState(GameState newState) {
    this->gameState = newState;
}
