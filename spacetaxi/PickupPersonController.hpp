//
//  PickupPersonController.hpp
//  SpaceTaxi
//
//  Created by Emil Rasmussen on 18/11/2022.
//

#pragma once

#include "Component.hpp"

class PickupPersonController : public Component {
public:
    explicit PickupPersonController(GameObject *gameObject, int value);
    
    void update(float deltaTime) override;

    void onCollisionStart(PhysicsComponent *comp) override;

    void onCollisionEnd(PhysicsComponent *comp) override;
    
    int value;
    bool canBePickedUp = false;
};
