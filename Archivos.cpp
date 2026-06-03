#include "Archivos.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

// Archivo para guardar usuarios (nombre,capital)
const string ARCHIVO_USUARIOS = "usuarios.txt";
// Archivo para guardar historial de movimientos
const string ARCHIVO_HISTORIAL = "historial.txt";

void guardarUsuario(string nombre, double capital) {
    //Verificamos si el usuario ya existe para actualizarlo
    ifstream archivoLectura(ARCHIVO_USUARIOS);
    string linea;
    string contenido = "";
    bool encontrado = false;

    while (getline(archivoLectura, linea)) {
        stringstream ss(linea);
        string nombreArchivo;
        double capitalArchivo;

        if (getline(ss, nombreArchivo, ',') && ss >> capitalArchivo) {
            if (nombreArchivo == nombre) {
            	string capitalStr = to_string(capital);
                // Existe, se actualiza el capital del usuario existente
                contenido += nombre + "," + capitalStr + "\n";
                encontrado = true;
            } else {
                contenido += linea + "\n";
            }
        } else {
            contenido += linea + "\n";
        }
    }
    archivoLectura.close();

    // Si no existe, se agrega
    if (!encontrado) {
        contenido += nombre + "," + to_string(capital) + "\n";
    }

    // Escritura archivo actualizado
    ofstream archivoEscritura(ARCHIVO_USUARIOS);
    archivoEscritura << contenido;
    archivoEscritura.close();
}

bool cargarUsuario(string nombre, double &capital) {
    ifstream archivo(ARCHIVO_USUARIOS);

	//El archivo esta abierto, no se puede usar
    if (!archivo.is_open()) {
        return false;
    }

    string linea;
    //Se busca el usuario en el archivo
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string nombreArchivo;
        double capitalArchivo;

        if (getline(ss, nombreArchivo, ',') && ss >> capitalArchivo) {
            if (nombreArchivo == nombre) {
            	//Se encuentra usuario, carga de capital
                capital = capitalArchivo;
                archivo.close();
                return true;
            }
        }
    }

    archivo.close();
    //No se encontró el usuario
    return false;
}

//Historial de juego
void registrarMovimiento(string usuario, string juego, double monto, bool gano) {
    ofstream archivo(ARCHIVO_HISTORIAL, ios::app);

    if (!archivo.is_open()) {
        cout << "Error al abrir el archivo de historial." << endl;
        return;
    }

    string resultado = gano ? "GANO" : "PERDIO";
    archivo << usuario << "," << juego << "," << monto << "," << resultado << endl;

    archivo.close();
}

//Revision de historial
void mostrarHistorial(string usuario) {
    ifstream archivo(ARCHIVO_HISTORIAL);

    if (!archivo.is_open()) {
        cout << "No hay historial registrado." << endl;
        return;
    }

    string linea;
    bool tieneHistorial = false;

    cout << "\n--- HISTORIAL DE " << usuario << " ---" << endl;
    cout << "Juego | Monto | Resultado" << endl;
    cout << "-------------------------" << endl;

	//Revision historial especifico de usuario
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string usuarioArchivo, juego, montoStr, resultado;

        if (getline(ss, usuarioArchivo, ',') &&
            getline(ss, juego, ',') &&
            getline(ss, montoStr, ',') &&
            getline(ss, resultado)) {

            if (usuarioArchivo == usuario) {
                cout << juego << " | " << montoStr << " | " << resultado << endl;
                tieneHistorial = true;
            }
        }
    }

    archivo.close();

    if (!tieneHistorial) {
        cout << "No hay movimientos registrados para este usuario." << endl;
    }
}

