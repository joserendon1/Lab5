#ifndef PARTICULA_H
#define PARTICULA_H

class Particula {
private:
    double x, y;
    double vx, vy;
    double masa;
    double radio;
    bool activa;

public:
    Particula(double x, double y, double vx, double vy, double masa, double radio);

    double getX() const { return x; }
    double getY() const { return y; }
    double getVx() const { return vx; }
    double getVy() const { return vy; }
    double getMasa() const { return masa; }
    double getRadio() const { return radio; }
    bool estaActiva() const { return activa; }

    void setVelocidad(double vx, double vy);
    void setPosicion(double x, double y);
    void desactivar() { activa = false; }

    virtual void actualizarPosicion(double dt);
    virtual ~Particula() = default;
};

#endif
