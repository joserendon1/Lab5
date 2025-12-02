#include "infraestructura.h"
#include <algorithm>
#include <cmath>

Infraestructura::Infraestructura(double x, double y, double ancho, double alto,
                                 int resistencia, QColor color, QString nombre)
    : x(x), y(y), ancho(ancho), alto(alto),
    resistenciaMaxima(resistencia), resistenciaActual(resistencia),
    color(color), nombre(nombre), destruida(false) {}

bool Infraestructura::contienePunto(double px, double py) const {
    return (px >= x && px <= x + ancho && py >= y && py <= y + alto);
}

bool Infraestructura::colisionaConProyectil(double px, double py, double radio) const {
    if (destruida) return false;

    // Encontrar el punto más cercano en el rectángulo al centro del proyectil
    double closestX = std::max(x, std::min(px, x + ancho));
    double closestY = std::max(y, std::min(py, y + alto));

    // Calcular distancia
    double distanciaX = px - closestX;
    double distanciaY = py - closestY;

    return (distanciaX * distanciaX + distanciaY * distanciaY) <= (radio * radio);
}

void Infraestructura::recibirDanio(int danio) {
    if (destruida) return;

    // Reducir daño progresivamente si tiene poca vida
    double factor = 1.0;
    double porcentaje = getPorcentajeResistencia();  // Usar getPorcentajeResistencia()
    if (porcentaje < 0.3) {
        factor = 1.2; // Más vulnerable cuando está dañada
    }

    resistenciaActual -= static_cast<int>(danio * factor);

    if (resistenciaActual <= 0) {
        resistenciaActual = 0;
        destruida = true;
        color = QColor(100, 100, 100, 100); // Gris transparente cuando se destruye
    } else {
        // Cambiar color según daño
        porcentaje = getPorcentajeResistencia();  // Recalcular porcentaje
        if (porcentaje < 0.3) {
            color.setRed(color.red() + 50);
            color.setGreen(color.green() - 30);
        }
    }
}

double Infraestructura::getPorcentajeResistencia() const {
    return static_cast<double>(resistenciaActual) / resistenciaMaxima;
}
