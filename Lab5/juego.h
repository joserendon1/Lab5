#ifndef JUEGO_H
#define JUEGO_H

#include "entidad.h"
#include <vector>
#include <optional>

class Juego {
public:
    enum Estado {
        CONFIGURANDO_DISPARO,
        PROYECTIL_EN_VUELO,
        TURNO_COMPLETADO,
        JUEGO_TERMINADO
    };

    Juego(double ancho = 1200, double alto = 600);

    // Getters
    Estado getEstado() const { return estado; }
    int getTurno() const { return turno; }
    Entidad* getJugadorActual();
    Entidad* getJugador1() { return jugadorActual == &jugador1 ? &jugador1 : &jugador2; }
    Entidad* getJugador2() { return jugadorActual == &jugador1 ? &jugador2 : &jugador1; }
    double getAnchoEscenario() const { return anchoEscenario; }
    double getAltoEscenario() const { return altoEscenario; }
    Entidad* getProyectilActual();

    // Métodos para mantener compatibilidad temporal
    bool prepararDisparo(double angulo, double potencia, double masa = 10.0);
    void actualizar(double dt);
    void finalizarTurno();

    // Métodos originales que necesitan mantenerse
    void setGravedad(double g) { gravedad = g; }
    void setCoeficienteRestitucion(double cr) { coeficienteRestitucion = cr; }

    bool verificarFinJuego() const;
    Entidad* obtenerGanador();

    // Para mantener compatibilidad con el dibujo
    std::vector<Entidad>& getDefensasJugador1() { return defensasJugador1; }
    std::vector<Entidad>& getDefensasJugador2() { return defensasJugador2; }

    // Método auxiliar para obtener jugador por número
    Entidad* getJugador(int numero);

private:
    // Estado del juego
    double anchoEscenario, altoEscenario;
    int turno = 1;
    Estado estado = CONFIGURANDO_DISPARO;

    // Entidades principales
    Entidad jugador1;
    Entidad jugador2;
    Entidad* jugadorActual;

    std::vector<Entidad> defensasJugador1;
    std::vector<Entidad> defensasJugador2;
    std::optional<Entidad> proyectilActual;

    // Física
    double gravedad = 9.8;
    double coeficienteRestitucion = 0.7;

    // Métodos privados
    void inicializarEscenario();
    bool verificarColisionConParedes(const Entidad& entidad) const;
    void manejarColisionPared(Entidad& entidad);
    bool verificarColisionConDefensas(Entidad& proyectil, int jugadorObjetivo);
    void manejarColisionDefensa(Entidad& proyectil, Entidad& defensa);
    void aplicarGravedad(Entidad& entidad, double dt);
};

#endif
