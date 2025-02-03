//
// Created by Xavi Cañadas on 3/2/25.
//

#ifndef STAGE_H
#define STAGE_H

#include "framework/includes.h"
#include "framework/camera.h"
#include "framework/utils.h"
#include "framework/entities/entity.h"



using namespace std; // to avoid writing std:: all the time

class Stage {
public:
    virtual ~Stage() = default;

    // methods
    virtual void render() = 0;
    virtual void update(double dt) = 0;

};

class PlayStage : public Stage {
  public:
    // attributes
    Camera* camera;
    Entity* root = nullptr;


    // constructor
    PlayStage(Camera* camera);

    // methods
    void render() override;
    void update(double dt) override;

};



#endif //STAGE_H
