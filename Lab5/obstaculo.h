#ifndef OBSTACULO_H
#define OBSTACULO_H

class Particula;

class Obstaculo {
private:
    double x, y;
    double ancho, alto;
    double coeficienteRestitucion;

public:
    Obstaculo(double x, double y, double ancho, double alto, double coeficienteRestitucion = 0.5);

    double getX() const { return x; }
    double getY() const { return y; }
    double getAncho() const { return ancho; }
    double getAlto() const { return alto; }
    double getCoeficienteRestitucion() const { return coeficienteRestitucion; }

    void setCoeficienteRestitucion(double coeficiente) { coeficienteRestitucion = coeficiente; }

    double getXCentro() const { return x + ancho / 2.0; }
    double getYCentro() const { return y + alto / 2.0; }
    double getXFin() const { return x + ancho; }
    double getYFin() const { return y + alto; }

    bool colisionaCon(const Particula& particula) const;
};

#endif
