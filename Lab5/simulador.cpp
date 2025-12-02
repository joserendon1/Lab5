// simulador.cpp
#include "simulador.h"
#include <fstream>
#include <cmath>
#include <iostream>

Simulador::Simulador(double ancho, double alto, double dt)
    : anchoCaja(ancho), altoCaja(alto), dt(dt), tiempoTotal(0) {}

void Simulador::agregarParticula(const Particula& p) {
    particulas.push_back(p);
}

void Simulador::agregarObstaculo(const Obstaculo& o) {
    obstaculos.push_back(o);
}

bool Simulador::verificarColisionParticulaPared(const Particula& p) const {
    double x = p.getX();
    double y = p.getY();
    double radio = p.getRadio();

    // Colisión con paredes izquierda/derecha
    if (x - radio <= 0 || x + radio >= anchoCaja) return true;

    // Colisión con paredes superior/inferior
    if (y - radio <= 0 || y + radio >= altoCaja) return true;

    return false;
}

bool Simulador::verificarColisionParticulaObstaculo(const Particula& p, int& indiceObstaculo) const {
    for (size_t i = 0; i < obstaculos.size(); ++i) {
        if (obstaculos[i].colisionaCon(p)) {
            indiceObstaculo = i;
            return true;
        }
    }
    return false;
}

bool Simulador::verificarColisionEntreParticulas(int i, int j) const {
    if (i == j) return false;

    const Particula& p1 = particulas[i];
    const Particula& p2 = particulas[j];

    if (!p1.estaActiva() || !p2.estaActiva()) return false;

    double dx = p2.getX() - p1.getX();
    double dy = p2.getY() - p1.getY();
    double distancia = sqrt(dx*dx + dy*dy);

    return distancia <= (p1.getRadio() + p2.getRadio());
}

void Simulador::manejarColisionPared(Particula& p) {
    double x = p.getX();
    double y = p.getY();
    double radio = p.getRadio();
    double vx = p.getVx();
    double vy = p.getVy();

    // Colisión con paredes verticales
    if (x - radio <= 0 || x + radio >= anchoCaja) {
        p.setVelocidad(-vx, vy); // Invertir componente horizontal
    }

    // Colisión con paredes horizontales
    if (y - radio <= 0 || y + radio >= altoCaja) {
        p.setVelocidad(vx, -vy); // Invertir componente vertical
    }
}

void Simulador::manejarColisionObstaculo(Particula& p, const Obstaculo& o) {
    double x = p.getX();
    double y = p.getY();
    double radio = p.getRadio();

    // Determinar el lado de colisión
    double closestX = std::max(o.getX(), std::min(x, o.getXFin()));
    double closestY = std::max(o.getY(), std::min(y, o.getYFin()));

    double distanciaX = x - closestX;
    double distanciaY = y - closestY;

    double vx = p.getVx();
    double vy = p.getVy();

    if (fabs(distanciaX) > fabs(distanciaY)) {
        // Colisión horizontal (izquierda/derecha)
        p.setVelocidad(-o.getCoeficienteRestitucion() * vx, vy);
    } else {
        // Colisión vertical (arriba/abajo)
        p.setVelocidad(vx, -o.getCoeficienteRestitucion() * vy);
    }
}

void Simulador::manejarColisionEntreParticulas(Particula& p1, Particula& p2) {
    // Colisión completamente inelástica: se unen
    double m1 = p1.getMasa();
    double m2 = p2.getMasa();
    double M = m1 + m2;

    double vx1 = p1.getVx();
    double vy1 = p1.getVy();
    double vx2 = p2.getVx();
    double vy2 = p2.getVy();

    // Conservación del momento lineal
    double vx_final = (m1 * vx1 + m2 * vx2) / M;
    double vy_final = (m1 * vy1 + m2 * vy2) / M;

    // Nueva posición (centro de masa)
    double x_final = (m1 * p1.getX() + m2 * p2.getX()) / M;
    double y_final = (m1 * p1.getY() + m2 * p2.getY()) / M;

    // Nuevo radio (conservando área total)
    double area1 = M_PI * p1.getRadio() * p1.getRadio();
    double area2 = M_PI * p2.getRadio() * p2.getRadio();
    double radio_final = sqrt((area1 + area2) / M_PI);

    // Actualizar p1 con los nuevos valores
    p1.setPosicion(x_final, y_final);
    p1.setVelocidad(vx_final, vy_final);

    // Desactivar p2
    p2.desactivar();
}

void Simulador::simular(double tiempoTotal) {
    std::ofstream archivo("simulacion.txt");

    for (double t = 0; t < tiempoTotal; t += dt) {
        // Actualizar posiciones
        for (auto& p : particulas) {
            if (p.estaActiva()) {
                p.actualizarPosicion(dt);
            }
        }

        // Verificar colisiones con paredes
        for (auto& p : particulas) {
            if (p.estaActiva() && verificarColisionParticulaPared(p)) {
                manejarColisionPared(p);
            }
        }

        // Verificar colisiones con obstáculos
        for (auto& p : particulas) {
            if (!p.estaActiva()) continue;

            int indiceObstaculo = -1;
            if (verificarColisionParticulaObstaculo(p, indiceObstaculo)) {
                manejarColisionObstaculo(p, obstaculos[indiceObstaculo]);
            }
        }

        // Verificar colisiones entre partículas
        for (size_t i = 0; i < particulas.size(); ++i) {
            for (size_t j = i + 1; j < particulas.size(); ++j) {
                if (verificarColisionEntreParticulas(i, j)) {
                    manejarColisionEntreParticulas(particulas[i], particulas[j]);
                }
            }
        }

        // Limpiar partículas inactivas (opcional)
        limpiarParticulasInactivas();

        // Exportar datos para este paso de tiempo
        archivo << t << " ";
        for (const auto& p : particulas) {
            if (p.estaActiva()) {
                archivo << p.getX() << " " << p.getY() << " ";
            } else {
                archivo << "NaN NaN "; // Marcador para partícula inactiva
            }
        }
        archivo << "\n";
    }

    archivo.close();
}

void Simulador::limpiarParticulasInactivas() {
    // Opcional: remover partículas inactivas del vector
    // Por simplicidad, las mantenemos pero marcadas como inactivas
}

void Simulador::exportarDatos(const std::string& nombreArchivo) {
    // Ya se exportan durante la simulación
}
