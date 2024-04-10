//
//  PickupPersonController.cpp
//  SpaceTaxi
//
//  Created by Emil Rasmussen on 18/11/2022.
//

#include <SDL_events.h>
#include "PickupPersonController.hpp"
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "BirdGame.hpp"

PickupPersonController::PickupPersonController(GameObject *gameObject, int value) : Component(gameObject) {
    this->value = value;
}

void PickupPersonController::update(float deltaTime) { }

void PickupPersonController::onCollisionStart(PhysicsComponent *comp) {
}

void PickupPersonController::onCollisionEnd(PhysicsComponent *comp) {
}
