#include "obstaculo.h"
#include <cmath>
#include <sstream>

Obstaculo::Obstaculo(double x, double y, double ancho, double alto,
                     double coeficienteRestitucion)
    : x(x), y(y), ancho(ancho), alto(alto),
    coeficienteRestitucion(coeficienteRestitucion) {}

bool Obstaculo::colisionaConParticula(double px, double py, double radio) const {
    // Encontrar el punto más cercano en el obstáculo a la partícula
    double closestX = std::max(x, std::min(px, x + ancho));
    double closestY = std::max(y, std::min(py, y + alto));

    // Calcular distancia
    double distanceX = px - closestX;
    double distanceY = py - closestY;

    // Verificar si la distancia es menor que el radio
    return (distanceX * distanceX + distanceY * distanceY) <= (radio * radio);
}

void Obstaculo::manejarColisionParticula(double& px, double& py,
                                         double& vx, double& vy, double radio) const {
    // Calcular el centro del obstáculo
    double centroX = x + ancho / 2.0;
    double centroY = y + alto / 2.0;

    // Calcular penetración en cada eje
    double overlapX = (ancho/2 + radio) - std::abs(px - centroX);
    double overlapY = (alto/2 + radio) - std::abs(py - centroY);

    // Determinar lado de colisión
    if (overlapX < overlapY) {
        // Colisión horizontal
        if (px < centroX) {
            // Colisión con lado izquierdo
            px = x - radio - 0.1;
            vx = -std::abs(vx) * coeficienteRestitucion;
        } else {
            // Colisión con lado derecho
            px = x + ancho + radio + 0.1;
            vx = std::abs(vx) * coeficienteRestitucion;
        }
        vy *= 0.9; // Fricción
    } else {
        // Colisión vertical
        if (py < centroY) {
            // Colisión con parte superior
            py = y - radio - 0.1;
            vy = -std::abs(vy) * coeficienteRestitucion;
        } else {
            // Colisión con parte inferior
            py = y + alto + radio + 0.1;
            vy = std::abs(vy) * coeficienteRestitucion;
        }
        vx *= 0.9; // Fricción
    }

    // Pérdida de energía adicional
    vx *= 0.95;
    vy *= 0.95;
}

std::string Obstaculo::toString() const {
    std::stringstream ss;
    ss << "Obstaculo: ";
    ss << "Pos(" << x << ", " << y << ") ";
    ss << "Tamano(" << ancho << "x" << alto << ") ";
    ss << "Coef. Rest: " << coeficienteRestitucion;
    return ss.str();
}
