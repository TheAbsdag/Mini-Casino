#ifndef RULETA_H
#define RULETA_H

#include <string>
#include <vector>
#include <random>
#include "Usuario.h"

class Ruleta {
private:
    static const int NUM_CASILLAS = 37;
    static constexpr double ROTACION = 90.0;
    int numeros[NUM_CASILLAS];              // secuencia de numeros de la rueda (sentido horario)
    std::string colores[NUM_CASILLAS];      // "Rojo", "Negro", "Verde"
    int numeroGanador;
    std::string colorGanador;
    double friccion;                        // constante de desaceleracion (grados/s^2)
    std::mt19937 generador;                 // generador de numeros aleatorios

    // Fisica y animacion
    void inicializarRueda();
    int simularGiro();                      // ejecuta fisica + animacion, devuelve el indice ganador
    void mostrarAnimacion(double &theta, double &omega, double dt);
    void mostrarAterrizaje(int indice, int numero, std::string color);

    // Evaluacion de apuestas
    bool evaluarApuesta(int tipoApuesta, std::vector<int> seleccion, int numero, std::string color);

public:
    Ruleta();
    void jugar(Usuario &usuario);
};

#endif
