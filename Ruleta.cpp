#include "Ruleta.h"
#include "Archivos.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <cmath>
#include <windows.h>

using namespace std;

/*
Secuencia ruleta maneja como se estructura en el sistema europero, mostrando el 0 como el punto de inicio
*/
const int SECUENCIA_RULETA[37] = {
    0, 32, 15, 19, 4, 21, 2, 25, 17, 34, 6, 27, 13, 36,
    11, 30, 8, 23, 10, 5, 24, 16, 33, 1, 20, 14, 31, 9,
    22, 18, 29, 7, 28, 12, 35, 3, 26
};

/*
Inicializacion de la rueda, manejamos entre pares e impares y los rangos
*/
void Ruleta::inicializarRueda() {
    for (int i = 0; i < NUM_CASILLAS; i++) {
        numeros[i] = SECUENCIA_RULETA[i];
        int n = SECUENCIA_RULETA[i];
        if (n == 0) {
            colores[i] = "Verde";
        } else if ((n >= 1 && n <= 10) || (n >= 19 && n <= 28)) {
            colores[i] = (n % 2 == 0) ? "Negro" : "Rojo";
        } else {
            colores[i] = (n % 2 == 0) ? "Rojo" : "Negro";
        }
    }
}

/*
Complementamos
*/
Ruleta::Ruleta() {
    generador.seed(random_device{}());
    inicializarRueda();
    numeroGanador = 0;
    colorGanador = "Verde";
}

int Ruleta::simularGiro() {
    uniform_real_distribution<double> distVelocidad(700.0, 1400.0);
    uniform_real_distribution<double> distDesfase(0.0, 360.0);
    uniform_real_distribution<double> distFriction(200.0, 400.0);

    double omega = distVelocidad(generador);
    double theta = distDesfase(generador);
    friccion = distFriction(generador);
    double dt = 1.0 / 30.0;

    system("cls");

    while (omega > 0.0) {
        mostrarAnimacion(theta, omega, dt);

        omega -= friccion * dt;
        if (omega < 0.0) omega = 0.0;
        theta += omega * dt;
        if (theta >= 360.0) theta -= 360.0;
        if (theta < 0.0) theta += 360.0;

        this_thread::sleep_for(chrono::milliseconds(33));
    }

    double anguloCasilla = 360.0 / NUM_CASILLAS;
    int indice = (int)round(theta / anguloCasilla) % NUM_CASILLAS;
    if (indice < 0) indice += NUM_CASILLAS;

    numeroGanador = numeros[indice];
    colorGanador = colores[indice];

    mostrarAterrizaje(indice, numeroGanador, colorGanador);

    return indice;
}

static void establecerColor(const string& color) {
    HANDLE consola = GetStdHandle(STD_OUTPUT_HANDLE);
    if (color == "R") {
        SetConsoleTextAttribute(consola, FOREGROUND_RED | FOREGROUND_INTENSITY);
    } else if (color == "V") {
        SetConsoleTextAttribute(consola, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    } else {
        SetConsoleTextAttribute(consola, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}

void Ruleta::mostrarAnimacion(double &theta, double &omega, double dt) {
    HANDLE consola = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(consola, {0, 0});

    double anguloCasilla = 360.0 / NUM_CASILLAS;
    int indice = (int)(theta / anguloCasilla) % NUM_CASILLAS;
    if (indice < 0) indice += NUM_CASILLAS;
    int numActual = numeros[indice];
    string colorActual = colores[indice];

    const double PI = 3.14159265358979323846;
    const int RY = 11;
    const int RX = 22;
    const int CX = RX + 4;
    const int CY = RY + 2;
    const int W = CX * 2 + 4;
    const int H = CY * 2 + 2;

    const int COL_DEF = 0, COL_ROJ = 1, COL_VER = 2, COL_BLA = 3, COL_AMA = 4;

    vector<string> cuadricula(H, string(W, ' '));
    vector<vector<int>> colorCuadricula(H, vector<int>(W, COL_DEF));

    double anguloRad = theta * PI / 180.0;
    double anguloRender = anguloRad - ROTACION * PI / 180.0;

    for (int row = 0; row < H; row++) {
        for (int col = 0; col < W; col++) {
            double dx = (double)(col - CX) / RX;
            double dy = (double)(row - CY) / RY;
            double dist = dx*dx + dy*dy;
            if (fabs(dist - 1.0) < 0.06) {
                cuadricula[row][col] = '*';
                double a = atan2(dy, dx) * 180.0 / PI;
                if (a < 0) a += 360.0;
                a = fmod(a + ROTACION, 360.0);
                int idx = (int)round(a / anguloCasilla) % NUM_CASILLAS;
                string c = colores[idx];
                colorCuadricula[row][col] = (c == "Rojo") ? COL_ROJ : (c == "Verde" ? COL_VER : COL_BLA);
            }
        }
    }

    int bolaX = CX + (int)round(RX * cos(anguloRender));
    int bolaY = CY + (int)round(RY * sin(anguloRender));
    if (bolaX >= 0 && bolaX < W && bolaY >= 0 && bolaY < H) {
        cuadricula[bolaY][bolaX] = 'O';
        colorCuadricula[bolaY][bolaX] = COL_AMA;
    }

    cuadricula[CY][CX] = '+';
    colorCuadricula[CY][CX] = COL_AMA;

    int etiqX = CX + (int)round((RX + 3) * cos(anguloRender));
    int etiqY = CY + (int)round((RY + 1) * sin(anguloRender));
    string cadenaNum = to_string(numActual);
    int colorEtiqueta = (colorActual == "Rojo") ? COL_ROJ : (colorActual == "Verde" ? COL_VER : COL_BLA);
    for (size_t i = 0; i < cadenaNum.size(); i++) {
        int px = etiqX + (int)i;
        if (px >= 0 && px < W && etiqY >= 0 && etiqY < H) {
            cuadricula[etiqY][px] = cadenaNum[i];
            colorCuadricula[etiqY][px] = colorEtiqueta;
        }
    }
    string cadenaColor = colorActual.substr(0, 1);
    int yColor = etiqY + 1;
    for (size_t i = 0; i < cadenaColor.size(); i++) {
        int px = etiqX + (int)i;
        if (px >= 0 && px < W && yColor >= 0 && yColor < H) {
            cuadricula[yColor][px] = cadenaColor[i];
            colorCuadricula[yColor][px] = colorEtiqueta;
        }
    }

    auto establecerColorCuadricula = [&](int c) {
        if (c == COL_ROJ) SetConsoleTextAttribute(consola, FOREGROUND_RED | FOREGROUND_INTENSITY);
        else if (c == COL_VER) SetConsoleTextAttribute(consola, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        else if (c == COL_BLA) SetConsoleTextAttribute(consola, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        else if (c == COL_AMA) SetConsoleTextAttribute(consola, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        else SetConsoleTextAttribute(consola, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    };

    for (int row = 0; row < H; row++) {
        cout << "     ";
        int actual = COL_DEF;
        string segmento;
        for (int col = 0; col < W; col++) {
            int c = colorCuadricula[row][col];
            if (c != actual) {
                if (!segmento.empty()) { establecerColorCuadricula(actual); cout << segmento; segmento.clear(); }
                actual = c;
            }
            segmento += cuadricula[row][col];
        }
        if (!segmento.empty()) { establecerColorCuadricula(actual); cout << segmento; }
        establecerColorCuadricula(COL_DEF);
        cout << "\n";
    }

    int ant3 = (indice - 3 + NUM_CASILLAS) % NUM_CASILLAS;
    int ant2 = (indice - 2 + NUM_CASILLAS) % NUM_CASILLAS;
    int ant1 = (indice - 1 + NUM_CASILLAS) % NUM_CASILLAS;
    int sig1 = (indice + 1) % NUM_CASILLAS;
    int sig2 = (indice + 2) % NUM_CASILLAS;

    cout << "\n     Bola: ";
    establecerColor(colorActual.substr(0, 1));
    cout << numActual << " " << colorActual;
    establecerColor("default");
    cout << "  |  Velocidad: " << fixed << setprecision(0) << omega << " deg/s\n";
    cout << "     Recorrido: " << numeros[ant3] << " -> " << numeros[ant2]
         << " -> " << numeros[ant1] << " -> [";
    establecerColor(colorActual.substr(0, 1));
    cout << numActual;
    establecerColor("default");
    cout << "] -> " << numeros[sig1] << " -> " << numeros[sig2] << "\n";
}

void Ruleta::mostrarAterrizaje(int indice, int numero, string color) {
    string codColor = color.substr(0, 1);
    cout << "\n";
    cout << "         +=========================+\n";
    cout << "         |   BOLA SE DETUVO!       |\n";
    cout << "         +=========================+\n";
    cout << "\n";
    cout << "              +-------------------+\n";
    cout << "              |  ";
    establecerColor(codColor);
    cout << setw(2) << numero << "               ";
    establecerColor("default");
    cout << "|\n";
    cout << "              |  ";
    establecerColor(codColor);
    cout << color;
    establecerColor("default");
    cout << "             |\n";
    cout << "              +-------------------+\n";
    cout << "\n";
    cout << "           +====================+\n";
    cout << "           |     GANADOR!        |\n";
    cout << "           +====================+\n";

    this_thread::sleep_for(chrono::milliseconds(2000));

    cout << "\n\n";
    cout << "         +============================+\n";
    cout << "         |      RESULTADO FINAL        |\n";
    cout << "         +============================+\n";
    cout << "\n";
    cout << "              +-------------------+\n";
    cout << "              |  ";
    establecerColor(codColor);
    cout << setw(2) << numero << "               ";
    establecerColor("default");
    cout << "|\n";
    cout << "              |  ";
    establecerColor(codColor);
    cout << color;
    establecerColor("default");
    cout << "             |\n";
    cout << "              +-------------------+\n";
    cout << "\n";
    this_thread::sleep_for(chrono::milliseconds(1000));
}

bool Ruleta::evaluarApuesta(int tipoApuesta, vector<int> seleccion, int numero, string color) {
    switch (tipoApuesta) {
        case 1:
            return (seleccion[0] == numero);
        case 2:
            return (numero == seleccion[0] || numero == seleccion[1]);
        case 3:
            return (numero >= seleccion[0] && numero <= seleccion[0] + 2);
        case 4: {
            int base = seleccion[0];
            return (numero == base || numero == base + 1 ||
                    numero == base + 3 || numero == base + 4);
        }
        case 5:
            return (color == "Rojo");
        case 6:
            return (color == "Negro");
        case 7:
            return (numero != 0 && numero % 2 == 0);
        case 8:
            return (numero != 0 && numero % 2 != 0);
        case 9:
            return (numero >= 1 && numero <= 18);
        case 10:
            return (numero >= 19 && numero <= 36);
        case 11:
            return (numero >= 1 && numero <= 12);
        case 12:
            return (numero >= 13 && numero <= 24);
        case 13:
            return (numero >= 25 && numero <= 36);
        case 14:
            return (numero > 0 && numero % 3 == 1);
        case 15:
            return (numero > 0 && numero % 3 == 2);
        case 16:
            return (numero > 0 && numero % 3 == 0);
        default:
            return false;
    }
}

static double obtenerPago(int tipoApuesta) {
    switch (tipoApuesta) {
        case 1:  return 35.0;
        case 2:  return 17.0;
        case 3:  return 11.0;
        case 4:  return 8.0;
        case 5:
        case 6:  return 1.0;
        case 7:
        case 8:  return 1.0;
        case 9:
        case 10: return 1.0;
        case 11:
        case 12:
        case 13: return 2.0;
        case 14:
        case 15:
        case 16: return 2.0;
        default: return 0.0;
    }
}

static void mostrarCelda(int n, const string& c) {
    establecerColor(c);
    string s = to_string(n);
    if (n < 10) s = " " + s;
    cout << " " << s << " " << c << "   ";
    establecerColor("default");
}

static void mostrarMenuApuestas() {
    system("cls");
    cout << "\n";
    cout << "     +===========================+\n";
    cout << "     |      MESA DE RULETA       |\n";
    cout << "     +===========================+\n\n";
    cout << "           +--------+\n";
    cout << "           |";
    mostrarCelda(0, "V");
    cout << "|\n";
    cout << "           +--------+\n";
    cout << "     +--------+--------+--------+\n";
    for (int f = 0; f < 12; f++) {
        int n1 = f * 3 + 1;
        int n2 = f * 3 + 2;
        int n3 = f * 3 + 3;
        string c1 = (f % 2 == 0) ? "R" : "N";
        string c2 = (f % 2 == 0) ? "N" : "R";
        string c3 = (f % 2 == 0) ? "R" : "N";
        cout << "     |";
        mostrarCelda(n1, c1);
        cout << "|";
        mostrarCelda(n2, c2);
        cout << "|";
        mostrarCelda(n3, c3);
        cout << "|\n";
        if (f < 11) cout << "     +--------+--------+--------+\n";
    }
    cout << "     +--------+--------+--------+\n";
    cout << "     |    1-12   (2:1)          |\n";
    cout << "     +--------------------------+\n";
    cout << "     |   13-24   (2:1)          |\n";
    cout << "     +--------------------------+\n";
    cout << "     |   25-36   (2:1)          |\n";
    cout << "     +------+---------+---------+\n";
    cout << "     | C1   |   C2    |   C3    |\n";
    cout << "     | 2:1  |  2:1    |  2:1    |\n";
    cout << "     +------+---------+---------+\n";
    cout << "   +-------+--------+--------+-------+\n";
    cout << "   | ";
    establecerColor("R"); cout << "Rojo"; establecerColor("default");
    cout << "  | ";
    cout << "Negro";
    cout << "  | Par    | Impar |\n";
    cout << "   | (1:1) | (1:1)  | (1:1)  | (1:1) |\n";
    cout << "   +-------+---+----+---+----+-------+\n";
    cout << "   | Bajo      | Alto   |              |\n";
    cout << "   | (1-18)    |(19-36) |              |\n";
    cout << "   | (1:1)     | (1:1)  |              |\n";
    cout << "   +-----------+--------+--------------+\n";
    cout << "\n";
    cout << "     (";
    establecerColor("R"); cout << "R"; establecerColor("default");
    cout << ")=Rojo  (";
    cout << "N";
    cout << ")=Negro  (";
    establecerColor("V"); cout << "V"; establecerColor("default");
    cout << ")=Verde\n";
    cout << "======================================\n";
    cout << "     SELECCIONE TIPO DE APUESTA:\n";
    cout << "\n";
    cout << "     +------+---------------+----------+\n";
    cout << "     |  #   | Tipo          |  Pago    |\n";
    cout << "     +------+---------------+----------+\n";
    cout << "     |  1   | Numero        |  35:1    |\n";
    cout << "     |  2   | Split         |  17:1    |\n";
    cout << "     |  3   | Calle         |  11:1    |\n";
    cout << "     |  4   | Esquina       |   8:1    |\n";
    cout << "     |  5   | Rojo          |   1:1    |\n";
    cout << "     |  6   | Negro         |   1:1    |\n";
    cout << "     |  7   | Par           |   1:1    |\n";
    cout << "     |  8   | Impar         |   1:1    |\n";
    cout << "     |  9   | Bajo(1-18)    |   1:1    |\n";
    cout << "     | 10   | Alto(19-36)   |   1:1    |\n";
    cout << "     | 11   | Docena 1-12   |   2:1    |\n";
    cout << "     | 12   | Docena 13-24  |   2:1    |\n";
    cout << "     | 13   | Docena 25-36  |   2:1    |\n";
    cout << "     | 14   | Columna 1     |   2:1    |\n";
    cout << "     | 15   | Columna 2     |   2:1    |\n";
    cout << "     | 16   | Columna 3     |   2:1    |\n";
    cout << "     +------+---------------+----------+\n";
    cout << "\n";
}

void Ruleta::jugar(Usuario &usuario) {
    char jugarOtra = 's';

    while (jugarOtra == 's' || jugarOtra == 'S') {
        if (usuario.getCapital() <= 0) {
            cout << "\nSaldo agotado. Debe recargar capital.\n";
            return;
        }

        mostrarMenuApuestas();

        int tipoApuesta;
        cout << "    Seleccione tipo de apuesta (1-16): ";
        cin >> tipoApuesta;

        if (tipoApuesta < 1 || tipoApuesta > 16) {
            cout << "    Opcion no valida.\n";
            continue;
        }

        vector<int> seleccion;
        int num;

        if (tipoApuesta == 1) {
            cout << "    Ingrese numero (0-36): ";
            cin >> num;
            if (num < 0 || num > 36) {
                cout << "    Numero invalido.\n";
                continue;
            }
            seleccion.push_back(num);
        } else if (tipoApuesta == 2) {
            int n1, n2;
            cout << "    Ingrese primer numero: ";
            cin >> n1;
            cout << "    Ingrese segundo numero: ";
            cin >> n2;
            seleccion.push_back(n1);
            seleccion.push_back(n2);
        } else if (tipoApuesta == 3) {
            cout << "    Ingrese el primer numero de la calle (1-34): ";
            cin >> num;
            if (num < 1 || num > 34) {
                cout << "    Invalido.\n";
                continue;
            }
            seleccion.push_back(num);
        } else if (tipoApuesta == 4) {
            cout << "    Ingrese la esquina (numero inferior izquierdo 1-32): ";
            cin >> num;
            if (num < 1 || num > 32) {
                cout << "    Invalido.\n";
                continue;
            }
            seleccion.push_back(num);
        } else {
            seleccion.push_back(0);
        }

        double montoApuesta;
        cout << "\n    Su capital actual: $" << fixed << setprecision(2)
             << usuario.getCapital() << "\n";
        cout << "    Ingrese monto a apostar: $";
        cin >> montoApuesta;

        if (!usuario.apostar(montoApuesta)) {
            cout << "    Monto invalido o excede su capital.\n";
            continue;
        }

        simularGiro();

        bool gano = evaluarApuesta(tipoApuesta, seleccion, numeroGanador, colorGanador);

        if (gano) {
            double pago = obtenerPago(tipoApuesta);
            double ganancia = montoApuesta * pago;
            double total = montoApuesta + ganancia;
            usuario.setCapital(usuario.getCapital() + total);

            cout << "\n    !!! FELICIDADES !!!\n";
            cout << "    Gano $" << fixed << setprecision(2) << ganancia << "\n";
            cout << "    Nuevo capital: $" << usuario.getCapital() << "\n";
            registrarMovimiento(usuario.getNombre(), "Ruleta", ganancia, true);
        } else {
            cout << "\n    Lo siento, perdio la apuesta.\n";
            cout << "    Nuevo capital: $" << fixed << setprecision(2)
                 << usuario.getCapital() << "\n";
            registrarMovimiento(usuario.getNombre(), "Ruleta", montoApuesta, false);
        }

        cout << "\n    Desea jugar otra vez? (s/n): ";
        cin >> jugarOtra;
    }
}
