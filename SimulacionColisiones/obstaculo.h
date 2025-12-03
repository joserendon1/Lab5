#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <string>

class Obstaculo {
public:
    // Constructor
    Obstaculo(double x = 0, double y = 0, double ancho = 40, double alto = 40,
              double coeficienteRestitucion = 0.7);

    // Getters
    double getX() const { return x; }
    double getY() const { return y; }
    double getAncho() const { return ancho; }
    double getAlto() const { return alto; }
    double getCoeficienteRestitucion() const { return coeficienteRestitucion; }

    // Setters
    void setPosicion(double x, double y) { this->x = x; this->y = y; }
    void setTamano(double ancho, double alto) { this->ancho = ancho; this->alto = alto; }
    void setCoeficienteRestitucion(double cr) { coeficienteRestitucion = cr; }

    // Colisiones
    bool colisionaConParticula(double px, double py, double radio) const;
    void manejarColisionParticula(double& px, double& py, double& vx, double& vy, double radio) const;

    // Información
    std::string toString() const;

private:
    double x, y;          // Posición (esquina superior izquierda)
    double ancho, alto;   // Dimensiones
    double coeficienteRestitucion;  // 0.0-1.0
};

#endif // OBSTACULO_H
