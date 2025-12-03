#include "particula.h"
#include <cmath>
#include <sstream>

Particula::Particula(double x, double y, double vx, double vy,
                     double radio, double masa, int id)
    : x(x), y(y), vx(vx), vy(vy), radio(radio), masa(masa), id(id), activa(true) {}

void Particula::actualizarPosicion(double dt) {
    if (!activa) return;
    x += vx * dt;
    y += vy * dt;
}

void Particula::aplicarFuerza(double fx, double fy, double dt) {
    if (!activa) return;
    vx += fx / masa * dt;
    vy += fy / masa * dt;
}

bool Particula::colisionaCon(const Particula& otra) const {
    if (!activa || !otra.activa) return false;

    double dx = x - otra.x;
    double dy = y - otra.y;
    double distancia = sqrt(dx*dx + dy*dy);

    return distancia <= (radio + otra.radio);
}

void Particula::colisionarElastica(Particula& otra) {
    // Colisión perfectamente elástica entre dos partículas
    double dx = otra.x - x;
    double dy = otra.y - y;
    double distancia = sqrt(dx*dx + dy*dy);

    if (distancia == 0) return;

    // Normalizar vector de separación
    double nx = dx / distancia;
    double ny = dy / distancia;

    // Velocidades relativas
    double dvx = otra.vx - vx;
    double dvy = otra.vy - vy;

    // Producto punto de velocidad relativa y normal
    double p = dvx * nx + dvy * ny;

    // Masas
    double m1 = masa;
    double m2 = otra.masa;
    double masaTotal = m1 + m2;

    // Coeficiente de restitución (1.0 = perfectamente elástico)
    double e = 1.0;

    // Calcular impulso
    double j = -(1 + e) * p * (m1 * m2) / masaTotal;

    // Aplicar impulso
    vx -= (j * nx) / m1;
    vy -= (j * ny) / m1;
    otra.vx += (j * nx) / m2;
    otra.vy += (j * ny) / m2;
}

void Particula::colisionarInelastica(Particula& otra) {
    // Colisión completamente inelástica (se unen)
    double nuevaMasa = masa + otra.masa;

    // Conservación del momento lineal
    double nuevoVx = (masa * vx + otra.masa * otra.vx) / nuevaMasa;
    double nuevoVy = (masa * vy + otra.masa * otra.vy) / nuevaMasa;

    // Centro de masa para la nueva posición
    double nuevoX = (masa * x + otra.masa * otra.x) / nuevaMasa;
    double nuevoY = (masa * y + otra.masa * otra.y) / nuevaMasa;

    // Radio combinado (aproximación)
    double nuevoRadio = sqrt(radio*radio + otra.radio*otra.radio);

    // Actualizar esta partícula con las propiedades combinadas
    x = nuevoX;
    y = nuevoY;
    vx = nuevoVx;
    vy = nuevoVy;
    masa = nuevaMasa;
    radio = nuevoRadio;

    // Desactivar la otra partícula
    otra.activa = false;
}

std::string Particula::toString() const {
    std::stringstream ss;
    ss << "Particula " << id << ": ";
    ss << "Pos(" << x << ", " << y << ") ";
    ss << "Vel(" << vx << ", " << vy << ") ";
    ss << "Masa: " << masa << " ";
    ss << "Radio: " << radio << " ";
    ss << "Activa: " << (activa ? "Si" : "No");
    return ss.str();
}
