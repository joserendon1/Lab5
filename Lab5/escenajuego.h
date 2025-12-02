#ifndef ESCENAJUEGO_H
#define ESCENAJUEGO_H

#include <QWidget>
#include <QTimer>

// Incluir infraestructura.h ANTES de declarar la clase
#include "infraestructura.h"

// Declaraciones anticipadas
class Juego;
class QPainter;

class EscenaJuego : public QWidget {
    Q_OBJECT

private:
    Juego* juego;
    QTimer* timerAnimacion;
    bool animacionActiva;

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
    void dibujarDefensas(QPainter &painter);
    void dibujarBarraResistencia(QPainter &painter, const Infraestructura& defensa);
    void dibujarCaja(QPainter &painter);
    void dibujarInformacion(QPainter &painter);
    void dibujarProyectil(QPainter &painter);
};

#endif
