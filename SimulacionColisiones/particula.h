#ifndef PARTICULA_H
#define PARTICULA_H

#include <string>

class Particula {
public:
    // Constructor
    Particula(double x = 0, double y = 0, double vx = 0, double vy = 0,
              double radio = 10, double masa = 1.0, int id = 0);

    // Getters
    double getX() const { return x; }
    double getY() const { return y; }
    double getVx() const { return vx; }
    double getVy() const { return vy; }
    double getRadio() const { return radio; }
    double getMasa() const { return masa; }
    int getId() const { return id; }
    bool estaActiva() const { return activa; }

    // Setters
    void setPosicion(double x, double y) { this->x = x; this->y = y; }
    void setVelocidad(double vx, double vy) { this->vx = vx; this->vy = vy; }
    void setActiva(bool activa) { this->activa = activa; }
    void setId(int id) { this->id = id; }

    // Métodos de física
    void actualizarPosicion(double dt);
    void aplicarFuerza(double fx, double fy, double dt);

    // Colisiones
    bool colisionaCon(const Particula& otra) const;
    void colisionarElastica(Particula& otra);
    void colisionarInelastica(Particula& otra);

    // Información
    std::string toString() const;

private:
    double x, y;        // Posición
    double vx, vy;      // Velocidad
    double radio;       // Radio de la partícula
    double masa;        // Masa
    int id;             // Identificador único
    bool activa;        // Estado activo/inactivo
};

#endif // PARTICULA_H
