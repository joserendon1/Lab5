#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "juego.h"
#include "escenajuego.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void actualizarInterfaz();
    void agregarLog(const QString& mensaje);

private:
    Ui::MainWindow *ui;
    Juego* juego;
    EscenaJuego* escenaJuego;

    void configurarJuego();
};

#endif
