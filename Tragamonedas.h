#ifndef TRAGAMONEDAS_H
#define TRAGAMONEDAS_H

#include <string>

class Tragamonedas {
private:
    std::string simbolos[3];
    int tirosGratis;

public:
    // Constructor
    Tragamonedas();
    int getTirosGratis();

    // Métodos básicos
    void girar();
    double calcularPremio(double apuesta);
    void mostrarResultado();
    void jugar(Usuario &jugador);
};

#endif
