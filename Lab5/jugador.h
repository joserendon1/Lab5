#ifndef JUGADOR_H
#define JUGADOR_H

#include <QColor>
#include <QString>
#include "infraestructura.h"
#include <vector>

// Declaración anticipada
class QPainter;

class Jugador {
private:
    int numero;
    double x, y;  // Posición del jugador
    QColor color;
    double anguloCanon;  // Cambié el nombre para evitar problemas de encoding
    double potenciaDisparo;
    bool estaVivo;
    std::vector<Infraestructura> defensas;  // Estructuras defensivas

public:
    Jugador(int numero, double x, double y, QColor color);

    // Getters
    int getNumero() const { return numero; }
    double getX() const { return x; }
    double getY() const { return y; }
    double getAnguloCanon() const { return anguloCanon; }
    double getPotenciaDisparo() const { return potenciaDisparo; }
    QColor getColor() const { return color; }
    bool estaVivoJugador() const { return estaVivo; }
    const std::vector<Infraestructura>& getDefensas() const { return defensas; }
    std::vector<Infraestructura>& getDefensas() { return defensas; }  // Versión no-const

    // Setters
    void setAnguloCanon(double angulo);
    void setPotenciaDisparo(double potencia);
    void recibirDanio(double danio);

    // Métodos
    void agregarDefensa(const Infraestructura& defensa);
    bool todasDefensasDestruidas() const;

    // Dibujar jugador
    void dibujar(QPainter& painter) const;
};

#endif
