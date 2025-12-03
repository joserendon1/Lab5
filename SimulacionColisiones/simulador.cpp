#include "simulador.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>

Simulador::Simulador(double ancho, double alto, double dt)
    : anchoCaja(ancho), altoCaja(alto), dt(dt),
    gravedadX(0), gravedadY(9.8),
    tiempoActual(0), colisionesTotales(0) {}

void Simulador::configurarCaja(double ancho, double alto) {
    anchoCaja = ancho;
    altoCaja = alto;
}

void Simulador::agregarParticula(const Particula& particula) {
    particulas.push_back(particula);
}

void Simulador::agregarObstaculo(const Obstaculo& obstaculo) {
    obstaculos.push_back(obstaculo);
}

void Simulador::configurarGravedad(double gx, double gy) {
    gravedadX = gx;
    gravedadY = gy;
}

void Simulador::simular(double tiempoTotal) {
    // Abrir archivo para guardar datos
    archivoSalida.open("simulacion_datos.txt");
    if (!archivoSalida) {
        std::cerr << "Error al abrir archivo de salida" << std::endl;
        return;
    }

    // Encabezado del archivo
    archivoSalida << "Tiempo,NumParticulas,";
    for (size_t i = 0; i < particulas.size(); ++i) {
        archivoSalida << "P" << i << "_X,P" << i << "_Y,";
    }
    archivoSalida << "Colisiones\n";

    std::cout << "Iniciando simulacion..." << std::endl;
    std::cout << "Tiempo total: " << tiempoTotal << " segundos" << std::endl;
    std::cout << "Paso de tiempo: " << dt << " segundos" << std::endl;
    std::cout << "Particulas: " << particulas.size() << std::endl;
    std::cout << "Obstaculos: " << obstaculos.size() << std::endl;

    int pasos = static_cast<int>(tiempoTotal / dt);

    for (int paso = 0; paso < pasos; ++paso) {
        pasoSimulacion();

        // Guardar datos cada 10 pasos (para reducir tamaño de archivo)
        if (paso % 10 == 0) {
            guardarDatos(""); // Guardar en archivo interno
        }

        // Mostrar progreso cada 100 pasos
        if (paso % 100 == 0) {
            std::cout << "Progreso: " << std::fixed << std::setprecision(1)
            << (paso * 100.0 / pasos) << "%" << std::endl;
        }
    }

    archivoSalida.close();
    std::cout << "\nSimulacion completada!" << std::endl;
    std::cout << "Colisiones totales: " << colisionesTotales << std::endl;
    std::cout << "Datos guardados en: simulacion_datos.txt" << std::endl;
}

void Simulador::pasoSimulacion() {
    // Aplicar gravedad
    aplicarGravedad();

    // Actualizar posiciones
    for (auto& p : particulas) {
        if (p.estaActiva()) {
            p.actualizarPosicion(dt);
        }
    }

    // Verificar colisiones
    verificarColisionesParedes();
    verificarColisionesObstaculos();
    verificarColisionesParticulas();

    tiempoActual += dt;
}

void Simulador::aplicarGravedad() {
    for (auto& p : particulas) {
        if (p.estaActiva()) {
            p.aplicarFuerza(0, p.getMasa() * gravedadY, dt);
        }
    }
}

void Simulador::verificarColisionesParedes() {
    for (auto& p : particulas) {
        if (!p.estaActiva()) continue;

        double x = p.getX();
        double y = p.getY();
        double radio = p.getRadio();
        double vx = p.getVx();
        double vy = p.getVy();
        bool colision = false;

        // Pared izquierda
        if (x - radio <= 0) {
            p.setPosicion(radio + 0.1, y);
            p.setVelocidad(-vx, vy);  // Perfectamente elástico
            colision = true;
        }
        // Pared derecha
        else if (x + radio >= anchoCaja) {
            p.setPosicion(anchoCaja - radio - 0.1, y);
            p.setVelocidad(-vx, vy);  // Perfectamente elástico
            colision = true;
        }
        // Techo
        if (y - radio <= 0) {
            p.setPosicion(x, radio + 0.1);
            p.setVelocidad(vx, -vy);  // Perfectamente elástico
            colision = true;
        }
        // Suelo
        else if (y + radio >= altoCaja) {
            p.setPosicion(x, altoCaja - radio - 0.1);
            p.setVelocidad(vx, -vy);  // Perfectamente elástico
            colision = true;
        }

        if (colision) {
            colisionesTotales++;
            registrarColision("Pared");
        }
    }
}

void Simulador::verificarColisionesObstaculos() {
    for (auto& p : particulas) {
        if (!p.estaActiva()) continue;

        for (const auto& obs : obstaculos) {
            if (obs.colisionaConParticula(p.getX(), p.getY(), p.getRadio())) {
                double x = p.getX();
                double y = p.getY();
                double vx = p.getVx();
                double vy = p.getVy();
                double radio = p.getRadio();

                // Manejar la colisión
                obs.manejarColisionParticula(x, y, vx, vy, radio);
                p.setPosicion(x, y);
                p.setVelocidad(vx, vy);

                colisionesTotales++;
                registrarColision("Obstaculo");
                break; // Solo una colisión por paso
            }
        }
    }
}

void Simulador::verificarColisionesParticulas() {
    for (size_t i = 0; i < particulas.size(); ++i) {
        if (!particulas[i].estaActiva()) continue;

        for (size_t j = i + 1; j < particulas.size(); ++j) {
            if (!particulas[j].estaActiva()) continue;

            if (particulas[i].colisionaCon(particulas[j])) {
                // Colisión completamente inelástica (se unen)
                particulas[i].colisionarInelastica(particulas[j]);

                colisionesTotales++;
                std::string msg = "Particulas " + std::to_string(i) +
                                  " y " + std::to_string(j) + " se unieron";
                registrarColision(msg);
                break; // Solo una colisión por paso
            }
        }
    }
}

void Simulador::guardarDatos(const std::string& nombreArchivo) {
    if (archivoSalida.is_open()) {
        archivoSalida << tiempoActual << "," << particulas.size() << ",";

        for (const auto& p : particulas) {
            archivoSalida << p.getX() << "," << p.getY() << ",";
        }

        archivoSalida << colisionesTotales << "\n";
    }
}

void Simulador::mostrarEstado() const {
    std::cout << "\n=== ESTADO DE LA SIMULACION ===" << std::endl;
    std::cout << "Tiempo: " << tiempoActual << " s" << std::endl;
    std::cout << "Particulas activas: "
              << std::count_if(particulas.begin(), particulas.end(),
                               [](const Particula& p) { return p.estaActiva(); })
              << std::endl;
    std::cout << "Colisiones totales: " << colisionesTotales << std::endl;

    std::cout << "\nParticulas:" << std::endl;
    for (const auto& p : particulas) {
        std::cout << p.toString() << std::endl;
    }

    std::cout << "\nUltimas 5 colisiones:" << std::endl;
    int inicio = std::max(0, static_cast<int>(historialColisiones.size()) - 5);
    for (size_t i = inicio; i < historialColisiones.size(); ++i) {
        std::cout << "- " << historialColisiones[i] << std::endl;
    }
}

void Simulador::registrarColision(const std::string& tipo) {
    std::string registro = "T=" + std::to_string(tiempoActual) +
                           " - " + tipo;
    historialColisiones.push_back(registro);
}
