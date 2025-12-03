#include "simulador.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::cout << "========================================" << std::endl;
    std::cout << "SIMULACION DE SISTEMA DE COLISIONES" << std::endl;
    std::cout << "========================================" << std::endl;

    Simulador simulador(800, 600, 0.01);

    simulador.configurarGravedad(0, 9.8);

    simulador.agregarObstaculo(Obstaculo(100, 100, 60, 60, 0.7));
    simulador.agregarObstaculo(Obstaculo(600, 100, 60, 60, 0.7));
    simulador.agregarObstaculo(Obstaculo(100, 400, 60, 60, 0.7));
    simulador.agregarObstaculo(Obstaculo(600, 400, 60, 60, 0.7));

    std::cout << "\nObstaculos creados en las esquinas del centro." << std::endl;
    std::cout << "\nCreando particulas" << std::endl;
    simulador.agregarParticula(Particula(400, 300, 50, -50, 15, 2.0, 1));
    std::cout << "Particula 1: Centro, velocidad (50, -50)" << std::endl;

    simulador.agregarParticula(Particula(100, 200, 80, 0, 12, 1.5, 2));
    std::cout << "Particula 2: Izquierda, velocidad (80, 0)" << std::endl;

    simulador.agregarParticula(Particula(700, 200, -60, 20, 10, 1.0, 3));
    std::cout << "Particula 3: Derecha, velocidad (-60, 20)" << std::endl;

    simulador.agregarParticula(Particula(400, 500, 0, -100, 8, 0.8, 4));
    std::cout << "Particula 4: Abajo, velocidad (0, -100)" << std::endl;

    simulador.agregarParticula(Particula(200, 100, 40, 40, 14, 2.5, 5));
    std::cout << "Particula 5: Superior izquierda, velocidad (40, 40)" << std::endl;

    std::cout << "\nTotal: " << simulador.getNumeroParticulas() << " particulas creadas." << std::endl;

    std::cout << "\n=== CONFIGURACION INICIAL ===" << std::endl;
    simulador.mostrarEstado();

    std::cout << "\nPresiona Enter para iniciar la simulacion";
    std::cin.get();

    simulador.simular(10.0);
    simulador.mostrarEstado();

    std::cout << "\n========================================" << std::endl;
    std::cout << "RESULTADOS" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\nEl archivo 'simulacion_datos.txt' contiene:" << std::endl;
    std::cout << "1. Tiempo de simulacion" << std::endl;
    std::cout << "2. Numero de particulas activas" << std::endl;
    std::cout << "3. Posiciones (x,y) de cada particula" << std::endl;
    std::cout << "4. Numero de colisiones acumuladas" << std::endl;
    std::cout << "\nPresiona Enter para terminar...";
    std::cin.get();

    return 0;
}
