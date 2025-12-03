#ifndef ENTIDAD_H
#define ENTIDAD_H

#include <QColor>
#include <QString>
#include <QRectF>

class Entidad {
public:
    enum Tipo {
        TIPO_JUGADOR,
        TIPO_PROYECTIL,
        TIPO_DEFENSA,
        TIPO_SUELO,
        TIPO_PARED
    };

    Entidad() = default;
    Entidad(Tipo tipo, double x, double y, double ancho = 0, double alto = 0,
            QColor color = Qt::gray, int vida = 100, int vidaMaxima = 100);

    // Propiedades básicas
    double getX() const { return x; }
    double getY() const { return y; }
    double getAncho() const { return ancho; }
    double getAlto() const { return alto; }
    double getVx() const { return vx; }
    double getVy() const { return vy; }
    QColor getColor() const { return color; }
    Tipo getTipo() const { return tipo; }
    int getVida() const { return vida; }
    int getVidaMaxima() const { return vidaMaxima; }
    bool estaActiva() const { return activa; }
    bool estaDestruida() const { return vida <= 0; }

    // Setters
    void setPosicion(double x, double y) { this->x = x; this->y = y; }
    void setVelocidad(double vx, double vy) { this->vx = vx; this->vy = vy; }
    void setColor(QColor color) { this->color = color; }
    void setTipo(Tipo tipo) { this->tipo = tipo; }
    void setRadio(double radio) {
        this->ancho = radio * 2;
        this->alto = radio * 2;
        this->radio = radio;
    }
    void setPropietario(int propietario) { this->propietario = propietario; }
    int getPropietario() const { return propietario; }

    // Métodos de actualización
    void actualizarPosicion(double dt);
    void recibirDanio(int cantidad);
    void desactivar() { activa = false; }
    void activar() { activa = true; }

    // Colisiones - MÉTODOS MEJORADOS
    bool colisionaCon(const Entidad& otra) const;
    bool contienePunto(double px, double py) const;

    // NUEVO: Para detección específica
    bool colisionaCirculoConRectangulo(const Entidad& rectangulo) const;
    bool colisionaRectanguloConCirculo(const Entidad& circulo) const;
    QRectF getBoundingBox() const;

    // Propiedades específicas
    double getRadio() const {
        if (radio > 0) return radio;
        return std::min(ancho, alto) / 2.0;
    }
    bool esProyectil() const { return tipo == TIPO_PROYECTIL; }
    bool esJugador() const { return tipo == TIPO_JUGADOR; }
    bool esDefensa() const { return tipo == TIPO_DEFENSA; }

    // Datos adicionales (para jugadores)
    void setAnguloCanon(double angulo) { anguloCanon = angulo; }
    double getAnguloCanon() const { return anguloCanon; }
    void setPotenciaDisparo(double potencia) { potenciaDisparo = potencia; }
    double getPotenciaDisparo() const { return potenciaDisparo; }

private:
    Tipo tipo = TIPO_DEFENSA;
    double x = 0, y = 0;
    double vx = 0, vy = 0;
    double ancho = 0, alto = 0;
    double radio = 0;  // NUEVO: radio explícito
    QColor color = Qt::gray;
    int vida = 100;
    int vidaMaxima = 100;
    bool activa = true;
    int propietario = 0; // 0 = neutral, 1 = jugador1, 2 = jugador2

    // Para jugadores
    double anguloCanon = 45.0;
    double potenciaDisparo = 50.0;
};

#endif
