#include "obstaculo.h"
#include "particula.h"
#include <cmath>
#include <algorithm>

Obstaculo::Obstaculo(double x, double y, double ancho, double alto, double coeficienteRestitucion)
    : x(x), y(y), ancho(ancho), alto(alto), coeficienteRestitucion(coeficienteRestitucion) {}

bool Obstaculo::colisionaCon(const Particula& particula) const {
    double closestX = std::max(x, std::min(particula.getX(), x + ancho));
    double closestY = std::max(y, std::min(particula.getY(), y + alto));

    double distanceX = particula.getX() - closestX;
    double distanceY = particula.getY() - closestY;
    double distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

    return distanceSquared <= (particula.getRadio() * particula.getRadio());
}
