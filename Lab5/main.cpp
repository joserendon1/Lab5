#include <iostream>
#include "particula.h"
#include "obstaculo.h"

int main() {
    std::cout << "=== Prueba Clase Obstaculo ===\n";

    Particula p(5.0, 5.0, 1.0, 0.0, 1.0, 1.0);

    Obstaculo obstaculo(10.0, 0.0, 4.0, 10.0, 0.7);

    std::cout << "Obstaculo creado:\n";
    std::cout << "Posicion: (" << obstaculo.getX() << ", " << obstaculo.getY() << ")\n";
    std::cout << "Dimensiones: " << obstaculo.getAncho() << " x " << obstaculo.getAlto() << "\n";
    std::cout << "Coeficiente restitucion: " << obstaculo.getCoeficienteRestitucion() << "\n";
    std::cout << "Centro: (" << obstaculo.getXCentro() << ", " << obstaculo.getYCentro() << ")\n";

    std::cout << "\nProbando deteccion\n";

    p.setPosicion(5.0, 5.0);
    std::cout << "Particula en (5,5): " << (obstaculo.colisionaCon(p) ? "COLISION" : "No colisiona") << "\n";

    p.setPosicion(9.0, 5.0);
    std::cout << "Particula en (9,5): " << (obstaculo.colisionaCon(p) ? "COLISION" : "No colisiona") << "\n";

    p.setPosicion(9.5, 5.0);
    std::cout << "Particula en (9.5,5): " << (obstaculo.colisionaCon(p) ? "COLISION" : "No colisiona") << "\n";

    p.setPosicion(11.0, 5.0);
    std::cout << "Particula en (11,5): " << (obstaculo.colisionaCon(p) ? "COLISION" : "No colisiona") << "\n";

    std::cout << "\nCambiando coeficiente de restitucion\n";
    obstaculo.setCoeficienteRestitucion(0.3);
    std::cout << "Nuevo coeficiente: " << obstaculo.getCoeficienteRestitucion() << "\n";

    return 0;
}
