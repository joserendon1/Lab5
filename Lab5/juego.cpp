#include "juego.h"
#include <cmath>
#include <QTimer>
#include <limits>

Juego::Juego(double ancho, double alto, QObject* parent)
    : QObject(parent), anchoEscenario(ancho), altoEscenario(alto), sueloY(alto - 50)
{
    jugador1 = Entidad(Entidad::TIPO_JUGADOR, 180, sueloY - 15, 30, 30, QColor(100, 150, 200), 100);
    jugador1.setPropietario(1);

    jugador2 = Entidad(Entidad::TIPO_JUGADOR, ancho - 160, sueloY - 15, 30, 30, QColor(200, 100, 150), 100);
    jugador2.setPropietario(2);

    jugadorActual = &jugador1;
    inicializarEscenario();
}

void Juego::inicializarEscenario()
{
    defensasJugador1.clear();
    defensasJugador2.clear();

    // Defensas jugador 1
    defensasJugador1.push_back(Entidad(Entidad::TIPO_DEFENSA, 100, sueloY - 120,
                                       40, 120, QColor(180, 180, 180), 200));
    defensasJugador1.back().setPropietario(1);

    defensasJugador1.push_back(Entidad(Entidad::TIPO_DEFENSA, 240, sueloY - 120,
                                       40, 120, QColor(180, 180, 180), 200));
    defensasJugador1.back().setPropietario(1);

    defensasJugador1.push_back(Entidad(Entidad::TIPO_DEFENSA, 100, sueloY - 140,
                                       180, 20, QColor(220, 220, 220), 100));
    defensasJugador1.back().setPropietario(1);

    // Defensas jugador 2
    double jugador2X = anchoEscenario - 180;
    double defensaIzq2 = jugador2X - 80;
    double defensaDer2 = jugador2X + 70;
    double anchoSuperior2 = 190;

    defensasJugador2.push_back(Entidad(Entidad::TIPO_DEFENSA, defensaIzq2, sueloY - 120,
                                       40, 120, QColor(180, 180, 180), 200));
    defensasJugador2.back().setPropietario(2);

    defensasJugador2.push_back(Entidad(Entidad::TIPO_DEFENSA, defensaDer2, sueloY - 120,
                                       40, 120, QColor(180, 180, 180), 200));
    defensasJugador2.back().setPropietario(2);

    defensasJugador2.push_back(Entidad(Entidad::TIPO_DEFENSA, defensaIzq2, sueloY - 140,
                                       anchoSuperior2, 20, QColor(220, 220, 220), 100));
    defensasJugador2.back().setPropietario(2);
}

bool Juego::prepararDisparo(double angulo, double potencia)
{
    if (estado != CONFIGURANDO_DISPARO) return false;

    jugadorActual->setAnguloCanon(angulo);

    double xInicial, yInicial, vx, vy;
    QColor color = jugadorActual->getColor();
    double velocidad = potencia * 12.0;
    double radianes = angulo * M_PI / 180.0;

    if (jugadorActual->getPropietario() == 1) {
        xInicial = jugadorActual->getX() + 40;
        yInicial = jugadorActual->getY() - 60;
        vx = velocidad * std::cos(radianes);
        vy = -velocidad * std::sin(radianes);
    } else {
        xInicial = jugadorActual->getX() - 40;
        yInicial = jugadorActual->getY() - 60;
        vx = -velocidad * std::cos(radianes);
        vy = -velocidad * std::sin(radianes);
    }

    Entidad proyectil(Entidad::TIPO_PROYECTIL, xInicial, yInicial,
                      24, 24, color, 100);
    proyectil.setPropietario(jugadorActual->getPropietario());
    proyectil.setVelocidad(vx, vy);
    proyectil.activar();

    proyectilActual = proyectil;
    estado = PROYECTIL_EN_VUELO;
    return true;
}

void Juego::actualizar(double dt)
{
    if (estado != PROYECTIL_EN_VUELO || !proyectilActual) return;

    double dtLimitado = std::min(dt, 1.0/60.0);
    aplicarGravedad(proyectilActual.value(), dtLimitado);
    proyectilActual->actualizarPosicion(dtLimitado);

    if (verificarColisionConParedes(proyectilActual.value())) {
        manejarColisionPared(proyectilActual.value());
    }

    int jugadorObjetivo = (jugadorActual->getPropietario() == 1) ? 2 : 1;
    if (verificarColisionConDefensas(proyectilActual.value(), jugadorObjetivo)) {
        // Colisión manejada dentro de la función
    }

    Entidad* jugadorObjetivoPtr = (jugadorActual->getPropietario() == 1) ? &jugador2 : &jugador1;
    if (verificarColisionConJugador(proyectilActual.value(), *jugadorObjetivoPtr)) {
        estado = JUEGO_TERMINADO;
        proyectilActual->desactivar();
        return;
    }

    double velocidad = std::sqrt(std::pow(proyectilActual->getVx(), 2) +
                                 std::pow(proyectilActual->getVy(), 2));

    if (velocidad < 6.0 ||
        proyectilActual->getY() > altoEscenario + 100 ||
        proyectilActual->getX() < -200 ||
        proyectilActual->getX() > anchoEscenario + 200) {

        estado = TURNO_COMPLETADO;
        proyectilActual->desactivar();
        QTimer::singleShot(800, this, &Juego::cambiarTurnoAutomatico);
    }
}

void Juego::cambiarTurnoAutomatico()
{
    if (estado == TURNO_COMPLETADO && !verificarFinJuego()) {
        jugadorActual = (jugadorActual == &jugador1) ? &jugador2 : &jugador1;
        turno++;
        estado = CONFIGURANDO_DISPARO;
        proyectilActual.reset();
    }
}

Entidad* Juego::getJugadorActual()
{
    return jugadorActual;
}

Entidad* Juego::getProyectilActual()
{
    return proyectilActual ? &proyectilActual.value() : nullptr;
}

Entidad* Juego::getJugador(int numero)
{
    if (numero == 1) return &jugador1;
    if (numero == 2) return &jugador2;
    return nullptr;
}

bool Juego::verificarColisionConParedes(const Entidad& entidad) const
{
    double x = entidad.getX();
    double y = entidad.getY();
    double radio = entidad.getRadio();

    return (x - radio <= 0) || (x + radio >= anchoEscenario) ||
           (y - radio <= 0) || (y + radio >= sueloY);
}

void Juego::manejarColisionPared(Entidad& entidad)
{
    double x = entidad.getX();
    double y = entidad.getY();
    double radio = entidad.getRadio();
    double vx = entidad.getVx();
    double vy = entidad.getVy();

    if (x - radio <= 0) {
        entidad.setVelocidad(-vx * coeficienteRestitucion, vy * 0.9);
        entidad.setPosicion(radio + 1, y);
    } else if (x + radio >= anchoEscenario) {
        entidad.setVelocidad(-vx * coeficienteRestitucion, vy * 0.9);
        entidad.setPosicion(anchoEscenario - radio - 1, y);
    }

    if (y - radio <= 0) {
        entidad.setVelocidad(vx * 0.9, -vy * coeficienteRestitucion);
        entidad.setPosicion(x, radio + 1);
    }

    if (y + radio >= sueloY) {
        entidad.setVelocidad(vx * 0.6, -vy * coeficienteRestitucion * 0.3);
        entidad.setPosicion(x, sueloY - radio - 5);

        if (std::fabs(vx) < 20.0) {
            entidad.setVelocidad(0, 0);
            entidad.desactivar();
        }
    }
}

bool Juego::verificarColisionConDefensas(Entidad& proyectil, int jugadorObjetivo)
{
    std::vector<Entidad>* defensas = (jugadorObjetivo == 1) ? &defensasJugador1 : &defensasJugador2;

    for (size_t i = 0; i < defensas->size(); ++i) {
        auto& defensa = (*defensas)[i];

        if (defensa.estaDestruida()) continue;

        if (proyectil.colisionaCon(defensa)) {
            double vx = proyectil.getVx();
            double vy = proyectil.getVy();
            double velocidadImpacto = std::sqrt(vx*vx + vy*vy);
            int danio = static_cast<int>(velocidadImpacto * 0.8);

            if (danio < 2) danio = 2;
            if (danio > 25) danio = 25;

            defensa.recibirDanio(danio);
            manejarColisionDefensa(proyectil, defensa);
            empujarProyectilFueraDeDefensas(proyectil, jugadorObjetivo);

            return true;
        }
    }

    return false;
}

void Juego::manejarColisionDefensa(Entidad& proyectil, Entidad& defensa)
{
    double px = proyectil.getX() + proyectil.getAncho()/2;
    double py = proyectil.getY() + proyectil.getAlto()/2;
    double radio = proyectil.getRadio();

    double left = defensa.getX();
    double right = defensa.getX() + defensa.getAncho();
    double top = defensa.getY();
    double bottom = defensa.getY() + defensa.getAlto();

    double penetracionIzquierda = (px + radio) - left;
    double penetracionDerecha = right - (px - radio);
    double penetracionSuperior = (py + radio) - top;
    double penetracionInferior = bottom - (py - radio);

    double minPenetracion = std::numeric_limits<double>::max();
    char ladoColision = 'N';

    if (penetracionIzquierda > 0 && penetracionIzquierda < minPenetracion) {
        minPenetracion = penetracionIzquierda;
        ladoColision = 'L';
    }
    if (penetracionDerecha > 0 && penetracionDerecha < minPenetracion) {
        minPenetracion = penetracionDerecha;
        ladoColision = 'R';
    }
    if (penetracionSuperior > 0 && penetracionSuperior < minPenetracion) {
        minPenetracion = penetracionSuperior;
        ladoColision = 'T';
    }
    if (penetracionInferior > 0 && penetracionInferior < minPenetracion) {
        minPenetracion = penetracionInferior;
        ladoColision = 'B';
    }

    double vx = proyectil.getVx();
    double vy = proyectil.getVy();

    switch (ladoColision) {
    case 'L':
        proyectil.setPosicion(left - radio * 2 - 2, proyectil.getY());
        proyectil.setVelocidad(-std::fabs(vx) * coeficienteRestitucion, vy * 0.85);
        break;
    case 'R':
        proyectil.setPosicion(right + 2, proyectil.getY());
        proyectil.setVelocidad(std::fabs(vx) * coeficienteRestitucion, vy * 0.85);
        break;
    case 'T':
        proyectil.setPosicion(proyectil.getX(), top - radio * 2 - 2);
        proyectil.setVelocidad(vx * 0.85, -std::fabs(vy) * coeficienteRestitucion);
        break;
    case 'B':
        proyectil.setPosicion(proyectil.getX(), bottom + 2);
        proyectil.setVelocidad(vx * 0.85, std::fabs(vy) * coeficienteRestitucion);
        break;
    }

    proyectil.setVelocidad(proyectil.getVx() * 0.75, proyectil.getVy() * 0.75);

    double velocidad = std::sqrt(std::pow(proyectil.getVx(), 2) +
                                 std::pow(proyectil.getVy(), 2));

    if (velocidad < 12.0) {
        double impulso = 15.0 / velocidad;
        proyectil.setVelocidad(proyectil.getVx() * impulso, proyectil.getVy() * impulso);
    }
}

void Juego::empujarProyectilFueraDeDefensas(Entidad& proyectil, int jugadorObjetivo)
{
    std::vector<Entidad>* defensas = (jugadorObjetivo == 1) ? &defensasJugador1 : &defensasJugador2;

    for (auto& defensa : *defensas) {
        if (defensa.estaDestruida()) continue;

        if (proyectil.colisionaCon(defensa)) {
            double px = proyectil.getX() + proyectil.getAncho()/2;
            double py = proyectil.getY() + proyectil.getAlto()/2;
            double radio = proyectil.getRadio();

            double dx = px - (defensa.getX() + defensa.getAncho()/2);
            double dy = py - (defensa.getY() + defensa.getAlto()/2);
            double distancia = std::sqrt(dx*dx + dy*dy);

            if (distancia > 0) {
                dx /= distancia;
                dy /= distancia;

                double nuevaX = defensa.getX() + defensa.getAncho()/2 + dx *
                                                                              (defensa.getAncho()/2 + radio + 5);
                double nuevaY = defensa.getY() + defensa.getAlto()/2 + dy *
                                                                             (defensa.getAlto()/2 + radio + 5);

                proyectil.setPosicion(nuevaX - proyectil.getAncho()/2,
                                      nuevaY - proyectil.getAlto()/2);
            }
        }
    }
}

bool Juego::verificarColisionConJugador(const Entidad& proyectil, Entidad& jugador)
{
    if (!jugador.estaActiva() || !proyectil.estaActiva()) return false;

    if (proyectil.colisionaCon(jugador)) {
        jugador.desactivar();
        jugador.setColor(QColor(100, 100, 100, 150));
        return true;
    }
    return false;
}

void Juego::aplicarGravedad(Entidad& entidad, double dt)
{
    if (!entidad.estaActiva()) return;

    const double g = 9.8 * 60.0;
    double nueva_vy = entidad.getVy() + g * dt;
    double nueva_vx = entidad.getVx() * 0.995;

    if (nueva_vy > 0) {
        nueva_vy = nueva_vy * 0.998;
    }

    entidad.setVelocidad(nueva_vx, nueva_vy);
}

bool Juego::verificarFinJuego() const
{
    return estado == JUEGO_TERMINADO;
}

Entidad* Juego::obtenerGanador()
{
    if (estado != JUEGO_TERMINADO) return nullptr;

    if (jugador1.estaActiva() && !jugador2.estaActiva()) {
        return &jugador1;
    } else if (!jugador1.estaActiva() && jugador2.estaActiva()) {
        return &jugador2;
    }

    return nullptr;
}

void Juego::finalizarTurno()
{
    // Método mantenido por compatibilidad
}
