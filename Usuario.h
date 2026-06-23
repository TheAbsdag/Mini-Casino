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
    
    //Constructor con datos
    Usuario(std::string nom, double cap); //Creacion del usuario

    // Getters
    std::string getNombre();
    double getCapital();

    // Setters 
    void setNombre(std::string nom); 
    void setCapital(double cap);

    // Métodos básicos
    bool apostar(double monto); //Booleando representa si es valida o no la apuesta
    void recargar(double monto); //Recarga al usuario
};

#endif


