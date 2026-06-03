#ifndef USUARIO_H
#define USUARIO_H

#include <string>

class Usuario {
private:
    std::string nombre;
    double capital;

public:
    // Constructor
    Usuario();
    Usuario(std::string nom, double cap);

    // Getters
    std::string getNombre();
    double getCapital();

    // Setters
    void setNombre(std::string nom);
    void setCapital(double cap);

    // Métodos básicos
    bool apostar(double monto);
    void recargar(double monto);
};

#endif
