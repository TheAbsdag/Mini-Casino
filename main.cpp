#include <iostream>
#include "Usuario.h"
#include "Tragamonedas.h"
#include "Ruleta.h"
#include "Archivos.h"
#include "Usuario.h"
#include "validacion.h"

using namespace std;

// Declaraci�n de funciones
void mostrarMenuPrincipal();
void mostrarMenuJugador();
void registrarUsuario();
void iniciarSesion();

Usuario usuarioActual;

int main(int argc, char** argv) {
    int opcion;

    cout << "========================================" << endl;
    cout << "        BIENVENIDO AL MINI CASINO       " << endl;
    cout << "========================================" << endl;

    do {
        mostrarMenuPrincipal();
        cout << "Ingrese una opcion: ";
        opcion = leerEntero();

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
    cout << "3. Jugar Dados"<<endl;
    cout << "4. Recargar capital" << endl;
    cout << "5. Ver historial" << endl;
    cout << "6. Cerrar sesion" << endl;
}

void registrarUsuario() {
    cout << "\n>> Registrar nuevo usuario" << endl;
    string nombre;
    double capital;

    cout << "Ingrese nombre de usuario: ";
    cin >> nombre;

    // Verificar si el usuario ya existe
    double capitalExistente;
    if (cargarUsuario(nombre, capitalExistente)) {
        cout << "El usuario ya existe. Capital actual: " << capitalExistente << endl;
        return;
    }

    cout << "Ingrese capital inicial: ";
    capital = leerDouble();

    if (capital <= 0) {
        cout << "El capital debe ser mayor a cero." << endl;
        return;
    }

    // Guardar el usuario en archivo
    guardarUsuario(nombre, capital);

    cout << "Usuario registrado exitosamente con capital: " << capital << endl;
}

void iniciarSesion() {
    cout << "\n>> Iniciar sesion" << endl;
    string nombre;
    cout << "Ingrese nombre de usuario: ";
    cin >> nombre;

    double capital;
    if (!cargarUsuario(nombre, capital)) {
        cout << "Usuario no encontrado. Debe registrarse primero." << endl;
        return;
    }

    // Crear el usuario actual
    usuarioActual = Usuario(nombre, capital);

    cout << "Bienvenido " << nombre << "! Capital actual: " << capital << endl;

    // Menu del jugador
    int opcion;
    do {
        mostrarMenuJugador();
        cout << "Ingrese una opcion: ";
        opcion = leerEntero();

        switch(opcion) {
            case 1:{
				Tragamonedas tragamonedas;
                tragamonedas.jugar(usuarioActual);
                guardarUsuario(usuarioActual.getNombre(), usuarioActual.getCapital());
                break;
			}
               
            case 2: {
                Ruleta ruleta;
                ruleta.jugar(usuarioActual);
                guardarUsuario(usuarioActual.getNombre(), usuarioActual.getCapital());
                break;
            }
            case 3:{
            	cout << "Dados - Funcionalidad proxima..." << endl;
				break;
			}
            case 4:
                // Recargar capital
                {
                    double monto;
                    cout << "Ingrese monto a recargar: ";
                    monto = leerDouble();

                    if (monto > 0) {
                        usuarioActual.recargar(monto);
                        guardarUsuario(usuarioActual.getNombre(), usuarioActual.getCapital());
                        cout << "Capital recargado. Nuevo capital: " << usuarioActual.getCapital() << endl;
                    } else {
                        cout << "El monto debe ser mayor a cero." << endl;
                    }
                }
                break;
            case 5:
                // Ver historial
                mostrarHistorial(usuarioActual.getNombre());
                break;
            case 6:
                // Guardar estado antes de cerrar sesion
                guardarUsuario(usuarioActual.getNombre(), usuarioActual.getCapital());
                cout << "Sesion cerrada. Datos guardados." << endl;
                break;
            default:
                cout << "Opcion no valida." << endl;
        }
    } while(opcion != 6);
}
