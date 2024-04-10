//
// Created by Morten Nobel-Jørgensen on 10/10/2017.
//

#include <sre/SpriteAtlas.hpp>
#include <sre/Texture.hpp>
#include "sre/RenderPass.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include "BackgroundComponent.hpp"
#include "BirdGame.hpp"
#include "PhysicsComponent.hpp"
#include "Component.hpp"
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "BackgroundComponent.hpp"

using namespace sre;
using namespace glm;

BackgroundComponent::BackgroundComponent()
{
}

void BackgroundComponent::renderBackground(sre::RenderPass &renderPass, float offset) {
    renderPass.draw(batch, glm::translate(vec3(offset,0,0)));
}

void BackgroundComponent::init(std::string filename) {
    //BirdGame bg;
    
    tex = Texture::create().withFile(filename)
            .withFilterSampling(false)
            .build();
    
    auto atlas = SpriteAtlas::createSingleSprite(tex,"background", vec2(0,0));
    auto sprite = atlas->get("background");
    float scale = BirdGame::windowSize.y / tex->getHeight();
    sprite.setScale({scale,scale});
    
    auto batchBuilder = SpriteBatch::create();
    for (int i=0;i<100;i++){
        sprite.setPosition(vec2(tex->getWidth() * (i-1) * scale, 0));
        batchBuilder.addSprite(sprite);
        
        //auto obj = BirdGame::createGameObject();
        //obj->name = filename;
       //auto so = obj->addComponent<SpriteComponent>();

        glm::vec2 pos{vec2(tex->getWidth() * (i-1) * scale, 0)};
        
        //obj->setPosition(pos);
        //so->setSprite(sprite);
        //auto phys = obj->addComponent<PhysicsComponent>();
        //phys->initBox(b2_staticBody, {0.3f,1.60f}, {obj->getPosition().x,obj->getPosition().y}, 1);//+0.5f

    }
    batch = batchBuilder.build();

}

glm::vec2 BackgroundComponent::getSize(std::string filename)
{
    tex = Texture::create().withFile(filename)
            .withFilterSampling(false)
            .build();
    
    auto atlas = SpriteAtlas::createSingleSprite(tex,"background", vec2(0,0));
    auto sprite = atlas->get("background");
    float scale = BirdGame::windowSize.y / tex->getHeight();
    
    sprite.setScale({scale,scale});
    
    return sprite.getSpriteSize();
}


void BackgroundComponent::addPhysics()
{
}
