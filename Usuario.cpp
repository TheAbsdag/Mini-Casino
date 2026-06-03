#include "Usuario.h"
#include <string>

/*
* Clase basica para guardar los datos del usuario
*
*/

using namespace std;

// Constructor por defecto
Usuario::Usuario() {
    nombre = "";
    capital = 0.0;
}

// Constructor con parámetros
Usuario::Usuario(string nom, double cap) {
    nombre = nom;
    capital = cap;
}

// Getters
std::string Usuario::getNombre() {
    return nombre;
}

double Usuario::getCapital() {
    return capital;
}

// Setters
void Usuario::setNombre(std::string nom) {
    nombre = nom;
}

void Usuario::setCapital(double cap) {
    if (cap >= 0) {
        capital = cap;
    }
}

// Método para apostar - verdadero si el valor es mayor a 0 despues del monto
bool Usuario::apostar(double monto) {
    if (monto > 0 && monto <= capital) {
        capital -= monto;
        return true;
    }
    return false;
}

// Método para recargar capital
void Usuario::recargar(double monto) {
    if (monto > 0) {
        capital += monto;
    }
}
