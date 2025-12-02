#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QTime>
#include <QKeyEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), juego(nullptr) {

    ui->setupUi(this);

    // Ajustar tamaño de ventana
    setFixedSize(1200, 700); // Ventana fija para coincidir con el juego

    // Crear juego con dimensiones iguales a la escena
    juego = new Juego(1200, 600);

    // Configurar escena
    escenaJuego = ui->widgetEscena;
    escenaJuego->setJuego(juego);
    escenaJuego->setFixedSize(1200, 600);

    // Configurar controles
    configurarJuego();

    // Actualizar interfaz inicial
    actualizarInterfaz();

    // Tomar foco para recibir eventos de teclado
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    // Mostrar instrucciones
    agregarLog("Juego inicializado. Jugador 1 comienza.");
    agregarLog("CONTROLES:");
    agregarLog("- W: Aumentar ángulo (0°-90°)");
    agregarLog("- D: Disminuir ángulo");
    agregarLog("- ESPACIO: Lanzar proyectil");
    agregarLog("- N: Siguiente turno");
    agregarLog("- R: Reiniciar juego");
    agregarLog("");
    agregarLog("FÍSICA MEJORADA:");
    agregarLog("- Gravedad realista");
    agregarLog("- Rebotes con pérdida de energía");
    agregarLog("- Daño progresivo a estructuras");
}

MainWindow::~MainWindow() {
    delete ui;
    delete juego;
}

void MainWindow::configurarJuego() {
    // Configurar física del juego
    juego->setGravedad(9.8);
    juego->setCoeficienteRestitucion(0.7);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (!juego) return;

    switch (event->key()) {
    case Qt::Key_W:
        if (juego->getEstado() == CONFIGURANDO_DISPARO) {
            // Aumentar ángulo
            double angulo = juego->getJugadorActual()->getAnguloCanon();
            angulo += 5.0;
            if (angulo > 90) angulo = 90;
            juego->getJugadorActual()->setAnguloCanon(angulo);
            agregarLog(QString("Ángulo: %1°").arg(angulo));
            actualizarInterfaz();
        }
        break;

    case Qt::Key_D:
        if (juego->getEstado() == CONFIGURANDO_DISPARO) {
            // Disminuir ángulo
            double angulo = juego->getJugadorActual()->getAnguloCanon();
            angulo -= 5.0;
            if (angulo < 0) angulo = 0;
            juego->getJugadorActual()->setAnguloCanon(angulo);
            agregarLog(QString("Ángulo: %1°").arg(angulo));
            actualizarInterfaz();
        }
        break;

    case Qt::Key_Space:
        if (juego->getEstado() == CONFIGURANDO_DISPARO) {
            // Lanzar proyectil con ángulo actual
            double angulo = juego->getJugadorActual()->getAnguloCanon();
            double potencia = 70.0; // Potencia fija más alta (70%)
            double masa = 10.0; // Masa fija

            agregarLog(QString("Disparando: Ángulo=%1°, Potencia=%2%").arg(angulo).arg(potencia));

            if (juego->prepararDisparo(angulo, potencia, masa)) {
                agregarLog("¡PROYECTIL LANZADO!");

                // Iniciar animación
                escenaJuego->iniciarAnimacion();
                actualizarInterfaz();
            }
        }
        break;

    case Qt::Key_N:
        if (juego->getEstado() == TURNO_COMPLETADO) {
            juego->finalizarTurno();

            if (juego->verificarFinJuego()) {
                Jugador* ganador = juego->obtenerGanador();
                if (ganador) {
                    QString mensaje = QString("¡JUGADOR %1 GANA!").arg(ganador->getNumero());
                    QMessageBox::information(this, "Fin del Juego", mensaje);
                    agregarLog("FIN DEL JUEGO: " + mensaje);
                }
            } else if (juego->getEstado() == CONFIGURANDO_DISPARO) {
                QString log = QString("Turno %1 - Jugador %2 listo para disparar")
                .arg(juego->getTurno())
                    .arg(juego->getJugadorActual()->getNumero());
                agregarLog(log);
            }
            actualizarInterfaz();
        }
        break;

    case Qt::Key_R:
        if (QMessageBox::question(this, "Reiniciar Juego",
                                  "¿Estás seguro de que quieres reiniciar el juego?",
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            // Reiniciar juego
            delete juego;
            juego = new Juego();
            escenaJuego->setJuego(juego);
            configurarJuego();

            agregarLog("Juego reiniciado. Jugador 1 comienza.");
            actualizarInterfaz();
        }
        break;
    }
}

void MainWindow::actualizarInterfaz() {
    if (!juego) return;

    // Actualizar título de la ventana con información del turno
    QString titulo = QString("Juego de Estrategia - Turno %1 - Jugador %2")
                         .arg(juego->getTurno())
                         .arg(juego->getJugadorActual()->getNumero());
    setWindowTitle(titulo);

    // Forzar redibujado de la escena
    escenaJuego->update();
}

void MainWindow::agregarLog(const QString& mensaje) {
    // Solo mostrar las últimas 5 líneas en el título como subtítulo
    QString tituloActual = windowTitle();
    QStringList partes = tituloActual.split(" - ");

    if (partes.size() >= 3) {
        QString nuevoSubtitulo = mensaje.left(30); // Limitar longitud
        setWindowTitle(partes[0] + " - " + partes[1] + " - " + nuevoSubtitulo);
    }
}
