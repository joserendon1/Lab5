// main.cpp
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Configurar aplicación
    app.setApplicationName("Juego de Estrategia Militar");
    app.setApplicationVersion("1.0");

    // Crear y mostrar ventana principal
    MainWindow window;
    window.show();

    return app.exec();
}
