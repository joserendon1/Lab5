#include "juego.h"
#include <cmath>
#include <QDebug>

Juego::Juego(double ancho, double alto)
    : anchoEscenario(ancho), altoEscenario(alto) {

    // Inicializar jugadores
    jugador1 = Entidad(Entidad::TIPO_JUGADOR, 180, alto - 180, 30, 30, Qt::blue, 100);
    jugador1.setPropietario(1);

    jugador2 = Entidad(Entidad::TIPO_JUGADOR, ancho - 180, alto - 180, 30, 30, Qt::red, 100);
    jugador2.setPropietario(2);

    jugadorActual = &jugador1;

    inicializarEscenario();
}

void Juego::inicializarEscenario() {
    defensasJugador1.clear();
    defensasJugador2.clear();

    double sueloY = altoEscenario - 100;

    // Defensas Jugador 1
    defensasJugador1.push_back(Entidad(Entidad::TIPO_DEFENSA, 100, sueloY - 200,
                                       30, 200, QColor(100, 150, 200), 200));
    defensasJugador1.back().setPropietario(1);

    defensasJugador1.push_back(Entidad(Entidad::TIPO_DEFENSA, 230, sueloY - 200,
                                       30, 200, QColor(100, 150, 200), 200));
    defensasJugador1.back().setPropietario(1);

    defensasJugador1.push_back(Entidad(Entidad::TIPO_DEFENSA, 100, sueloY - 200,
                                       160, 30, QColor(80, 130, 180), 100));
    defensasJugador1.back().setPropietario(1);

    defensasJugador1.push_back(Entidad(Entidad::TIPO_DEFENSA, 100, sueloY - 30,
                                       160, 30, QColor(120, 170, 220), 200));
    defensasJugador1.back().setPropietario(1);

    // Defensas Jugador 2
    defensasJugador2.push_back(Entidad(Entidad::TIPO_DEFENSA, anchoEscenario - 260, sueloY - 200,
                                       30, 200, QColor(200, 100, 150), 200));
    defensasJugador2.back().setPropietario(2);

    defensasJugador2.push_back(Entidad(Entidad::TIPO_DEFENSA, anchoEscenario - 130, sueloY - 200,
                                       30, 200, QColor(200, 100, 150), 200));
    defensasJugador2.back().setPropietario(2);

    defensasJugador2.push_back(Entidad(Entidad::TIPO_DEFENSA, anchoEscenario - 260, sueloY - 200,
                                       160, 30, QColor(180, 80, 130), 100));
    defensasJugador2.back().setPropietario(2);

    defensasJugador2.push_back(Entidad(Entidad::TIPO_DEFENSA, anchoEscenario - 260, sueloY - 30,
                                       160, 30, QColor(220, 120, 170), 200));
    defensasJugador2.back().setPropietario(2);
}

bool Juego::prepararDisparo(double angulo, double potencia, double masa) {
    Q_UNUSED(masa); // Por ahora no usamos la masa, pero mantenemos el parámetro para compatibilidad

    if (estado != CONFIGURANDO_DISPARO) return false;

    jugadorActual->setAnguloCanon(angulo);

    // Calcular posición y velocidad inicial
    double xInicial, yInicial, vx, vy;
    QColor color = jugadorActual->getColor();

    if (jugadorActual->getPropietario() == 1) {
        xInicial = jugadorActual->getX() + 40;
        yInicial = jugadorActual->getY() - 60;

        double velocidad = potencia * 15.0;
        double radianes = angulo * M_PI / 180.0;
        vx = velocidad * cos(radianes);
        vy = -velocidad * sin(radianes);
    } else {
        xInicial = jugadorActual->getX() - 40;
        yInicial = jugadorActual->getY() - 60;

        double velocidad = potencia * 15.0;
        double radianes = angulo * M_PI / 180.0;
        vx = -velocidad * cos(radianes);
        vy = -velocidad * sin(radianes);
    }

    // Crear proyectil
    Entidad proyectil(Entidad::TIPO_PROYECTIL, xInicial, yInicial,
                      24, 24, color, 100);
    proyectil.setPropietario(jugadorActual->getPropietario());
    proyectil.setVelocidad(vx, vy);
    proyectil.activar();

    proyectilActual = proyectil;
    estado = PROYECTIL_EN_VUELO;

    qDebug() << "Disparo preparado. Velocidad:" << vx << vy;
    return true;
}

void Juego::actualizar(double dt) {
    if (estado != PROYECTIL_EN_VUELO || !proyectilActual) return;

    // Aplicar gravedad
    aplicarGravedad(proyectilActual.value(), dt);

    // Actualizar posición
    proyectilActual->actualizarPosicion(dt);

    // Verificar colisiones con paredes
    if (verificarColisionConParedes(proyectilActual.value())) {
        manejarColisionPared(proyectilActual.value());
    }

    // Verificar colisión con defensas del jugador contrario
    int jugadorObjetivo = (jugadorActual->getPropietario() == 1) ? 2 : 1;
    if (verificarColisionConDefensas(proyectilActual.value(), jugadorObjetivo)) {
        // La colisión ya se maneja dentro de verificarColisionConDefensas
    }

    // Verificar si el proyectil se detuvo
    double velocidad = sqrt(pow(proyectilActual->getVx(), 2) +
                            pow(proyectilActual->getVy(), 2));
    if (velocidad < 10.0 && proyectilActual->getY() > altoEscenario - 50) {
        estado = TURNO_COMPLETADO;
        proyectilActual->desactivar();
    }

    // Verificar si salió del escenario
    if (proyectilActual->getY() > altoEscenario + 100 ||
        proyectilActual->getX() < -200 ||
        proyectilActual->getX() > anchoEscenario + 200) {
        estado = TURNO_COMPLETADO;
        proyectilActual->desactivar();
    }
}

void Juego::finalizarTurno() {
    if (estado != TURNO_COMPLETADO) return;

    if (verificarFinJuego()) {
        estado = JUEGO_TERMINADO;
    } else {
        // Cambiar jugador actual
        jugadorActual = (jugadorActual == &jugador1) ? &jugador2 : &jugador1;
        turno++;
        estado = CONFIGURANDO_DISPARO;
        proyectilActual.reset();

        qDebug() << "Turno" << turno << "- Jugador" << jugadorActual->getPropietario();
    }
}

Entidad* Juego::getJugadorActual() {
    return jugadorActual;
}

Entidad* Juego::getProyectilActual() {
    return proyectilActual ? &proyectilActual.value() : nullptr;
}

bool Juego::verificarColisionConParedes(const Entidad& entidad) const {
    double x = entidad.getX();
    double y = entidad.getY();
    double radio = entidad.getRadio();

    // Pared izquierda o derecha
    if (x - radio <= 0 || x + radio >= anchoEscenario) {
        return true;
    }

    // Techo
    if (y - radio <= 0) {
        return true;
    }

    // Piso
    if (y + radio >= altoEscenario) {
        return true;
    }

    return false;
}

void Juego::manejarColisionPared(Entidad& entidad) {
    double x = entidad.getX();
    double y = entidad.getY();
    double radio = entidad.getRadio();
    double vx = entidad.getVx();
    double vy = entidad.getVy();

    // Colisión con paredes laterales
    if (x - radio <= 0) {
        entidad.setVelocidad(-vx * coeficienteRestitucion, vy * 0.9);
        entidad.setPosicion(radio + 1, y);
        qDebug() << "Colisión pared izquierda";
    }
    else if (x + radio >= anchoEscenario) {
        entidad.setVelocidad(-vx * coeficienteRestitucion, vy * 0.9);
        entidad.setPosicion(anchoEscenario - radio - 1, y);
        qDebug() << "Colisión pared derecha";
    }

    // Colisión con techo
    if (y - radio <= 0) {
        entidad.setVelocidad(vx * 0.9, -vy * coeficienteRestitucion);
        entidad.setPosicion(x, radio + 1);
        qDebug() << "Colisión techo";
    }

    // Colisión con suelo
    if (y + radio >= altoEscenario) {
        entidad.setVelocidad(vx * 0.6, -vy * coeficienteRestitucion * 0.3);
        entidad.setPosicion(x, altoEscenario - radio - 5);

        // Si la velocidad horizontal es muy baja, detener
        if (fabs(vx) < 20.0) {
            entidad.setVelocidad(0, 0);
            entidad.desactivar();
        }
        qDebug() << "Colisión suelo";
    }
}

bool Juego::verificarColisionConDefensas(Entidad& proyectil, int jugadorObjetivo) {
    std::vector<Entidad>* defensas = (jugadorObjetivo == 1) ? &defensasJugador1 : &defensasJugador2;

    for (auto& defensa : *defensas) {
        if (!defensa.estaDestruida() && proyectil.colisionaCon(defensa)) {
            // Aplicar daño
            int danio = 10; // Daño básico por ahora
            defensa.recibirDanio(danio);

            // Manejar rebote
            manejarColisionDefensa(proyectil, defensa);

            qDebug() << "Colisión con defensa! Daño:" << danio;
            return true;
        }
    }

    return false;
}

void Juego::manejarColisionDefensa(Entidad& proyectil, Entidad& defensa) {
    double px = proyectil.getX();
    double py = proyectil.getY();
    double vx = proyectil.getVx();
    double vy = proyectil.getVy();

    double dx = px - (defensa.getX() + defensa.getAncho()/2);
    double dy = py - (defensa.getY() + defensa.getAlto()/2);

    // Determinar lado de colisión
    if (fabs(dx) > fabs(dy)) {
        // Colisión horizontal
        proyectil.setVelocidad(-vx * coeficienteRestitucion * 0.7, vy * 0.9);
    } else {
        // Colisión vertical
        proyectil.setVelocidad(vx * 0.9, -vy * coeficienteRestitucion * 0.7);
    }

    // Reducir velocidad después del impacto
    proyectil.setVelocidad(proyectil.getVx() * 0.8, proyectil.getVy() * 0.8);
}

void Juego::aplicarGravedad(Entidad& entidad, double dt) {
    if (!entidad.estaActiva()) return;

    const double g = 9.8 * 60.0; // Escalado similar al original
    double nueva_vy = entidad.getVy() + g * dt;

    // Aplicar fricción del aire
    double factor_friccion = 0.995;
    double nueva_vx = entidad.getVx() * factor_friccion;

    // Fricción vertical solo cuando está cayendo
    if (nueva_vy > 0) {
        nueva_vy = nueva_vy * 0.998;
    }

    entidad.setVelocidad(nueva_vx, nueva_vy);
}

bool Juego::verificarFinJuego() const {
    // Verificar si algún jugador ha perdido todas sus defensas
    bool jugador1SinDefensas = true;
    bool jugador2SinDefensas = true;

    for (const auto& defensa : defensasJugador1) {
        if (!defensa.estaDestruida()) {
            jugador1SinDefensas = false;
            break;
        }
    }

    for (const auto& defensa : defensasJugador2) {
        if (!defensa.estaDestruida()) {
            jugador2SinDefensas = false;
            break;
        }
    }

    return jugador1SinDefensas || jugador2SinDefensas;
}

Entidad* Juego::obtenerGanador() {
    if (!verificarFinJuego()) return nullptr;

    // Verificar qué jugador aún tiene defensas
    bool jugador1TieneDefensas = false;
    bool jugador2TieneDefensas = false;

    for (const auto& defensa : defensasJugador1) {
        if (!defensa.estaDestruida()) {
            jugador1TieneDefensas = true;
            break;
        }
    }

    for (const auto& defensa : defensasJugador2) {
        if (!defensa.estaDestruida()) {
            jugador2TieneDefensas = true;
            break;
        }
    }

    if (!jugador1TieneDefensas) return &jugador2;
    if (!jugador2TieneDefensas) return &jugador1;

    return nullptr;
}
