#ifndef VALIDACION_H
#define VALIDACION_H

#include <iostream>
#include <limits>

using namespace std;

inline int leerEntero() {
    int valor;
    while (!(cin >> valor)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Entrada invalida. Ingrese un numero entero: ";
    }
    return valor;
}

inline double leerDouble() {
    double valor;
    while (!(cin >> valor)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Entrada invalida. Ingrese un numero: ";
    }
    return valor;
}

#endif
