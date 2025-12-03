#include "entidad.h"
#include <cmath>
#include <algorithm>

Entidad::Entidad(Tipo tipo, double x, double y, double ancho, double alto,
                 QColor color, int vida, int vidaMaxima)
    : tipo(tipo), x(x), y(y), ancho(ancho), alto(alto),
    color(color), vida(vida), vidaMaxima(vidaMaxima) {}

void Entidad::actualizarPosicion(double dt) {
    if (!activa) return;

    x += vx * dt;
    y += vy * dt;
}

void Entidad::recibirDanio(int cantidad) {
    if (!activa || vida <= 0) return;

    vida -= cantidad;
    if (vida <= 0) {
        vida = 0;
        activa = false;
        // Cambiar color cuando se destruye
        color = QColor(100, 100, 100, 150);
    }
}

bool Entidad::colisionaCon(const Entidad& otra) const {
    if (!activa || !otra.activa) return false;

    // Para entidades circulares (proyectiles, jugadores)
    if (esProyectil() || esJugador() || otra.esProyectil() || otra.esJugador()) {
        double radio1 = getRadio();
        double radio2 = otra.getRadio();

        double dx = x - otra.x;
        double dy = y - otra.y;
        double distancia = sqrt(dx*dx + dy*dy);

        return distancia <= (radio1 + radio2);
    }
    // Para entidades rectangulares (defensas)
    else {
        return !(x > otra.x + otra.ancho ||
                 x + ancho < otra.x ||
                 y > otra.y + otra.alto ||
                 y + alto < otra.y);
    }
}

bool Entidad::contienePunto(double px, double py) const {
    if (!activa) return false;

    if (esProyectil() || esJugador()) {
        // Para entidades circulares
        double radio = getRadio();
        double dx = px - x;
        double dy = py - y;
        return (dx*dx + dy*dy) <= (radio * radio);
    } else {
        // Para entidades rectangulares
        return (px >= x && px <= x + ancho && py >= y && py <= y + alto);
    }
}
