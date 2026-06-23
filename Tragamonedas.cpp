#include <iostream>
#include "Usuario.h"
#include "Tragamonedas.h"
#include "Archivos.h"
#include "validacion.h"
#include <cstdlib>
#include <ctime>
#include <string>
#include <chrono> 
#include <thread>

using namespace std;

string Opciones_Simbolos[] ={"UN","7","cereza","limon","campana","Diamante"};
int Total_simbolos= 6;

Tragamonedas::Tragamonedas(){
	srand(time(0));
	tirosGratis= 0;
}

void Tragamonedas::girar(){
	for(int animacion = 0; animacion < 15; animacion++) {
		for(int i = 0; i < 3; i++){
			int indiceAleatorio = rand() % Total_simbolos;
			simbolos[i] = Opciones_Simbolos[indiceAleatorio];
		}
		system("cls"); 
		
		cout << "\n=====================================" << endl;
		cout << "        ?? GIRO EN PROCESO ??        " << endl;
		cout << "=====================================" << endl;
		cout << "  RODILLOS: [ " << simbolos[0] << " ] [ " << simbolos[1] << " ] [ " << simbolos[2] << " ]" << endl;
		cout << "=====================================\n" << endl;
		

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	

	if(tirosGratis > 0){
		tirosGratis--;	
	}
}

double Tragamonedas::calcularPremio(double apuesta){
	if(simbolos[0]==simbolos[1] && simbolos[1] == simbolos[2]){
		if(simbolos[0]=="7"){
		return apuesta*50;
		}else if(simbolos[0]=="cereza"){
		return apuesta*4;
		}else if(simbolos[0]=="limon"){
		return apuesta*2;	
		}else if(simbolos[0]=="campana"){
		return apuesta*5;	
		}else if(simbolos[0]=="Diamante"){
		return apuesta*15;
		}else if(simbolos[0]=="UN"){
		tirosGratis= tirosGratis +3;
		cout << "\n¡¡BONO ESPECIAL CRÍTICO!! Has ganado 3 TIRADAS GRATIS por triple UN." << endl;
		return apuesta;
		}	
	}else if (simbolos[0] == simbolos[1] || simbolos[1] == simbolos[2] || simbolos[0] == simbolos[2]) {
		string simboloRepetido = "";
		if (simbolos[0] == simbolos[1] || simbolos[0] == simbolos[2]) {
			simboloRepetido = simbolos[0];
		} else {
			simboloRepetido = simbolos[1];
		}
		if (simboloRepetido == "7") {
			return apuesta * 1.5;
		} else if (simboloRepetido == "cereza") {
			return apuesta * 1.2;
		} else if (simboloRepetido == "limon") {
			return apuesta * 1.1;	
		} else if (simboloRepetido == "campana") {
			return apuesta * 1.3;	
		} else if (simboloRepetido == "Diamante") {
			return apuesta * 1.4;
		} else if (simboloRepetido == "UN") {
			tirosGratis = tirosGratis + 2;
			cout << "\n¡¡BONO ESPECIAL!! Has ganado 2 TIRADAS GRATIS por doble UN." << endl;
			return 0.0; 
		}
	} return 0.0;
	}
void Tragamonedas::mostrarResultado() {
	system("cls"); 
	cout << "\n=====================================" << endl;
	cout << "        ?? RESULTADO FINAL ??        " << endl;
	cout << "=====================================" << endl;
	cout << "  RODILLOS: [ " << simbolos[0] << " ] [ " << simbolos[1] << " ] [ " << simbolos[2] << " ]" << endl;
	cout << "=====================================\n" << endl;
}

int Tragamonedas::getTirosGratis() {
    return tirosGratis;
}

void Tragamonedas::jugar(Usuario &jugador){
	double ultimaApuestaEfectuada = 0.0;
    int opcion; 
    do {
        cout << "\n--- MENU PRINCIPAL (Saldo actual: $" << jugador.getCapital() << ") ---" << endl;
        cout << "1. Jugar a la Tragaperras" << endl;
        cout << "2. Ver mi historial de juego" << endl;
        cout << "3. Recargar saldo" << endl;
        cout << "4. Salir y guardar" << endl;
        cout << "Elige una opción: ";
        cin >> opcion;

        if (opcion == 1) {
            double apuesta;
            if (getTirosGratis() > 0) { 
        cout << "\n¡¡TIRADA GRATIS ACTIVA!! Giros restantes: " << getTirosGratis() << endl;
        
        // Usamos como base el valor de la última apuesta real que se hizo
        apuesta = ultimaApuestaEfectuada; 
        cout << "Girando los rodillos...!" << endl;
        girar();
        mostrarResultado();
        
        double premio = calcularPremio(apuesta);
        if (premio > 0) {
            jugador.recargar(premio);
            cout << "Has ganado: $" << premio << endl;
        }
        registrarMovimiento(jugador.getNombre(), "Tragaperras (Gratis)", apuesta, (premio > 0));
        guardarUsuario(jugador.getNombre(), jugador.getCapital());
    }else{
	 cout << "======================================\n";
    cout << "     SELECCIONE TIPO DE APUESTA:\n";
    cout << "\n";
    cout << "     +---------------+----------+\n";
    cout << "     | Tipo          |  Pago    |\n";
    cout << "     +---------------+----------+\n";
    cout << "     | 777           |   50x    |\n";
    cout << "     | 3x cereza     |    4x    |\n";
    cout << "     | 3x limon      |    2x    |\n";
    cout << "     | 3x campana    |    5x    |\n";
    cout << "     | 3x Diamante   |   15x    |\n";
    cout << "     | 3x UN         | 3 tiradas|\n";
    cout << "     |               |  gratis  |\n";
    cout << "     | 2x 7          |   1.5x   |\n";
    cout << "     | 2x cereza     |   1.2x   |\n";
    cout << "     | 2x limon      |   1.1x   |\n";
    cout << "     | 2x campana    |   1.3x   |\n";
    cout << "     | 2x Diamante   |   1.4x   |\n";
    cout << "     | 2x UN         | 2 tirada |\n";
    cout << "     |               |  gratis  |\n";
    cout << "     +---------------+----------+\n";
    cout << "\n";
            cout << "¿Cuánto deseas apostar?: $";
            cin >> apuesta;

            // Intentamos ejecutar la apuesta reduciendo el saldo del jugador
            if (jugador.apostar(apuesta)) {
            	ultimaApuestaEfectuada = apuesta;
                cout << "\n¡Girando los rodillos...!" << endl;
                girar();
                mostrarResultado();

                double premio = calcularPremio(apuesta);
                bool gano = (premio > 0);

                if (gano) {
                    jugador.recargar(premio);
                    cout << "Has ganado: $" << premio << endl;
                }

                // Registramos la jugada en el historial.txt usando tus funciones de Archivos
                registrarMovimiento(jugador.getNombre(), "Tragaperras", apuesta, gano);
                // Sincronizamos el saldo actual en usuarios.txt inmediatamente
                guardarUsuario(jugador.getNombre(), jugador.getCapital());
            } else {
                cout << "Error: No tienes saldo suficiente o la apuesta es inválida." << endl;
            }
        } }
        else if (opcion == 2) {
            mostrarHistorial(jugador.getNombre());
        } 
        else if (opcion == 3) {
            double montoRecarga;
            cout << "Introduce el monto a recargar: $";
            cin >> montoRecarga;
            jugador.recargar(montoRecarga);
            guardarUsuario(jugador.getNombre(), jugador.getCapital());
            cout << "Recarga exitosa. Nuevo saldo: $" << jugador.getCapital() << endl;
        }}
     while (opcion != 4);

    cout << "Gracias por jugar. Tu saldo final ha sido guardado de forma segura." << endl;
}

