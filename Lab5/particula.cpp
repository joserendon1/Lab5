#include "particula.h"

Particula::Particula(double x, double y, double vx, double vy, double masa, double radio)
    : x(x), y(y), vx(vx), vy(vy), masa(masa), radio(radio), activa(true) {}

void Particula::actualizarPosicion(double dt) {
    if (!activa) return;

    x += vx * dt;
    y += vy * dt;
}

void Particula::setVelocidad(double vx, double vy) {
    this->vx = vx;
    this->vy = vy;
}

void Particula::setPosicion(double x, double y) {
    this->x = x;
    this->y = y;
}
