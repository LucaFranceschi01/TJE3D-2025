/*  by Xavier Cañadas and Luca Franceschi 2025
    Here we define the HalfPlayer class
*/

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

    HalfPlayer(Mesh* mesh, const Material& material, bool is_left, const std::string& name = "");

    void init(const Vector3& pos) override;

    void moveControl(Vector3& move_dir, const float dt) override;
};
