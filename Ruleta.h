#ifndef RULETA_H
#define RULETA_H

#include <string>
#include <vector>
#include <random>
#include "Usuario.h"

struct Apuesta {
    int tipo;
    std::vector<int> seleccion;
    double monto;
};

class Ruleta {
private:
    static const int NUM_CASILLAS = 37;
    static const int MIN_NUMERO = 0;
    //En caso de que se haga una ruleta personalizada
    static const int MAX_NUMERO = NUM_CASILLAS-1;
    static const int PRIMERA_CALLE_MAX = MAX_NUMERO - 2;  // 34 (cubre 34,35,36)
    static const int PRIMERA_ESQUINA_MAX = MAX_NUMERO - 4; // 32 (cubre 32,33,35,36)
    int numeros[NUM_CASILLAS];                  // secuencia de numeros de la rueda (sentido horario)
    std::string colores[NUM_CASILLAS];          // "Rojo", "Negro", "Verde"
    int numeroGanador;
    std::string colorGanador;
    std::mt19937 generador;                     // generador de numeros aleatorios

    double friccion_mesa;                       // friccion de la mesa (grados/s^2), persistente entre fases
    int fps;

    // Fisica y animacion
    void inicializarRueda();					// Inicializaci�n de los valores de rueda, este caso tipo europea
    int simularGiro();                          // ejecuta fisica + animacion, devuelve el indice ganador

    // Renderizado de la animacion (recibe estado de bola + mesa)
    void mostrarAnimacion(double theta_bola, double omega_bola, double theta_mesa, double omega_mesa);

    // Evaluacion de apuestas
    bool evaluarApuesta(int tipoApuesta, std::vector<int> seleccion, int numero, std::string color);

public:
    Ruleta(int frecuencia_actualizacion = 60); //Constructor manejado, con los frames deseados en la construccion del mismo
    void jugar(Usuario &usuario);
};

#endif
