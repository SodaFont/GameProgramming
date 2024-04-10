#pragma once

#include "Component.hpp"

class BirdController : public Component {
public:
    explicit BirdController(GameObject *gameObject);
    
    void update(float deltaTime) override;

    bool onKey(SDL_Event &event) override;

    void onCollisionStart(PhysicsComponent *comp) override;

    void onCollisionEnd(PhysicsComponent *comp) override;
    
    // I added this
    std::shared_ptr<PhysicsComponent> physics;
    
    bool left,right,up,down = false;
    GameObject* pickupPerson = nullptr;
    GameObject* deliveryPoint = nullptr;
    int pendingCustomerValue = 0;
};
