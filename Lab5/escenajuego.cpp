#include "escenajuego.h"
#include "juego.h"
#include "entidad.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QDebug>
#include <cmath>

EscenaJuego::EscenaJuego(QWidget *parent)
    : QWidget(parent), juego(nullptr), animacionActiva(false) {

    timerAnimacion = new QTimer(this);
    timerAnimacion->setInterval(16); // ~60 FPS
    connect(timerAnimacion, &QTimer::timeout, this, &EscenaJuego::actualizarAnimacion);

    setMinimumSize(1200, 600);
    setMaximumSize(1200, 600);
}

EscenaJuego::~EscenaJuego() {
    if (timerAnimacion->isActive()) {
        timerAnimacion->stop();
    }
}

void EscenaJuego::setJuego(Juego* juego) {
    this->juego = juego;
    update();
}

void EscenaJuego::iniciarAnimacion() {
    if (!animacionActiva) {
        animacionActiva = true;
        timerAnimacion->start();
    }
}

void EscenaJuego::detenerAnimacion() {
    if (animacionActiva) {
        animacionActiva = false;
        timerAnimacion->stop();
    }
}

void EscenaJuego::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Fondo simple
    painter.fillRect(rect(), QColor(200, 230, 255));

    if (!juego) return;

    // Escalar coordenadas del juego a la ventana
    double escalaX = width() / juego->getAnchoEscenario();
    double escalaY = height() / juego->getAltoEscenario();

    painter.save();
    painter.scale(escalaX, escalaY);

    // Suelo
    double sueloY = juego->getAltoEscenario() - 100;
    painter.setBrush(QBrush(QColor(150, 100, 50)));
    painter.setPen(Qt::NoPen);
    painter.drawRect(0, sueloY, juego->getAnchoEscenario(), 100);

    // Línea del suelo
    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(0, sueloY, juego->getAnchoEscenario(), sueloY);

    // Dibujar defensas
    dibujarDefensas(painter);

    // Dibujar jugadores
    dibujarJugador(painter, *juego->getJugador1());
    dibujarJugador(painter, *juego->getJugador2());

    // Dibujar proyectil si existe
    dibujarProyectil(painter);

    painter.restore();

    // Información (dibujar sin escalar)
    dibujarInformacion(painter);
}

void EscenaJuego::dibujarJugador(QPainter &painter, const Entidad& jugador) {
    if (!jugador.estaActiva()) return;

    painter.save();

    // Dibujar cuerpo del jugador (círculo)
    painter.setBrush(QBrush(jugador.getColor()));
    painter.setPen(QPen(Qt::black, 2));
    double radio = jugador.getRadio();
    painter.drawEllipse(QPointF(jugador.getX(), jugador.getY()), radio, radio);

    // Dibujar cañón
    double angulo = jugador.getAnguloCanon();
    double radianes = angulo * M_PI / 180.0;
    double canonLength = 30.0;

    double endX, endY;
    if (jugador.getPropietario() == 1) {
        // Jugador 1: cañón apunta hacia la derecha
        endX = jugador.getX() + canonLength * cos(radianes);
        endY = jugador.getY() - canonLength * sin(radianes);
    } else {
        // Jugador 2: cañón apunta hacia la izquierda
        endX = jugador.getX() - canonLength * cos(radianes);
        endY = jugador.getY() - canonLength * sin(radianes);
    }

    painter.setPen(QPen(Qt::black, 4));
    painter.drawLine(QPointF(jugador.getX(), jugador.getY()),
                     QPointF(endX, endY));

    painter.restore();
}

void EscenaJuego::dibujarDefensas(QPainter &painter) {
    if (!juego) return;

    // Dibujar defensas del jugador 1
    for (const auto& defensa : juego->getDefensasJugador1()) {
        if (defensa.estaDestruida()) continue;

        QRectF rect(defensa.getX(), defensa.getY(),
                    defensa.getAncho(), defensa.getAlto());

        // Color con opacidad si tiene poca vida
        QColor color = defensa.getColor();
        double porcentaje = static_cast<double>(defensa.getVida()) / defensa.getVidaMaxima();
        if (porcentaje < 0.3) {
            color.setAlpha(150); // Semi-transparente si está dañada
        }

        painter.setBrush(QBrush(color));
        painter.setPen(QPen(Qt::black, 2));
        painter.drawRect(rect);

        // Barra de resistencia
        dibujarBarraResistencia(painter, defensa);
    }

    // Dibujar defensas del jugador 2
    for (const auto& defensa : juego->getDefensasJugador2()) {
        if (defensa.estaDestruida()) continue;

        QRectF rect(defensa.getX(), defensa.getY(),
                    defensa.getAncho(), defensa.getAlto());

        QColor color = defensa.getColor();
        double porcentaje = static_cast<double>(defensa.getVida()) / defensa.getVidaMaxima();
        if (porcentaje < 0.3) {
            color.setAlpha(150);
        }

        painter.setBrush(QBrush(color));
        painter.setPen(QPen(Qt::black, 2));
        painter.drawRect(rect);

        dibujarBarraResistencia(painter, defensa);
    }
}

void EscenaJuego::dibujarProyectil(QPainter &painter) {
    if (!juego) return;

    Entidad* proyectil = juego->getProyectilActual();
    if (!proyectil || !proyectil->estaActiva()) return;

    double x = proyectil->getX();
    double y = proyectil->getY();
    double radio = proyectil->getRadio();

    QColor color = proyectil->getColor();
    painter.setBrush(QBrush(color));
    painter.setPen(QPen(Qt::black, 1));
    painter.drawEllipse(QPointF(x, y), radio, radio);
}

void EscenaJuego::dibujarBarraResistencia(QPainter &painter, const Entidad& defensa) {
    // Barra de resistencia más pequeña
    QRectF barraRect(defensa.getX(), defensa.getY() - 12,
                     defensa.getAncho(), 8);

    // Fondo de la barra
    painter.setBrush(QBrush(Qt::gray));
    painter.setPen(QPen(Qt::black, 1));
    painter.drawRect(barraRect);

    // Relleno según resistencia
    double porcentaje = static_cast<double>(defensa.getVida()) / defensa.getVidaMaxima();
    QRectF barraRelleno(defensa.getX(), defensa.getY() - 12,
                        defensa.getAncho() * porcentaje, 8);

    QColor colorBarra;
    if (porcentaje > 0.7) colorBarra = Qt::green;
    else if (porcentaje > 0.4) colorBarra = QColor(255, 165, 0);  // Naranja
    else if (porcentaje > 0.1) colorBarra = Qt::red;
    else colorBarra = Qt::darkRed;

    painter.setBrush(QBrush(colorBarra));
    painter.setPen(Qt::NoPen);
    painter.drawRect(barraRelleno);
}

void EscenaJuego::dibujarCaja(QPainter &painter) {
    // Dibujar paredes de la caja
    painter.setPen(QPen(Qt::black, 3));
    painter.setBrush(Qt::NoBrush);

    // Pared izquierda
    painter.drawLine(0, 0, 0, height());
    // Pared derecha
    painter.drawLine(width() - 1, 0, width() - 1, height());
    // Techo
    painter.drawLine(0, 0, width(), 0);
}

void EscenaJuego::dibujarInformacion(QPainter &painter) {
    if (!juego) return;

    painter.save();

    QFont font = painter.font();
    font.setPointSize(12);
    font.setBold(true);
    painter.setFont(font);

    // Información simple en la esquina superior izquierda
    QString info = QString("Turno: %1 - Jugador %2 - Ángulo: %3°")
                       .arg(juego->getTurno())
                       .arg(juego->getJugadorActual()->getPropietario())
                       .arg(static_cast<int>(juego->getJugadorActual()->getAnguloCanon()));

    // Fondo semitransparente para el texto
    painter.setBrush(QBrush(QColor(255, 255, 255, 200)));
    painter.setPen(Qt::NoPen);
    painter.drawRect(5, 5, 350, 30);

    // Texto
    painter.setPen(Qt::black);
    painter.drawText(10, 25, info);

    // Estado del juego
    font.setPointSize(14);
    painter.setFont(font);

    QString estadoTexto;
    switch (juego->getEstado()) {
    case Juego::CONFIGURANDO_DISPARO:
        estadoTexto = "← W/D PARA ÁNGULO | ESPACIO PARA DISPARAR →";
        painter.setPen(Qt::darkBlue);
        break;
    case Juego::PROYECTIL_EN_VUELO:
        estadoTexto = "PROYECTIL EN VUELO";
        painter.setPen(Qt::green);
        break;
    case Juego::TURNO_COMPLETADO:
        estadoTexto = "← PRESIONA N PARA SIGUIENTE TURNO →";
        painter.setPen(Qt::blue);
        break;
    case Juego::JUEGO_TERMINADO:
        estadoTexto = "¡JUEGO TERMINADO! - R PARA REINICIAR";
        painter.setPen(Qt::red);
        break;
    }

    painter.drawText(width()/2 - 200, 30, estadoTexto);

    painter.restore();
}

void EscenaJuego::actualizarAnimacion() {
    if (!juego || !animacionActiva) return;

    // Actualizar juego
    juego->actualizar(0.016); // dt = 16ms

    // Actualizar visualización
    update();

    // Verificar si terminó el movimiento
    if (juego->getEstado() != Juego::PROYECTIL_EN_VUELO) {
        detenerAnimacion();
    }
}
