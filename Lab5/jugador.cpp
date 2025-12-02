#include "jugador.h"
#include <QPainter>
#include <cmath>

Jugador::Jugador(int numero, double x, double y, QColor color)
    : numero(numero), x(x), y(y), color(color),
    anguloCanon(45.0), potenciaDisparo(50.0), estaVivo(true) {}

void Jugador::setAnguloCanon(double angulo) {
    // Limitar ángulo entre 0 y 90 grados para disparos parabólicos
    if (angulo < 0) angulo = 0;
    if (angulo > 90) angulo = 90;
    anguloCanon = angulo;
}

void Jugador::setPotenciaDisparo(double potencia) {
    // Limitar potencia entre 0 y 100
    if (potencia < 0) potencia = 0;
    if (potencia > 100) potencia = 100;
    potenciaDisparo = potencia;
}

void Jugador::recibirDanio(double danio) {
    // Aquí podrías implementar vida del jugador si es necesario
    Q_UNUSED(danio); // Para evitar warning de parámetro no usado
    // Por ahora, el jugador solo muere si pierde todas sus defensas
}

void Jugador::agregarDefensa(const Infraestructura& defensa) {
    defensas.push_back(defensa);
}

bool Jugador::todasDefensasDestruidas() const {
    for (const auto& defensa : defensas) {
        if (!defensa.estaDestruida()) {
            return false;
        }
    }
    return true;
}

void Jugador::dibujar(QPainter& painter) const {
    if (!estaVivo) return;

    // Dibujar cuerpo del jugador (círculo) dentro de la estructura
    painter.save();

    painter.setBrush(QBrush(color));
    painter.setPen(QPen(Qt::black, 2));
    painter.drawEllipse(QPointF(x, y), 15, 15);

    // Dibujar cañón - posición ajustada para que salga del jugador
    double radianes = anguloCanon * M_PI / 180.0;
    double canonLength = 30.0;

    // Calcular dirección del cañón según jugador
    double endX, endY;

    if (numero == 1) {
        // Jugador 1: cañón apunta hacia la derecha
        endX = x + canonLength * cos(radianes);
        endY = y - canonLength * sin(radianes);  // Negativo porque Y crece hacia abajo
    } else {
        // Jugador 2: cañón apunta hacia la izquierda
        endX = x - canonLength * cos(radianes);
        endY = y - canonLength * sin(radianes);
    }

    painter.setPen(QPen(Qt::black, 4));
    painter.drawLine(QPointF(x, y), QPointF(endX, endY));

    painter.restore();
}
