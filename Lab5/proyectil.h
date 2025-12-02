#ifndef PROYECTIL_H
#define PROYECTIL_H

#include "particula.h"
#include <QColor>

class Proyectil : public Particula {
private:
    int danioBase;
    QColor color;
    bool enMovimiento;
    bool impacto;

public:
    Proyectil(double x, double y, double vx, double vy, double masa, double radio,
              int danioBase = 10, QColor color = Qt::red);

    int getDanioBase() const { return danioBase; }
    QColor getColor() const { return color; }
    bool estaEnMovimiento() const { return enMovimiento; }
    bool haImpactado() const { return impacto; }

    void setEnMovimiento(bool mov) { enMovimiento = mov; }
    void setImpacto(bool imp) { impacto = imp; }

    void actualizarPosicion(double dt);
    int calcularDanio() const;
};

#endif
