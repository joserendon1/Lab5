#include "juego.h"
#include <cmath>
#include <QDebug>

Juego::Juego(double ancho, double alto)
    : anchoEscenario(ancho), altoEscenario(alto),
    jugador1(1, 180, alto - 180, Qt::blue),
    jugador2(2, ancho - 180, alto - 180, Qt::red),
    turno(1), estado(CONFIGURANDO_DISPARO),
    gravedad(9.8), coeficienteRestitucion(0.7), resistenciaParedes(0.8) {

    jugadorActual = &jugador1;
    inicializarEscenario();
}

void Juego::inicializarEscenario() {
    // Limpiar defensas existentes
    jugador1.getDefensas().clear();
    jugador2.getDefensas().clear();

    // ALTURA del suelo
    double sueloY = altoEscenario - 100;

    // AGREGAR DEFENSAS AL JUGADOR 1 (IZQUIERDA)
    // Estructura más grande y mejor posicionada

    // Muro izquierdo (vertical) - 200 vida
    jugador1.agregarDefensa(Infraestructura(100, sueloY - 200, 30, 200, 200, QColor(100, 150, 200), "Pared Izq"));

    // Muro derecho (vertical) - 200 vida
    jugador1.agregarDefensa(Infraestructura(230, sueloY - 200, 30, 200, 200, QColor(100, 150, 200), "Pared Der"));

    // Techo (horizontal) - 100 vida
    jugador1.agregarDefensa(Infraestructura(100, sueloY - 200, 160, 30, 100, QColor(80, 130, 180), "Techo"));

    // Piso interno (horizontal) - 200 vida
    jugador1.agregarDefensa(Infraestructura(100, sueloY - 30, 160, 30, 200, QColor(120, 170, 220), "Piso"));

    // AGREGAR DEFENSAS AL JUGADOR 2 (DERECHA)
    // Mismo diseño pero en el lado derecho

    // Muro izquierdo (vertical) - 200 vida
    jugador2.agregarDefensa(Infraestructura(anchoEscenario - 260, sueloY - 200, 30, 200, 200, QColor(200, 100, 150), "Pared Izq"));

    // Muro derecho (vertical) - 200 vida
    jugador2.agregarDefensa(Infraestructura(anchoEscenario - 130, sueloY - 200, 30, 200, 200, QColor(200, 100, 150), "Pared Der"));

    // Techo (horizontal) - 100 vida
    jugador2.agregarDefensa(Infraestructura(anchoEscenario - 260, sueloY - 200, 160, 30, 100, QColor(180, 80, 130), "Techo"));

    // Piso interno (horizontal) - 200 vida
    jugador2.agregarDefensa(Infraestructura(anchoEscenario - 260, sueloY - 30, 160, 30, 200, QColor(220, 120, 170), "Piso"));
}

bool Juego::prepararDisparo(double angulo, double potencia, double masaProyectil) {
    if (estado != CONFIGURANDO_DISPARO) return false;

    // Configurar ángulo y potencia del jugador actual
    jugadorActual->setAnguloCanon(angulo);
    jugadorActual->setPotenciaDisparo(potencia);

    // Calcular velocidad inicial - FACTORES MÁS REALISTAS
    double velocidad = potencia * 15.0;  // Factor mucho mayor para alcanzar distancia
    double radianes = angulo * M_PI / 180.0;

    double vx, vy;
    double xInicial, yInicial;

    if (jugadorActual->getNumero() == 1) {
        // Jugador 1 dispara desde la DERECHA de su estructura
        xInicial = jugadorActual->getX() + 40; // Más lejos del centro
        yInicial = jugadorActual->getY() - 60; // Más arriba

        // Velocidad hacia la derecha
        vx = velocidad * cos(radianes);
        vy = -velocidad * sin(radianes);  // Negativo para ir hacia arriba inicialmente
    } else {
        // Jugador 2 dispara desde la IZQUIERDA de su estructura
        xInicial = jugadorActual->getX() - 40; // Más lejos del centro
        yInicial = jugadorActual->getY() - 60; // Más arriba

        // Velocidad hacia la izquierda
        vx = -velocidad * cos(radianes);
        vy = -velocidad * sin(radianes);
    }

    // Usar masa fija de 10.0
    double masaFija = 10.0;

    proyectilActual = std::make_unique<Proyectil>(
        xInicial, yInicial, vx, vy, masaFija, 12.0, 10, jugadorActual->getColor()
        );
    proyectilActual->setEnMovimiento(true);

    qDebug() << "=== DISPARO ===";
    qDebug() << "Posición inicial:" << xInicial << yInicial;
    qDebug() << "Velocidad inicial:" << vx << "px/s, " << vy << "px/s";
    qDebug() << "Ángulo:" << angulo << "°, Potencia:" << potencia;
    qDebug() << "Masa:" << masaFija;

    estado = PROYECTIL_EN_VUELO;
    return true;
}

void Juego::actualizar(double dt) {
    if (estado != PROYECTIL_EN_VUELO || !proyectilActual) return;

    // Guardar posición anterior
    double x_ant = proyectilActual->getX();
    double y_ant = proyectilActual->getY();

    // Actualizar posición del proyectil (con gravedad)
    proyectilActual->actualizarPosicion(dt);

    // Calcular velocidad actual para logging
    double vx = proyectilActual->getVx();
    double vy = proyectilActual->getVy();
    double velocidad = sqrt(vx*vx + vy*vy);

    // Verificar colisión con paredes del escenario
    if (verificarColisionConParedes(*proyectilActual)) {
        manejarColisionPared(*proyectilActual);
    }

    // Verificar colisión con jugador contrario
    Jugador* jugadorObjetivo = (jugadorActual->getNumero() == 1) ? &jugador2 : &jugador1;

    if (verificarColisionProyectilJugador(*proyectilActual, *jugadorObjetivo)) {
        qDebug() << "¡IMPACTO DIRECTO AL JUGADOR" << jugadorObjetivo->getNumero() << "!";
        int danio = proyectilActual->calcularDanio() * 3;
        jugadorObjetivo->recibirDanio(danio);
        proyectilActual->setImpacto(true);
        proyectilActual->setEnMovimiento(false);
        estado = TURNO_COMPLETADO;
        return;
    }

    // Verificar colisión con defensas
    if (verificarColisionProyectilDefensas(*proyectilActual, *jugadorObjetivo)) {
        qDebug() << "Colisión con defensa a velocidad:" << velocidad;
    }

    // Verificar si el proyectil se detuvo
    if (velocidad < 10.0) { // Umbral más bajo para detenerse
        // Si está en el suelo y muy lento, detener
        if (proyectilActual->getY() > altoEscenario - 50) {
            qDebug() << "Proyectil detenido en el suelo. Velocidad:" << velocidad;
            estado = TURNO_COMPLETADO;
            proyectilActual->setEnMovimiento(false);
        }
    }

    // Verificar si salió del escenario
    if (proyectilActual->getY() > altoEscenario + 100 ||
        proyectilActual->getX() < -200 ||
        proyectilActual->getX() > anchoEscenario + 200) {

        qDebug() << "Proyectil salió del escenario";
        estado = TURNO_COMPLETADO;
        proyectilActual->setEnMovimiento(false);
    }
}

void Juego::finalizarTurno() {
    if (estado != TURNO_COMPLETADO) return;

    // Cambiar jugador actual
    if (verificarFinJuego()) {
        estado = JUEGO_TERMINADO;
    } else {
        jugadorActual = (jugadorActual->getNumero() == 1) ? &jugador2 : &jugador1;
        turno++;
        estado = CONFIGURANDO_DISPARO;
        proyectilActual.reset();

        qDebug() << "Turno" << turno << "- Jugador" << jugadorActual->getNumero();
    }
}

bool Juego::verificarColisionProyectilJugador(const Proyectil& p, Jugador& jugador) {
    double dx = p.getX() - jugador.getX();
    double dy = p.getY() - jugador.getY();
    double distancia = sqrt(dx*dx + dy*dy);

    // Radio del jugador (15) + radio del proyectil (8)
    return distancia <= (15 + 8);
}

bool Juego::verificarColisionProyectilDefensas(Proyectil& p, Jugador& jugador) {
    bool colision = false;

    // Usar referencia no-const para poder modificar las defensas
    auto& defensas = jugador.getDefensas();

    for (auto& defensa : defensas) {
        if (!defensa.estaDestruida() &&
            defensa.colisionaConProyectil(p.getX(), p.getY(), p.getRadio())) {

            // Aplicar daño (cantidad moderada)
            int danio = p.calcularDanio();
            defensa.recibirDanio(danio);

            qDebug() << "Defensa dañada! Daño:" << danio
                     << "Resistencia restante:" << defensa.getResistenciaActual();

            // Manejar rebote físico
            manejarColisionDefensa(p, defensa);

            colision = true;
            break; // Solo una colisión por frame
        }
    }

    return colision;
}

bool Juego::verificarColisionConParedes(const Proyectil& p) const {
    double x = p.getX();
    double y = p.getY();
    double radio = p.getRadio();

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

void Juego::manejarColisionDefensa(Proyectil& p, Infraestructura& defensa) {
    // Calcular punto de colisión
    double px = p.getX();
    double py = p.getY();
    double radio = p.getRadio();

    double dx = px - (defensa.getX() + defensa.getAncho()/2);
    double dy = py - (defensa.getY() + defensa.getAlto()/2);

    // Determinar lado de colisión
    double vx = p.getVx();
    double vy = p.getVy();

    if (fabs(dx) > fabs(dy)) {
        // Colisión horizontal
        p.setVelocidad(-vx * coeficienteRestitucion * 0.7, vy * 0.9);
    } else {
        // Colisión vertical
        p.setVelocidad(vx * 0.9, -vy * coeficienteRestitucion * 0.7);
    }

    // Reducir velocidad después del impacto
    p.setVelocidad(p.getVx() * 0.8, p.getVy() * 0.8);
}

void Juego::manejarColisionPared(Proyectil& p) {
    double x = p.getX();
    double y = p.getY();
    double radio = p.getRadio();
    double vx = p.getVx();
    double vy = p.getVy();

    // Colisión con paredes laterales
    if (x - radio <= 0) {
        p.setVelocidad(-vx * coeficienteRestitucion, vy * 0.9);
        p.setPosicion(radio + 1, y);
        qDebug() << "Colisión pared izquierda";
    }
    else if (x + radio >= anchoEscenario) {
        p.setVelocidad(-vx * coeficienteRestitucion, vy * 0.9);
        p.setPosicion(anchoEscenario - radio - 1, y);
        qDebug() << "Colisión pared derecha";
    }

    // Colisión con techo
    if (y - radio <= 0) {
        p.setVelocidad(vx * 0.9, -vy * coeficienteRestitucion);
        p.setPosicion(x, radio + 1);
        qDebug() << "Colisión techo";
    }

    // Colisión con suelo
    if (y + radio >= altoEscenario) {
        p.setVelocidad(vx * 0.6, -vy * coeficienteRestitucion * 0.3);
        p.setPosicion(x, altoEscenario - radio - 5);

        // Si la velocidad horizontal es muy baja, detener
        if (fabs(vx) < 20.0) {
            p.setVelocidad(0, 0);
            p.setEnMovimiento(false);
        }
        qDebug() << "Colisión suelo";
    }
}

bool Juego::verificarFinJuego() const {
    // Verificar si algún jugador ha perdido todas sus defensas
    return jugador1.todasDefensasDestruidas() || jugador2.todasDefensasDestruidas();
}

Jugador* Juego::obtenerGanador() const {
    if (!verificarFinJuego()) return nullptr;

    if (jugador1.todasDefensasDestruidas()) return const_cast<Jugador*>(&jugador2);
    if (jugador2.todasDefensasDestruidas()) return const_cast<Jugador*>(&jugador1);

    return nullptr;
}
