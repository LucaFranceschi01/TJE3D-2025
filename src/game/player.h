//
// Created by Xavi Cañadas on 6/2/25.
//

#ifndef PLAYER_H
#define PLAYER_H

#include "stage.h"

#include "framework/includes.h"
#include "framework/entities/entityMesh.h"

using namespace std;

class Player : public EntityMesh {
private:
    float walk_speed = 0.0f;
    Vector3 speed = Vector3(0.0f);

public:
    Player() = default;
    Player(Mesh* mesh, const Material& material, const string& name = "");

    void update(float dt) override;

};



#endif //PLAYER_H
