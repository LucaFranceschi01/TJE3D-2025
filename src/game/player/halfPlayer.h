//
// Created by Xavi Cañadas on 20/2/25.
//

#pragma once

#include "player.h"



/*

Para dividir la bola en dos mas pequeñas habia pensado en hacer una subclase de Player

Lo que cambia es el update. Podemos hacer que cuando hayan dos players,
las flechas controlen el de la derecha y "ad" el de la izquierda.

Para implementarlo, guardamos como instanced la entity de la bola en world y los tres players: el normal y estos dos.
Para renderizar estos dos solo tendremos que canviar el scale de la model y ya.

En world podemos tener un bool para elegir jugar con el normal o con estos dos, pulsando la tecla q pusimos

Esta clase representa solo un half player, podemos tener como atributo si es el de la derecha o izquierda,
así en update elegir un control u otro.

 */

class HalfPlayer : public Player {
public:
    bool is_left;

    HalfPlayer(Mesh* mesh, const Material& material, bool is_left, const std::string& name = ""): Player(mesh, material, name)
    {
        this->is_left = is_left;
    }

    void init(Vector3 pos = Vector3(0, 10, 0)) override;

    void moveControl(Vector3& move_dir,  float dt);
};



