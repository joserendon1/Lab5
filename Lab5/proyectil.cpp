#include "proyectil.h"
#include <cmath>

Proyectil::Proyectil(double x, double y, double vx, double vy, double masa, double radio,
                     int danioBase, QColor color)
    : Particula(x, y, vx, vy, masa, radio),
    danioBase(danioBase),
    color(color),
    enMovimiento(false),
    impacto(false) {}

void Proyectil::actualizarPosicion(double dt) {
    if (!estaActiva() || !enMovimiento) return;

    const double g = 9.8 * 60.0;

    double nueva_vy = getVy() + g * dt;

    // Aplicar fricción del aire (solo afecta componente horizontal)
    double factor_friccion = 0.995; // Fricción muy ligera
    double nueva_vx = getVx() * factor_friccion;

    // Fricción vertical solo cuando está en el aire
    if (nueva_vy > 0) { // Cuando está cayendo
        nueva_vy = nueva_vy * 0.998;
    }

    // Actualizar velocidad
    setVelocidad(nueva_vx, nueva_vy);

    // Actualizar posición usando velocidad actualizada
    setPosicion(getX() + nueva_vx * dt, getY() + nueva_vy * dt);
}

int Proyectil::calcularDanio() const {
    double vx = getVx();
    double vy = getVy();

    // Velocidad total (magnitud)
    double velocidad = sqrt(vx*vx + vy*vy);

    // Daño basado en energía cinética: 0.5 * masa * velocidad²
    double energia = 0.5 * getMasa() * velocidad * velocidad;

    // Escalar el daño para que sea razonable (entre 5 y 50)
    int danio = static_cast<int>(energia * 0.01 + danioBase);

    // Limitar daño
    if (danio < 5) danio = 5;
    if (danio > 50) danio = 50;

    return danio;
}
