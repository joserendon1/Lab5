#ifndef ESCENAJUEGO_H
#define ESCENAJUEGO_H

#include <QWidget>
#include <QTimer>

// Declaraciones anticipadas
class Juego;
class QPainter;
class Entidad;

class EscenaJuego : public QWidget {
    Q_OBJECT

public:
    explicit EscenaJuego(QWidget *parent = nullptr);
    ~EscenaJuego();

    void setJuego(Juego* juego);
    void iniciarAnimacion();
    void detenerAnimacion();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void actualizarAnimacion();

private:
    // Métodos de dibujo
    void dibujarFondo(QPainter& painter);
    void dibujarJugador(QPainter &painter, const Entidad& jugador);
    void dibujarProyectil(QPainter &painter);
    void dibujarDefensas(QPainter &painter);
    void dibujarBarraResistencia(QPainter &painter, const Entidad& defensa);
    void dibujarInformacion(QPainter &painter);
    void dibujarCaja(QPainter &painter);

    // Referencias
    Juego* juego;
    QTimer* timerAnimacion;
    bool animacionActiva;
};

#endif
