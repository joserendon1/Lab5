#ifndef JUEGO_H
#define JUEGO_H

#include "jugador.h"
#include "proyectil.h"
#include <vector>
#include <memory>

enum EstadoJuego {
    CONFIGURANDO_DISPARO,
    PROYECTIL_EN_VUELO,
    TURNO_COMPLETADO,
    JUEGO_TERMINADO
};

class Juego {
private:
    // Escenario - DIMENSIONES AJUSTADAS
    double anchoEscenario;
    double altoEscenario;

    // Jugadores
    Jugador jugador1;
    Jugador jugador2;

    // Estado del juego
    int turno;
    Jugador* jugadorActual;
    EstadoJuego estado;

    // Proyectil
    std::unique_ptr<Proyectil> proyectilActual;

    // Configuración física
    double gravedad;
    double coeficienteRestitucion;
    double resistenciaParedes;

public:
    // CONSTRUCTOR CON DIMENSIONES MÁS GRANDES
    Juego(double ancho = 1200.0, double alto = 600.0);

    // Configuración inicial
    void inicializarEscenario();

    // Control de juego
    bool prepararDisparo(double angulo, double potencia, double masaProyectil = 5.0);
    void actualizar(double dt);
    void finalizarTurno();

    // Consultas
    Jugador* getJugadorActual() { return jugadorActual; }
    Jugador* getJugador1() { return &jugador1; }
    Jugador* getJugador2() { return &jugador2; }
    int getTurno() const { return turno; }
    EstadoJuego getEstado() const { return estado; }
    Proyectil* getProyectilActual() { return proyectilActual.get(); }
    double getAnchoEscenario() const { return anchoEscenario; }
    double getAltoEscenario() const { return altoEscenario; }

    bool verificarFinJuego() const;
    Jugador* obtenerGanador() const;

    // Verificación de colisiones
    bool verificarColisionProyectilJugador(const Proyectil& p, Jugador& jugador);
    bool verificarColisionProyectilDefensas(Proyectil& p, Jugador& jugador);
    bool verificarColisionConParedes(const Proyectil& p) const;
    void manejarColisionPared(Proyectil& p);
    void manejarColisionDefensa(Proyectil& p, Infraestructura& defensa);

    // Configuración física
    void setGravedad(double g) { gravedad = g; }
    void setCoeficienteRestitucion(double cr) { coeficienteRestitucion = cr; }
    void setResistenciaParedes(double r) { resistenciaParedes = r; }
};

#endif
