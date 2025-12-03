#include "entidad.h"
#include <cmath>
#include <algorithm>

Entidad::Entidad(Tipo tipo, double x, double y, double ancho, double alto,
                 QColor color, int vida, int vidaMaxima)
    : tipo(tipo), x(x), y(y), ancho(ancho), alto(alto),
    color(color), vida(vida), vidaMaxima(vidaMaxima)
{
    if (tipo == TIPO_PROYECTIL || tipo == TIPO_JUGADOR) {
        radio = std::min(ancho, alto) / 2.0;
    }
}

void Entidad::actualizarPosicion(double dt)
{
    if (!activa) return;
    x += vx * dt;
    y += vy * dt;
}

void Entidad::recibirDanio(int cantidad)
{
    if (!activa || vida <= 0) return;

    vida -= cantidad;
    if (vida <= 0) {
        vida = 0;
        activa = false;
        color = QColor(100, 100, 100, 150);
    }
}

QRectF Entidad::getBoundingBox() const
{
    return QRectF(x, y, ancho, alto);
}

bool Entidad::colisionaCirculoConRectangulo(const Entidad& rectangulo) const
{
    double centroX = x + ancho/2;
    double centroY = y + alto/2;
    double radioCirculo = getRadio();

    double rectLeft = rectangulo.getX();
    double rectRight = rectangulo.getX() + rectangulo.getAncho();
    double rectTop = rectangulo.getY();
    double rectBottom = rectangulo.getY() + rectangulo.getAlto();

    double closestX = std::max(rectLeft, std::min(centroX, rectRight));
    double closestY = std::max(rectTop, std::min(centroY, rectBottom));

    double distanceX = centroX - closestX;
    double distanceY = centroY - closestY;

    return (distanceX * distanceX + distanceY * distanceY) <= (radioCirculo * radioCirculo);
}

bool Entidad::colisionaRectanguloConCirculo(const Entidad& circulo) const
{
    return circulo.colisionaCirculoConRectangulo(*this);
}

bool Entidad::colisionaCon(const Entidad& otra) const
{
    if (!activa || !otra.activa) return false;

    // Círculo vs Rectángulo
    if ((esProyectil() || esJugador()) && otra.esDefensa()) {
        return colisionaCirculoConRectangulo(otra);
    }

    // Rectángulo vs Círculo
    if (esDefensa() && (otra.esProyectil() || otra.esJugador())) {
        return colisionaRectanguloConCirculo(otra);
    }

    // Círculo vs Círculo
    if ((esProyectil() || esJugador()) && (otra.esProyectil() || otra.esJugador())) {
        double centroX1 = x + ancho/2;
        double centroY1 = y + alto/2;
        double centroX2 = otra.x + otra.ancho/2;
        double centroY2 = otra.y + otra.alto/2;

        double dx = centroX1 - centroX2;
        double dy = centroY1 - centroY2;
        double distancia = std::sqrt(dx*dx + dy*dy);

        return distancia <= (getRadio() + otra.getRadio());
    }

    // Rectángulo vs Rectángulo
    if (esDefensa() && otra.esDefensa()) {
        return !(x > otra.x + otra.ancho ||
                 x + ancho < otra.x ||
                 y > otra.y + otra.alto ||
                 y + alto < otra.y);
    }

    return false;
}

bool Entidad::contienePunto(double px, double py) const
{
    if (!activa) return false;

    if (esProyectil() || esJugador()) {
        double centroX = x + ancho/2;
        double centroY = y + alto/2;
        double radio = getRadio();

        double dx = px - centroX;
        double dy = py - centroY;
        return (dx*dx + dy*dy) <= (radio * radio);
    } else {
        return (px >= x && px <= x + ancho && py >= y && py <= y + alto);
    }
}
