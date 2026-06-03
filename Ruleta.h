#ifndef RULETA_H
#define RULETA_H

#include <string>

class Ruleta {
private:
    int numeroGanador;
    std::string colorGanador;

public:
    // Constructor
    Ruleta();

    // Métodos básicos
    void girar();
    bool verificarApuesta(int opcion, int valor);
    double calcularPago(double apuesta, std::string tipoApuesta);
};

#endif
