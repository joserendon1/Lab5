#ifndef JUEGO_H
#define JUEGO_H

#include "entidad.h"
#include <QObject>
#include <vector>
#include <optional>

class Juego : public QObject {
    Q_OBJECT

public:
    enum Estado {
        CONFIGURANDO_DISPARO,
        PROYECTIL_EN_VUELO,
        TURNO_COMPLETADO,
        JUEGO_TERMINADO
    };

    explicit Juego(double ancho = 1200, double alto = 600, QObject* parent = nullptr);

    // Getters
    Estado getEstado() const { return estado; }
    int getTurno() const { return turno; }
    double getAnchoEscenario() const { return anchoEscenario; }
    double getAltoEscenario() const { return altoEscenario; }
    double getSueloY() const { return sueloY; }

    Entidad* getJugadorActual();
    Entidad* getJugador1() { return &jugador1; }
    Entidad* getJugador2() { return &jugador2; }
    Entidad* getJugador(int numero);
    Entidad* getProyectilActual();

    // Métodos de juego
    bool prepararDisparo(double angulo, double potencia);
    void actualizar(double dt);
    void finalizarTurno();

    // Configuración física
    void setGravedad(double g) { gravedad = g; }
    void setCoeficienteRestitucion(double cr) { coeficienteRestitucion = cr; }

    // Estado del juego
    bool verificarFinJuego() const;
    Entidad* obtenerGanador();

    // Acceso a defensas para dibujo
    const std::vector<Entidad>& getDefensasJugador1() const { return defensasJugador1; }
    const std::vector<Entidad>& getDefensasJugador2() const { return defensasJugador2; }

private slots:
    void cambiarTurnoAutomatico();

private:
    // Estado del juego
    double anchoEscenario;
    double altoEscenario;
    double sueloY;
    int turno = 1;
    Estado estado = CONFIGURANDO_DISPARO;

    // Entidades
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
    bool verificarColisionConJugador(const Entidad& proyectil, Entidad& jugador);
    void empujarProyectilFueraDeDefensas(Entidad& proyectil, int jugadorObjetivo);
};

#endif
