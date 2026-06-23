#ifndef VALIDACION_H
#define VALIDACION_H

#include <iostream>
#include <limits>

using namespace std;

inline int leerEntero() {
    int valor;
    while (!(cin >> valor)) {
        cin.clear(); //limpieza buiffer actual
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); //Ignorar todos los caracteres hasta une nueva linea
        cout << "Entrada invalida. Ingrese un numero entero: "; //Informar usuario entrada incorrecta
    }
    return valor;
}

inline double leerDouble() {
    double valor;
    while (!(cin >> valor)) {
        cin.clear(); //limpieza buiffer actual
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); //Ignorar todos los caracteres hasta une nueva linea
        cout << "Entrada invalida. Ingrese un numero: "; //Informar usuario entrada incorrecta
    }
    return valor;
}

#endif
