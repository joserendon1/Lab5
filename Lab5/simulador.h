#ifndef SIMULADOR_H
#define SIMULADOR_H

#include <vector>
#include <string>
#include "particula.h"
#include "obstaculo.h"

class Simulador {
private:
    double anchoCaja;
    double altoCaja;
    double tiempoTotal;
    double dt;
    std::vector<Particula> particulas;
    std::vector<Obstaculo> obstaculos;
    std::string archivoSalida;

public:
    Simulador(double ancho, double alto, double dt = 0.01);

    void agregarParticula(const Particula& p);
    void agregarObstaculo(const Obstaculo& o);

    void simular(double tiempoTotal);

    bool verificarColisionParticulaPared(const Particula& p) const;
    bool verificarColisionParticulaObstaculo(const Particula& p, int& indiceObstaculo) const;
    bool verificarColisionEntreParticulas(int i, int j) const;

    void manejarColisionPared(Particula& p);
    void manejarColisionObstaculo(Particula& p, const Obstaculo& o);
    void manejarColisionEntreParticulas(Particula& p1, Particula& p2);

    void exportarDatos(const std::string& nombreArchivo);

    void limpiarParticulasInactivas();

    // Getters
    const std::vector<Particula>& getParticulas() const { return particulas; }
    const std::vector<Obstaculo>& getObstaculos() const { return obstaculos; }
};

#endif
