#include "Dados.h"
#include "Archivos.h"
#include "validacion.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

void Dados::jugar(Usuario &usuario) {
cout << "\n===================================" << endl;
cout << "         JUEGO DE DADOS" << endl;
cout << "===================================" << endl;
cout << "REGLAS:" << endl;
cout << "- Se lanzan dos dados." << endl;
cout << "- Si la suma es 7 u 11, ganas el doble." << endl;
cout << "- Si la suma es 2, 3 o 12, pierdes." << endl;
cout << "- Si sale cualquier otro numero, ganas 1:1." << endl;
cout << "===================================" << endl;
    if(usuario.getCapital() <= 0){
        cout << "No tiene capital suficiente." << endl;
        return;
    }

    double apuesta;

    cout << "\n=== JUEGO DE DADOS ===" << endl;
    cout << "Capital actual: $" << usuario.getCapital() << endl;

    cout << "Ingrese monto a apostar: ";
    apuesta = leerDouble();

    if(apuesta <= 0 || apuesta > usuario.getCapital()){
        cout << "Apuesta invalida." << endl;
        return;
    }

    int dado1 = rand() % 6 + 1;
    int dado2 = rand() % 6 + 1;

    int suma = dado1 + dado2;

    cout << "\nDado 1: " << dado1 << endl;
    cout << "Dado 2: " << dado2 << endl;
    cout << "Total: " << suma << endl;

    if(suma == 7 || suma == 11){

        double premio = apuesta * 2;

        usuario.setCapital(
            usuario.getCapital() + apuesta
        );

        cout << "¡GANASTE!" << endl;
        cout << "Premio: $" << premio << endl;

        registrarMovimiento(
            usuario.getNombre(),
            "Dados",
            premio,
            true
        );

    }
    else if(suma == 2 || suma == 3 || suma == 12){

        usuario.setCapital(
            usuario.getCapital() - apuesta
        );

        cout << "Perdiste." << endl;

        registrarMovimiento(
            usuario.getNombre(),
            "Dados",
            apuesta,
            false
        );
    }
    else{

        usuario.setCapital(
            usuario.getCapital() + apuesta
        );

        cout << "Ganaste." << endl;

        registrarMovimiento(
            usuario.getNombre(),
            "Dados",
            apuesta,
            true
        );
    }

    cout << "Capital actual: $"
         << usuario.getCapital()
         << endl;
}
