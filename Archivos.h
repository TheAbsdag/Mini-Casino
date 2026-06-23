#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include <string>

// Funciones para manejo de archivos
void guardarUsuario(std::string nombre, double capital);
bool cargarUsuario(std::string nombre, double &capital); //Bool representa si el usuario existe o no
void registrarMovimiento(std::string usuario, std::string juego, double monto, bool gano);
void mostrarHistorial(std::string usuario);

class Archivos
{
};

#endif


