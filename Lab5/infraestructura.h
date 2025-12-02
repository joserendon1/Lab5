#ifndef INFRAESTRUCTURA_H
#define INFRAESTRUCTURA_H

#include <QColor>
#include <QString>

class Infraestructura {
private:
    double x, y;
    double ancho, alto;
    int resistenciaMaxima;
    int resistenciaActual;
    QColor color;
    QString nombre;
    bool destruida;

public:
    Infraestructura(double x, double y, double ancho, double alto,
                    int resistencia, QColor color = Qt::gray,
                    QString nombre = "Obstaculo");

    // Getters
    double getX() const { return x; }
    double getY() const { return y; }
    double getAncho() const { return ancho; }
    double getAlto() const { return alto; }
    int getResistenciaMaxima() const { return resistenciaMaxima; }
    int getResistenciaActual() const { return resistenciaActual; }
    QColor getColor() const { return color; }
    QString getNombre() const { return nombre; }
    bool estaDestruida() const { return destruida; }
    double getPorcentajeResistencia() const;  // Solo esta declaración

    // Setters
    void setResistenciaActual(int resistencia) { resistenciaActual = resistencia; }
    void setDestruida(bool dest) { destruida = dest; }

    // Métodos de colisión
    bool contienePunto(double px, double py) const;
    bool colisionaConProyectil(double px, double py, double radio) const;

    // Métodos de daño
    void recibirDanio(int danio);
};
#endif
