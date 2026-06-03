#include <iostream>
#include "Usuario.h"
#include "Tragamonedas.h"
#include "Ruleta.h"

using namespace std;

// Declaración de funciones
void mostrarMenuPrincipal();
void mostrarMenuJugador();
void registrarUsuario();
void iniciarSesion();

int main(int argc, char** argv) {
    int opcion;

    cout << "========================================" << endl;
    cout << "        BIENVENIDO AL MINI CASINO       " << endl;
    cout << "========================================" << endl;

    do {
        mostrarMenuPrincipal();
        cout << "Ingrese una opcion: ";
        cin >> opcion;

        switch(opcion) {
            case 1:
                registrarUsuario();
                break;
            case 2:
                iniciarSesion();
                break;
            case 3:
                cout << "Gracias por visitar el Mini Casino!" << endl;
                break;
            default:
                cout << "Opcion no valida. Intente nuevamente." << endl;
        }
    } while(opcion != 3);

    return 0;
}

void mostrarMenuPrincipal() {
    cout << "\n--- MENU PRINCIPAL ---" << endl;
    cout << "1. Registrar usuario" << endl;
    cout << "2. Iniciar sesion" << endl;
    cout << "3. Salir" << endl;
}

void mostrarMenuJugador() {
    cout << "\n--- MENU JUGADOR ---" << endl;
    cout << "1. Jugar Tragamonedas" << endl;
    cout << "2. Jugar Ruleta" << endl;
    cout << "3. Recargar capital" << endl;
    cout << "4. Ver historial" << endl;
    cout << "5. Cerrar sesion" << endl;
}

void registrarUsuario() {
    cout << "\n>> Registrar nuevo usuario" << endl;
    // TODO: Implementar registro
}

void iniciarSesion() {
    cout << "\n>> Iniciar sesion" << endl;
    // TODO: Implementar inicio de sesion
}
