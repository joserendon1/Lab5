#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "juego.h"
#include <QMessageBox>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), juego(nullptr)
{
    ui->setupUi(this);

    setFixedSize(1200, 700);
    juego = new Juego(1200, 600);

    escenaJuego = ui->widgetEscena;
    escenaJuego->setJuego(juego);
    escenaJuego->setFixedSize(1200, 600);

    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    // Mostrar instrucciones en la ventana
    QString titulo = "Juego de Estrategia - CONTROLES: W/D (ángulo) | ESPACIO (disparar) | R (reiniciar)";
    setWindowTitle(titulo);

    actualizarInterfaz();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete juego;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!juego) return;

    switch (event->key()) {
    case Qt::Key_W:
        if (juego->getEstado() == Juego::CONFIGURANDO_DISPARO) {
            double angulo = juego->getJugadorActual()->getAnguloCanon();
            angulo = std::min(angulo + 5.0, 90.0);
            juego->getJugadorActual()->setAnguloCanon(angulo);
            actualizarInterfaz();
        }
        break;

    case Qt::Key_D:
        if (juego->getEstado() == Juego::CONFIGURANDO_DISPARO) {
            double angulo = juego->getJugadorActual()->getAnguloCanon();
            angulo = std::max(angulo - 5.0, 0.0);
            juego->getJugadorActual()->setAnguloCanon(angulo);
            actualizarInterfaz();
        }
        break;

    case Qt::Key_Space:
        if (juego->getEstado() == Juego::CONFIGURANDO_DISPARO) {
            double angulo = juego->getJugadorActual()->getAnguloCanon();
            if (juego->prepararDisparo(angulo, 70.0)) {
                escenaJuego->iniciarAnimacion();
                actualizarInterfaz();
            }
        }
        break;

    case Qt::Key_R:
        if (QMessageBox::question(this, "Reiniciar Juego",
                                  "¿Estás seguro de que quieres reiniciar el juego?",
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            delete juego;
            juego = new Juego(1200, 600);
            escenaJuego->setJuego(juego);
            actualizarInterfaz();
        }
        break;
    }
}

void MainWindow::actualizarInterfaz()
{
    if (!juego) return;

    QString titulo = QString("Turno %1 - Jugador %2 - Ángulo: %3° - W/D(ángulo) | ESPACIO(disparar) | R(reiniciar)")
                         .arg(juego->getTurno())
                         .arg(juego->getJugadorActual()->getPropietario())
                         .arg(static_cast<int>(juego->getJugadorActual()->getAnguloCanon()));
    setWindowTitle(titulo);

    escenaJuego->update();
}
