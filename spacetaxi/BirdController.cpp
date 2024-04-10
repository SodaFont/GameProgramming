//
// Created by Morten Nobel-Jørgensen on 19/10/2017.
//

#include <SDL_events.h>
#include <iostream>
#include "BirdController.hpp"
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "BirdGame.hpp"
#include "SpriteComponent.hpp"
#include "PickUpPersonController.hpp"

BirdController::BirdController(GameObject *gameObject) : Component(gameObject) {
    std::shared_ptr<PhysicsComponent> physics = gameObject->getComponent<PhysicsComponent>();
    glm::vec2 v = physics->getLinearVelocity();
    v.x += 0.f;
    physics->setLinearVelocity(v);
}

void BirdController::update(float deltaTime)
{
    glm::vec2 movement{0,0};
    if (this->left){
        movement.x --;
    }
    if (this->right){
        movement.x ++;
    }
    if (this->up) {
        movement.y += 0.5f;
    }
    if (this->down) {
        movement.y -= 0.5f;
    }
    
    std::shared_ptr<PhysicsComponent> physics = gameObject->getComponent<PhysicsComponent>();
    float accelerationSpeed = 0.01f;
    physics->addImpulse(movement*accelerationSpeed);
    float maximumVelocity = 1.f;
    auto linearVelocity = physics->getLinearVelocity();
    float currentVelocity = linearVelocity.x;
    
    if (abs(currentVelocity) > maximumVelocity){
        linearVelocity.x = glm::sign(linearVelocity.x)*maximumVelocity;
        physics->setLinearVelocity(linearVelocity);
    }
}

bool BirdController::onKey(SDL_Event &event) {
    
    std::shared_ptr<PhysicsComponent> physics = gameObject->getComponent<PhysicsComponent>();
    
    if (event.type == SDL_KEYDOWN){
        
        if (event.key.keysym.sym == SDLK_UP)
        {
            this->up = true;
        }
        
        if (event.key.keysym.sym == SDLK_LEFT)
        {
            this->left = true;
        }
        
        if (event.key.keysym.sym == SDLK_RIGHT)
        {
            this->right = true;
        }
        
        if (event.key.keysym.sym == SDLK_DOWN)
        {
            this->down = true;
        }
        
        if (event.key.keysym.sym == SDLK_SPACE)
        {
            if (this->pickupPerson != nullptr && this->pendingCustomerValue == 0)
            {
                // Handle the adding of points in update loop
                this->pickupPerson->shouldDelete = true;
                this->pendingCustomerValue = this->pickupPerson->getComponent<PickupPersonController>()->value;
            }
            else if (this->deliveryPoint != nullptr && this->pendingCustomerValue > 0)
            {
                // add value to scoreboard
                this->pendingCustomerValue = 0;
            }
        }
    } else if (event.type == SDL_KEYUP){
        std::cout << "some key released" << std::endl;
        
        if (event.key.keysym.sym == SDLK_UP)
        {
            this->up = false;
        }
        
        if (event.key.keysym.sym == SDLK_LEFT)
        {
            this->left = false;
        }
        
        if (event.key.keysym.sym == SDLK_RIGHT)
        {
            this->right = false;
        }
        
        if (event.key.keysym.sym == SDLK_DOWN)
        {
            this->down = false;
        }
    }
    return false;
}

void BirdController::onCollisionStart(PhysicsComponent *comp) {
    std::cout << "bird collided with something" << std::endl;
    
    if (comp->getGameObject()->name == "PickupPerson")
    {
        pickupPerson = comp->getGameObject();
    }
    
    if (comp->getGameObject()->name == "DeliveryPoint")
    {
        deliveryPoint = comp->getGameObject();
    }
}

void BirdController::onCollisionEnd(PhysicsComponent *comp) {
    if (comp->getGameObject()->name == "PickupPerson")
    {
        pickupPerson = nullptr;
    }
    
    if (comp->getGameObject()->name == "DeliveryPoint")
    {
        deliveryPoint = nullptr;
    }
}


