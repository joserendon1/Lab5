#include "escenajuego.h"
#include "juego.h"
#include "entidad.h"
#include <QPainter>
#include <cmath>

EscenaJuego::EscenaJuego(QWidget *parent)
    : QWidget(parent), juego(nullptr), animacionActiva(false)
{
    timerAnimacion = new QTimer(this);
    timerAnimacion->setInterval(16);
    connect(timerAnimacion, &QTimer::timeout, this, &EscenaJuego::actualizarAnimacion);

    setMinimumSize(1200, 600);
    setMaximumSize(1200, 600);
}

EscenaJuego::~EscenaJuego()
{
    if (timerAnimacion->isActive()) {
        timerAnimacion->stop();
    }
}

void EscenaJuego::setJuego(Juego* juego)
{
    this->juego = juego;
    update();
}

void EscenaJuego::iniciarAnimacion()
{
    if (!animacionActiva) {
        animacionActiva = true;
        timerAnimacion->start();
    }
}

void EscenaJuego::detenerAnimacion()
{
    animacionActiva = false;
    timerAnimacion->stop();
}

void EscenaJuego::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(240, 248, 255));

    if (!juego) return;

    double escalaX = static_cast<double>(width()) / juego->getAnchoEscenario();
    double escalaY = static_cast<double>(height()) / juego->getAltoEscenario();

    painter.save();
    painter.scale(escalaX, escalaY);

    double sueloY = juego->getSueloY();
    painter.setBrush(QBrush(QColor(139, 115, 85)));
    painter.setPen(Qt::NoPen);
    painter.drawRect(0, sueloY, juego->getAnchoEscenario(),
                     juego->getAltoEscenario() - sueloY);

    painter.setPen(QPen(Qt::darkGray, 2));
    painter.drawLine(0, sueloY, juego->getAnchoEscenario(), sueloY);

    dibujarDefensas(painter);
    dibujarJugador(painter, *juego->getJugador1());
    dibujarJugador(painter, *juego->getJugador2());
    dibujarProyectil(painter);

    painter.restore();
    dibujarInformacion(painter);
}

void EscenaJuego::dibujarJugador(QPainter &painter, const Entidad& jugador)
{
    if (!jugador.estaActiva()) return;

    painter.save();

    QColor color = (jugador.getPropietario() == 1) ?
                       QColor(150, 200, 255) : QColor(255, 150, 200);

    double radio = jugador.getRadio();
    painter.setBrush(QBrush(color));
    painter.setPen(QPen(Qt::darkGray, 2));
    painter.drawEllipse(QPointF(jugador.getX(), jugador.getY()), radio, radio);

    double angulo = jugador.getAnguloCanon();
    double radianes = angulo * M_PI / 180.0;
    double canonLength = 30.0;

    double endX, endY;
    if (jugador.getPropietario() == 1) {
        endX = jugador.getX() + canonLength * std::cos(radianes);
        endY = jugador.getY() - canonLength * std::sin(radianes);
    } else {
        endX = jugador.getX() - canonLength * std::cos(radianes);
        endY = jugador.getY() - canonLength * std::sin(radianes);
    }

    painter.setPen(QPen(Qt::darkGray, 4));
    painter.drawLine(QPointF(jugador.getX(), jugador.getY()),
                     QPointF(endX, endY));

    painter.restore();
}

void EscenaJuego::dibujarDefensas(QPainter &painter)
{
    if (!juego) return;

    QColor colorDefensa(180, 180, 180);
    QColor colorDefensaSuperior(220, 220, 220);

    const auto& defensas1 = juego->getDefensasJugador1();
    const auto& defensas2 = juego->getDefensasJugador2();

    for (const auto& defensa : defensas1) {
        if (defensa.estaDestruida()) continue;

        QRectF rect(defensa.getX(), defensa.getY(),
                    defensa.getAncho(), defensa.getAlto());

        painter.setBrush(QBrush(defensa.getAlto() < 50 ? colorDefensaSuperior : colorDefensa));
        painter.setPen(QPen(Qt::darkGray, 2));
        painter.drawRect(rect);

        painter.setPen(Qt::black);
        QFont font = painter.font();
        font.setPointSize(10);
        font.setBold(true);
        painter.setFont(font);
        painter.drawText(rect, Qt::AlignCenter, QString::number(defensa.getVida()));
    }

    for (const auto& defensa : defensas2) {
        if (defensa.estaDestruida()) continue;

        QRectF rect(defensa.getX(), defensa.getY(),
                    defensa.getAncho(), defensa.getAlto());

        painter.setBrush(QBrush(defensa.getAlto() < 50 ? colorDefensaSuperior : colorDefensa));
        painter.setPen(QPen(Qt::darkGray, 2));
        painter.drawRect(rect);

        painter.setPen(Qt::black);
        QFont font = painter.font();
        font.setPointSize(10);
        font.setBold(true);
        painter.setFont(font);
        painter.drawText(rect, Qt::AlignCenter, QString::number(defensa.getVida()));
    }
}

void EscenaJuego::dibujarProyectil(QPainter &painter)
{
    if (!juego) return;

    Entidad* proyectil = juego->getProyectilActual();
    if (!proyectil || !proyectil->estaActiva()) return;

    double x = proyectil->getX();
    double y = proyectil->getY();
    double radio = proyectil->getRadio();

    QColor color = (proyectil->getPropietario() == 1) ?
                       QColor(100, 150, 200) : QColor(200, 100, 150);

    painter.setBrush(QBrush(color));
    painter.setPen(QPen(Qt::darkGray, 1));
    painter.drawEllipse(QPointF(x, y), radio, radio);
}

void EscenaJuego::dibujarInformacion(QPainter &painter)
{
    if (!juego) return;

    painter.save();

    QString info = QString("Turno: %1 - Jugador %2 - Ángulo: %3°")
                       .arg(juego->getTurno())
                       .arg(juego->getJugadorActual()->getPropietario())
                       .arg(static_cast<int>(juego->getJugadorActual()->getAnguloCanon()));

    painter.setBrush(QBrush(QColor(255, 255, 255, 200)));
    painter.setPen(Qt::NoPen);
    painter.drawRect(5, 5, 350, 30);

    painter.setPen(Qt::black);
    QFont font = painter.font();
    font.setPointSize(12);
    font.setBold(true);
    painter.setFont(font);
    painter.drawText(10, 25, info);

    painter.restore();
}

void EscenaJuego::actualizarAnimacion()
{
    if (!juego || !animacionActiva) return;

    juego->actualizar(0.016);
    update();

    if (juego->getEstado() != Juego::PROYECTIL_EN_VUELO) {
        detenerAnimacion();
    }
}
