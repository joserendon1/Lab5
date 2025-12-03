#ifndef SIMULADOR_H
#define SIMULADOR_H

#include "particula.h"
#include "obstaculo.h"
#include <vector>
#include <string>
#include <fstream>

class Simulador {
public:
    // Constructor
    Simulador(double ancho = 800, double alto = 600, double dt = 0.016);

    // Configuración
    void configurarCaja(double ancho, double alto);
    void agregarParticula(const Particula& particula);
    void agregarObstaculo(const Obstaculo& obstaculo);
    void configurarGravedad(double gx, double gy);

    // Simulación
    void simular(double tiempoTotal);
    void pasoSimulacion();

    // Salida de datos
    void guardarDatos(const std::string& nombreArchivo);
    void mostrarEstado() const;

    // Getters
    double getTiempoActual() const { return tiempoActual; }
    int getNumeroParticulas() const { return particulas.size(); }
    int getNumeroColisiones() const { return colisionesTotales; }

private:
    // Métodos de simulación
    void aplicarGravedad();
    void verificarColisionesParedes();
    void verificarColisionesObstaculos();
    void verificarColisionesParticulas();

    // Métodos auxiliares
    void manejarColisionPared(Particula& p);
    void registrarColision(const std::string& tipo);

    // Datos de simulación
    std::vector<Particula> particulas;
    std::vector<Obstaculo> obstaculos;
    std::vector<std::string> historialColisiones;

    // Configuración
    double anchoCaja, altoCaja;
    double gravedadX, gravedadY;
    double dt;  // Paso de tiempo
    double tiempoActual;
    int colisionesTotales;

    // Archivo de salida
    std::ofstream archivoSalida;
};

#endif // SIMULADOR_H
