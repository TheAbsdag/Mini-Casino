#ifndef TRAGAMONEDAS_H
#define TRAGAMONEDAS_H

#include <string>

class Tragamonedas {
private:
    std::string simbolos[3];

public:
    // Constructor
    Tragamonedas();

    // Métodos básicos
    void girar();
    double calcularPremio(double apuesta);
    void mostrarResultado();
};

#endif
