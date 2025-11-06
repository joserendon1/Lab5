#include <iostream>
#include "particula.h"

int main() {
    std::cout << "=== Prueba Clase Particula ===\n";

    Particula p(0.0, 0.0, 1.0, 1.0, 1.0, 0.5);

    std::cout << "Estado inicial:\n";
    std::cout << "Posicion: (" << p.getX() << ", " << p.getY() << ")\n";
    std::cout << "Velocidad: (" << p.getVx() << ", " << p.getVy() << ")\n";
    std::cout << "Masa: " << p.getMasa() << ", Radio: " << p.getRadio() << "\n";
    std::cout << "Activa: " << (p.estaActiva() ? "Si" : "No") << "\n";

    std::cout << "\nMovimiento\n";
    for (int i = 1; i <= 5; i++) {
        p.actualizarPosicion(0.1);
        std::cout << "Paso " << i << ": (" << p.getX() << ", " << p.getY() << ")\n";
    }

    std::cout << "\nCambiando velocidad\n";
    p.setVelocidad(2.0, 0.0);
    p.actualizarPosicion(0.1);
    std::cout << "Nueva posicion: (" << p.getX() << ", " << p.getY() << ")\n";

    std::cout << "\nDesactivando particula...\n";
    p.desactivar();
    p.actualizarPosicion(0.1);
    std::cout << "Posicion despues de desactivar: (" << p.getX() << ", " << p.getY() << ")\n";
    std::cout << "Activa: " << (p.estaActiva() ? "Si" : "No") << "\n";

    return 0;
}
