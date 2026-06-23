#include "Ruleta.h"
#include "Archivos.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <cmath>
#include <windows.h>
#include "validacion.h"

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
Inicializacion de la rueda: asigna a cada posicion el numero
correspondiente de la secuencia europea y su color (Rojo, Negro o Verde)
basado en las reglas estandar de la ruleta europea
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
Inicializa el generador, la secuencia de la rueda y configura los FPS
*/
Ruleta::Ruleta(int frecuencia_actualizacion) : fps(frecuencia_actualizacion), friccion_mesa(0.0) {
    generador.seed(random_device{}());
    inicializarRueda();
    numeroGanador = 0;
    colorGanador = "Verde";
}

/*
Simula el giro completo: fase 1 (bola CW + mesa CCW giran),
fase 2 (bola cae, mesa sigue girando con la bola en la casilla),
fase 3 (mesa se detiene, se devuelve el indice ganador)
*/
int Ruleta::simularGiro() {
    uniform_real_distribution<double> distVelBola(700.0, 1400.0); // Velocidad inicial de la bola
    uniform_real_distribution<double> distFricBola(150.0, 300.0); // Friccion de la bola
    uniform_real_distribution<double> distVelMesa(200.0, 400.0); // Velocidad inicial de la mesa
    uniform_real_distribution<double> distFricMesa(10.0, 30.0); // Friccion de la mesa
    uniform_real_distribution<double> distDesfase(0.0, 360.0); // Desface aleatorio entre la mesa y la bola

    double omega_bola = distVelBola(generador);
    double theta_bola = distDesfase(generador);
    double friccion_bola = distFricBola(generador);

    double omega_mesa = -distVelMesa(generador);
    double theta_mesa = distDesfase(generador);
    friccion_mesa = distFricMesa(generador);

	//double dt = 0.0001;
    double dt = 1.0 / fps;
    int sleep_ms = (int)(1000.0 / fps);
    
    double ralentizacionFase2 = 1.02; //Desaceleracion para fase 2 aumentada

	//Fase 1
    while (omega_bola > 0.0) {
        mostrarAnimacion(theta_bola, omega_bola, theta_mesa, omega_mesa);

        omega_bola -= friccion_bola * dt;
        if (omega_bola < 0.0) omega_bola = 0.0;

        omega_mesa += friccion_mesa * dt;
        if (omega_mesa > 0.0) omega_mesa = 0.0;

        theta_bola += omega_bola * dt;
        theta_mesa += omega_mesa * dt;

        if (theta_bola >= 360.0) theta_bola -= 360.0;
        if (theta_bola < 0.0) theta_bola += 360.0;
        if (theta_mesa >= 360.0) theta_mesa -= 360.0;
        if (theta_mesa < 0.0) theta_mesa += 360.0;

        this_thread::sleep_for(chrono::milliseconds(sleep_ms));
    }

    double slot_width = 360.0 / NUM_CASILLAS;
    int indice = (int)round((theta_bola - theta_mesa) / slot_width) % NUM_CASILLAS;
    if (indice < 0) indice += NUM_CASILLAS;

    numeroGanador = numeros[indice];
    colorGanador = colores[indice];
	
	//Fase 2
    while (omega_mesa < 0.0) {
    	friccion_mesa = friccion_mesa *ralentizacionFase2; //Aumento de la friccion, desaceleracion forzada para que esta fase dure menos
        theta_bola = fmod(indice * slot_width + theta_mesa, 360.0);
        mostrarAnimacion(theta_bola, 0.0, theta_mesa, omega_mesa);

        omega_mesa += friccion_mesa * dt;
        if (omega_mesa > 0.0) omega_mesa = 0.0;
        theta_mesa += omega_mesa * dt;

        if (theta_mesa >= 360.0) theta_mesa -= 360.0;
        if (theta_mesa < 0.0) theta_mesa += 360.0;

        this_thread::sleep_for(chrono::milliseconds(sleep_ms));
    }
	
	//Fase 3
    return indice;
}

/*
Cambia el color del texto en la consola usando SetConsoleTextAttribute.
"R" = rojo intenso, "V" = verde intenso, cualquier otro = blanco/gris
*/
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

/*
Codigos de color para usar con SetConsoleTextAttribute en toda
la ruleta. Se usan tanto en la animacion del giro como en los
paneles de la mesa y el menu de apuestas.
COL_DEF = color por defecto (blanco/gris)
COL_ROJ = rojo intenso
COL_VER = verde intenso
COL_BLA = blanco (gris claro, mismo que COL_DEF)
COL_AMA = amarillo intenso
*/
static const int COL_DEF = 0;
static const int COL_ROJ = 1;
static const int COL_VER = 2;
static const int COL_BLA = 3;
static const int COL_AMA = 4;

/*
Dibuja la animacion del circulo y la bola en la consola. Usa
SetConsoleCursorPosition para sobrescribir el frame anterior.
Construye una cuadricula de caracteres con una matriz paralela
de colores, luego la renderiza en segmentos del mismo color
para minimizar llamadas a la API de Windows. Ahora incluye
la rueda giratoria (slots offset por theta_mesa), la cruz
rotativa y el panel informativo (velocidades bola/mesa, dif)
*/
void Ruleta::mostrarAnimacion(double theta_bola, double omega_bola, double theta_mesa, double omega_mesa) {
    HANDLE consola = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(consola, {0, 0});

    double anguloCasilla = 360.0 / NUM_CASILLAS;
    int indice = (int)round((theta_bola - theta_mesa) / anguloCasilla) % NUM_CASILLAS;
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

	
    vector<string> cuadricula(H, string(W, ' '));
    vector<vector<int>> colorCuadricula(H, vector<int>(W, COL_DEF));

    double anguloRad = theta_bola * PI / 180.0;
    double anguloRender = anguloRad;

    for (int row = 0; row < H; row++) {
        for (int col = 0; col < W; col++) {
            double dx = (double)(col - CX) / RX;
            double dy = (double)(row - CY) / RY;
            double dist = dx*dx + dy*dy;
            if (fabs(dist - 1.0) < 0.06) {
                cuadricula[row][col] = '*';
                double a = atan2(dy, dx) * 180.0 / PI;
                if (a < 0) a += 360.0;
                a = a - theta_mesa;
                a = fmod(a, 360.0);
                if (a < 0) a += 360.0;
                int idx = (int)round(a / anguloCasilla) % NUM_CASILLAS;
                if (idx < 0) idx += NUM_CASILLAS;
                string c = colores[idx];
                colorCuadricula[row][col] = (c == "Rojo") ? COL_ROJ : (c == "Verde" ? COL_VER : COL_BLA);
            }
        }
    }

    //Cruz rotativa (gira con la mesa, un brazo apunta a la casilla 0)
    double armAngle = theta_mesa;
    armAngle = fmod(armAngle, 360.0);
    if (armAngle < 0) armAngle += 360.0;
    double crossAngle = armAngle * PI / 180.0;
    int crossLen = 6;
    for (int arm = 0; arm < 4; arm++) {
        double angle = crossAngle + arm * PI / 2.0;
        for (int r = 1; r <= crossLen; r++) {
            int x = CX + (int)round(r * cos(angle));
            int y = CY + (int)round(r * sin(angle));
            if (x >= 0 && x < W && y >= 0 && y < H && cuadricula[y][x] != 'O') {
                cuadricula[y][x] = '+';
                colorCuadricula[y][x] = COL_AMA;
            }
        }
    }

    //Bola: fuera de la rueda mientras gira, sobre la rueda al caer
    double ballOffX = (omega_bola > 0.0) ? (RX + 3) : RX;
    double ballOffY = (omega_bola > 0.0) ? (RY + 2) : RY;
    int bolaX = CX + (int)round(ballOffX * cos(anguloRender));
    int bolaY = CY + (int)round(ballOffY * sin(anguloRender));
    if (bolaX >= 0 && bolaX < W && bolaY >= 0 && bolaY < H) {
        cuadricula[bolaY][bolaX] = 'O';
        colorCuadricula[bolaY][bolaX] = COL_AMA;
    }

    //Etiqueta del numero y color: mas afuera si la bola esta en pista exterior
    double etiqOffX = (omega_bola > 0.0) ? (RX + 5) : (RX + 3);
    double etiqOffY = (omega_bola > 0.0) ? (RY + 3) : (RY + 1);
    int etiqX = CX + (int)round(etiqOffX * cos(anguloRender));
    int etiqY = CY + (int)round(etiqOffY * sin(anguloRender));
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

    //Lambda que aplica el color en la consola segun el codigo numerico
    //de colorCuadricula, para cambiar el color del texto al escribir
    //cada segmento de caracteres del mismo color
    auto establecerColorCuadricula = [&](int c) {
        if (c == COL_ROJ) SetConsoleTextAttribute(consola, FOREGROUND_RED | FOREGROUND_INTENSITY);
        else if (c == COL_VER) SetConsoleTextAttribute(consola, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        else if (c == COL_BLA) SetConsoleTextAttribute(consola, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        else if (c == COL_AMA) SetConsoleTextAttribute(consola, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        else SetConsoleTextAttribute(consola, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    };

    //Renderiza la cuadricula fila por fila. Agrupa caracteres del mismo
    //color en un segmento de texto para minimizar llamadas a la API de
    //consola (critico para mantener fluidez)
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

    int velBola = (int)round(omega_bola);
    int velMesa = (int)round(fabs(omega_mesa));
    int diff = velBola - velMesa;

    SetConsoleCursorPosition(consola, {62, 0});
    establecerColor("default");
    cout << "| Bola: " << setw(4) << velBola << " deg/s         ";

    SetConsoleCursorPosition(consola, {62, 1});
    cout << "| Mesa: " << setw(4) << velMesa << " deg/s         ";

    SetConsoleCursorPosition(consola, {62, 2});
    cout << "| Dif:  " << setw(4) << diff << " deg/s         ";

    if (omega_bola <= 0.0) {
        SetConsoleCursorPosition(consola, {62, 4});
        establecerColor(colorActual.substr(0, 1));
        cout << "  *** CAYO: " << setw(2) << setfill('0') << numActual
             << " " << colorActual << " ***";
        cout << setfill(' ');
        establecerColor("default");
    }

    SetConsoleCursorPosition(consola, {0, H});
}

/*
Evalua si la apuesta del usuario es ganadora segun el tipo:
1=numero exacto, 2=split, 3=calle, 4=esquina, 5=rojo, 6=negro,
7=par, 8=impar, 9=bajo(1-18), 10=alto(19-36), 11-13=docenas,
14-16=columnas
*/
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

/*
Retorna el multiplicador de pago segun el tipo de apuesta en la ruleta
europea: numero=35:1, split=17:1, calle=11:1, esquina=8:1,
rojo/negro/par/impar/bajo/alto=1:1, docenas/columnas=2:1
*/
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

/*
Estructura que almacena un panel de texto con informacion de color
por caracter. Cada linea de texto (string en 'lineas') tiene un vector
paralelo de codigos de color en 'colores' para renderizar cada caracter
con el color correcto en la consola.
*/
struct PanelTexto {
    vector<string> lineas;
    vector<vector<int>> colores;
};

/*
Agrega una linea completa al panel con un color uniforme para todas
sus posiciones (COL_DEF por defecto). Util cuando la linea entera
tiene un solo color (como los bordes y separadores).
*/
static void agregarLineaAlPanel(PanelTexto& panel, const string& linea, int color = COL_DEF) {
    panel.lineas.push_back(linea);
    panel.colores.push_back(vector<int>(linea.size(), color));
}

/*
Construye el panel izquierdo con la representacion grafica de la mesa
de ruleta. Incluye el numero 0, la cuadricula de 12 filas x 3 columnas
con sus numeros y colores (rojo, negro, verde), las secciones de
docenas (1-12, 13-24, 25-36), columnas (C1, C2, C3), apuestas externas
(rojo, negro, par, impar, bajo, alto) y la leyenda de colores.
*/
static PanelTexto construirPanelTabla() {
    PanelTexto panel;

    /*
    Escribe una celda de numero en la cuadricula aplicando el color
    correspondiente segun el codigo (R=rojo intenso, V=verde intenso,
    N=color por defecto). Cada celda ocupa 8 caracteres: espacio,
    numero de 1 o 2 digitos, espacio, codigo de color y 3 espacios.
    */
    auto escribirCelda = [](string& linea, vector<int>& cols, int numero, const string& codigoColor) {
        string s = to_string(numero);
        if (numero < 10) s = " " + s;
        string texto = " " + s + " " + codigoColor + "   ";
        int codigo = (codigoColor == "R") ? COL_ROJ : (codigoColor == "V") ? COL_VER : COL_DEF;
        for (char ch : texto) {
            linea += ch;
            cols.push_back(codigo);
        }
    };

    /*
    Construye una linea completa de la cuadricula con 3 celdas numericas.
    Recibe el prefijo (borde izquierdo con tuberia), los 3 numeros y sus
    codigos de color, y agrega la linea completa al panel con los colores
    de cada celda preservados.
    */
    auto agregarLineaConCeldas = [&](const string& prefijo, int n1, const string& c1, int n2, const string& c2, int n3, const string& c3) {
        string linea = prefijo;
        vector<int> cols(linea.size(), COL_DEF);
        escribirCelda(linea, cols, n1, c1); linea += "|"; cols.push_back(COL_DEF);
        escribirCelda(linea, cols, n2, c2); linea += "|"; cols.push_back(COL_DEF);
        escribirCelda(linea, cols, n3, c3); linea += "|"; cols.push_back(COL_DEF);
        panel.lineas.push_back(linea);
        panel.colores.push_back(cols);
    };

    // Encabezado de la mesa
    agregarLineaAlPanel(panel, "");
    agregarLineaAlPanel(panel, "     +===========================+");
    agregarLineaAlPanel(panel, "     |      MESA DE RULETA       |");
    agregarLineaAlPanel(panel, "     +===========================+");
    agregarLineaAlPanel(panel, "");

    // Fila del numero 0 (verde)
    agregarLineaAlPanel(panel, "              +--------+");
    {
        string linea = "              |";
        vector<int> cols(linea.size(), COL_DEF);
        escribirCelda(linea, cols, 0, "V");
        linea += "|";
        cols.push_back(COL_DEF);
        panel.lineas.push_back(linea);
        panel.colores.push_back(cols);
    }
    agregarLineaAlPanel(panel, "     +--------+--------+--------+");

    // Cuadricula principal de 12 filas x 3 columnas (numeros 1-36)
    for (int f = 0; f < 12; f++) {
        int n1 = f * 3 + 1, n2 = f * 3 + 2, n3 = f * 3 + 3;
        string c1 = (f % 2 == 0) ? "R" : "N";
        string c2 = (f % 2 == 0) ? "N" : "R";
        string c3 = (f % 2 == 0) ? "R" : "N";
        agregarLineaConCeldas("     |", n1, c1, n2, c2, n3, c3);
        if (f < 11) agregarLineaAlPanel(panel, "     +--------+--------+--------+");
    }
    agregarLineaAlPanel(panel, "     +--------+--------+--------+");

    // Seccion de docenas (1-12, 13-24, 25-36) con pago 2:1
    agregarLineaAlPanel(panel, "     +--------+--------+--------+");
    agregarLineaAlPanel(panel, "     |    1-12   (2:1)          |");
    agregarLineaAlPanel(panel, "     +--------------------------+");
    agregarLineaAlPanel(panel, "     |   13-24   (2:1)          |");
    agregarLineaAlPanel(panel, "     +--------------------------+");
    agregarLineaAlPanel(panel, "     |   25-36   (2:1)          |");

    // Seccion de columnas (C1, C2, C3) con pago 2:1
    agregarLineaAlPanel(panel, "     +------+---------+---------+");
    agregarLineaAlPanel(panel, "     | C1   |   C2    |   C3    |");
    agregarLineaAlPanel(panel, "     | 2:1  |  2:1    |  2:1    |");
    agregarLineaAlPanel(panel, "     +------+---------+---------+");

    // Apuestas externas: rojo, negro, par, impar
    agregarLineaAlPanel(panel, "   +-------+--------+--------+-------+");
    agregarLineaAlPanel(panel, "   | Rojo  | Negro  | Par    | Impar |");
    agregarLineaAlPanel(panel, "   | (1:1) | (1:1)  | (1:1)  | (1:1) |");
    agregarLineaAlPanel(panel, "   +-------+--------+--------+-------+");

    // Apuestas externas: bajo (1-18), alto (19-36)
    agregarLineaAlPanel(panel, "   | Bajo      | Alto   |              |");
    agregarLineaAlPanel(panel, "   | (1-18)    |(19-36) |              |");
    agregarLineaAlPanel(panel, "   | (1:1)     | (1:1)  |              |");
    agregarLineaAlPanel(panel, "   +-----------+--------+--------------+");
    agregarLineaAlPanel(panel, "");

    // Leyenda: colorea la R en (R) y la V en (V)
    string lineaColor = "     (R)=Rojo  (N)=Negro  (V)=Verde";
    vector<int> coloresColor(lineaColor.size(), COL_DEF);
    size_t rPos = lineaColor.find("(R)");
    if (rPos != string::npos) coloresColor[rPos + 1] = COL_ROJ;
    size_t vPos = lineaColor.find("(V)");
    if (vPos != string::npos) coloresColor[vPos + 1] = COL_VER;
    panel.lineas.push_back(lineaColor);
    panel.colores.push_back(coloresColor);

    return panel;
}

/*
Construye el panel derecho con el menu de tipos de apuesta disponibles.
Muestra una tabla con el numero de opcion, el nombre del tipo de apuesta
y el pago correspondiente (35:1 para numero exacto, 17:1 para split,
11:1 para calle, 8:1 para esquina, 1:1 para pares, 2:1 para docenas
y columnas). Los datos se alinean en columnas de ancho fijo.
*/
static PanelTexto construirPanelOpciones() {
    PanelTexto panel;

    agregarLineaAlPanel(panel, "     SELECCIONE TIPO DE APUESTA:");
    agregarLineaAlPanel(panel, "");
    agregarLineaAlPanel(panel, "     +------+---------------+----------+");
    agregarLineaAlPanel(panel, "     |  #   | Tipo          |  Pago    |");
    agregarLineaAlPanel(panel, "     +------+---------------+----------+");

    static const char* tipos[16] = {
        "Numero", "Split", "Calle", "Esquina",
        "Rojo", "Negro", "Par", "Impar",
        "Bajo(1-18)", "Alto(19-36)", "Docena 1-12", "Docena 13-24",
        "Docena 25-36", "Columna 1", "Columna 2", "Columna 3"
    };
    static const char* pagos[16] = {
        "35:1", "17:1", "11:1", "8:1",
        "1:1", "1:1", "1:1", "1:1",
        "1:1", "1:1", "2:1", "2:1",
        "2:1", "2:1", "2:1", "2:1"
    };

    for (int i = 0; i < 16; i++) {
        int numero = i + 1;
        string numStr = to_string(numero);
        if (numero < 10) numStr = " " + numStr;

        string tipoStr = tipos[i];
        tipoStr += string(15 - tipoStr.size(), ' ');

        string pagoStr = pagos[i];
        pagoStr = string(9 - pagoStr.size(), ' ') + pagoStr;

        string linea = "     | " + numStr + "   | " + tipoStr + " | " + pagoStr + " |";
        agregarLineaAlPanel(panel, linea);
    }

    agregarLineaAlPanel(panel, "     +------+---------------+----------+");
    return panel;
}

/*
Renderiza dos paneles lado a lado en la consola. Calcula el ancho
maximo de cada panel y los centra verticalmente si tienen diferentes
alturas. Recorre las lineas de ambos paneles simultaneamente aplicando
los colores del panel izquierdo caracter por caracter mediante la
API de consola de Windows (SetConsoleTextAttribute). Entre el panel
izquierdo y el derecho se dejan 4 espacios de separacion.
*/
static void mostrarPanelesJuntos(const PanelTexto& izquierdo, const PanelTexto& derecho) {
    HANDLE consola = GetStdHandle(STD_OUTPUT_HANDLE);

    // Calcular el ancho maximo de cada panel
    size_t anchoIzq = 0;
    for (const auto& linea : izquierdo.lineas) anchoIzq = max(anchoIzq, linea.size());
    size_t anchoDer = 0;
    for (const auto& linea : derecho.lineas) anchoDer = max(anchoDer, linea.size());

    size_t maxLineas = max(izquierdo.lineas.size(), derecho.lineas.size());

    // Calcular desplazamiento vertical para centrar el panel mas corto
    size_t desplIzq = 0, desplDer = 0;
    if (izquierdo.lineas.size() < maxLineas) desplIzq = (maxLineas - izquierdo.lineas.size()) / 2;
    if (derecho.lineas.size() < maxLineas) desplDer = (maxLineas - derecho.lineas.size()) / 2;

    for (size_t i = 0; i < maxLineas; i++) {
        // Renderizar linea del panel izquierdo con colores
        size_t idxIzq = (i >= desplIzq && i - desplIzq < izquierdo.lineas.size()) ? i - desplIzq : (size_t)-1;
        if (idxIzq != (size_t)-1) {
            const string& linea = izquierdo.lineas[idxIzq];
            const vector<int>& cols = izquierdo.colores[idxIzq];
            for (size_t j = 0; j < linea.size(); j++) {
                int c = (j < cols.size()) ? cols[j] : COL_DEF;
                if (c == COL_ROJ) SetConsoleTextAttribute(consola, FOREGROUND_RED | FOREGROUND_INTENSITY);
                else if (c == COL_VER) SetConsoleTextAttribute(consola, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                else SetConsoleTextAttribute(consola, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                cout << linea[j];
            }
            for (size_t j = linea.size(); j < anchoIzq + 4; j++) cout << ' ';
        } else {
            for (size_t j = 0; j < anchoIzq + 4; j++) cout << ' ';
        }

        SetConsoleTextAttribute(consola, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

        // Renderizar linea del panel derecho (sin colores)
        size_t idxDer = (i >= desplDer && i - desplDer < derecho.lineas.size()) ? i - desplDer : (size_t)-1;
        if (idxDer != (size_t)-1) {
            cout << derecho.lineas[idxDer];
            for (size_t j = derecho.lineas[idxDer].size(); j < anchoDer; j++) cout << ' ';
        }

        cout << "\n";
    }

    SetConsoleTextAttribute(consola, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

/*
Muestra el menu de apuestas de la ruleta en la consola. Limpia la
pantalla, construye el panel de la mesa (izquierda) y el panel de
opciones de apuesta (derecha), y los renderiza lado a lado.
*/
static void mostrarMenuApuestas() {
    system("cls");

    PanelTexto tabla = construirPanelTabla();
    PanelTexto opciones = construirPanelOpciones();
    mostrarPanelesJuntos(tabla, opciones);

    cout << "\n";
}

//Funcion para los tipos de apuesta, podria ser lambda pero separado por como el metodo esta lleno ya
static string obtenerNombreApuesta(int tipo) {
    switch (tipo) {
        case 1:  return "Numero";
        case 2:  return "Split";
        case 3:  return "Calle";
        case 4:  return "Esquina";
        case 5:  return "Rojo";
        case 6:  return "Negro";
        case 7:  return "Par";
        case 8:  return "Impar";
        case 9:  return "Bajo";
        case 10: return "Alto";
        case 11: return "Doc1-12";
        case 12: return "Doc13-24";
        case 13: return "Doc25-36";
        case 14: return "Colum1";
        case 15: return "Colum2";
        case 16: return "Colum3";
        default: return "???";
    }
}

//Renderizado apuestas actuales dadas
static void mostrarApuestasActuales(const vector<Apuesta>& apuestas, double capital) {
    cout << "\n";
    cout << "     +========================================+\n";
    cout << "     |         APUESTAS ACTUALES              |\n";
    cout << "     +------+-----------+----------+----------+\n";
    cout << "     |  #   | Tipo      | Monto    | Seleccion|\n";
    cout << "     +------+-----------+----------+----------+\n";

    double total = 0.0;
    for (size_t i = 0; i < apuestas.size(); i++) {
        cout << "     | " << setw(2) << (i+1) << "  | " << setw(9) << left
             << obtenerNombreApuesta(apuestas[i].tipo) << right << " | $"
             << setw(7) << fixed << setprecision(2) << apuestas[i].monto << " | ";
        string sel;
        for (size_t j = 0; j < apuestas[i].seleccion.size(); j++) {
            if (j > 0) sel += ",";
            sel += to_string(apuestas[i].seleccion[j]);
        }
        cout << left << setw(9) << sel << right << "|\n";
        total += apuestas[i].monto;
    }

    cout << "     +------+-----------+----------+----------+\n";
    cout << "     |  Capital: $" << fixed << setprecision(2) << capital
         << "  | Apostado: $" << total
         << "  | Disp: $" << (capital - total) << " |\n";
    cout << "     +========================================+\n";
    cout << "\n";
}

/*
Bucle principal: menu de apuestas multiples, giro con fisica,
evaluacion de cada apuesta, tabla detallada de resultados y neto
*/
void Ruleta::jugar(Usuario &usuario) {
    char jugarOtra = 's';

    while (jugarOtra == 's' || jugarOtra == 'S') {
        if (usuario.getCapital() <= 0) {
            cout << "\nSaldo agotado. Debe recargar capital.\n";
            return;
        }
        
        vector<Apuesta> apuestas;
        bool salir = false;

        while (!salir) {
        	mostrarMenuApuestas();
            mostrarApuestasActuales(apuestas, usuario.getCapital());

            cout << "    (1) Agregar apuesta\n";
            cout << "    (2) Girar ruleta\n";
            cout << "    (3) Cancelar y volver\n";
            cout << "    Opcion: ";

            int opcion;
            opcion = leerEntero();

            if (opcion == 3) {
                salir = true;
                continue;
            }

            if (opcion == 2) {
                if (apuestas.empty()) {
                    cout << "    Debe agregar al menos una apuesta.\n";
                    continue;
                }
                break;
            }

            if (opcion != 1) {
                cout << "    Opcion no valida.\n";
                continue;
            }

            // --- Agregar apuesta ---
            int tipoApuesta;
            cout << "    Seleccione tipo de apuesta (1-16): ";
            tipoApuesta = leerEntero();

            if (tipoApuesta < 1 || tipoApuesta > 16) {
                cout << "    Tipo de apuesta no valido.\n";
                continue;
            }

            vector<int> seleccion;
            int num;

            if (tipoApuesta == 1) {
                cout << "    Ingrese numero (" << MIN_NUMERO << "-" << MAX_NUMERO << "): ";
                num = leerEntero();
                if (num < MIN_NUMERO || num > MAX_NUMERO) {
                    cout << "    Numero invalido.\n";
                    continue;
                }
                seleccion.push_back(num);
            } else if (tipoApuesta == 2) {
                int n1, n2;
                cout << "    Ingrese primer numero (" << MIN_NUMERO << "-" << MAX_NUMERO << "): ";
                n1 = leerEntero();
                if (n1 < MIN_NUMERO || n1 > MAX_NUMERO) {
                    cout << "    Numero invalido.\n";
                    continue;
                }
                cout << "    Ingrese segundo numero (" << MIN_NUMERO << "-" << MAX_NUMERO << "): ";
                n2 = leerEntero();
                if (n2 < MIN_NUMERO || n2 > MAX_NUMERO) {
                    cout << "    Numero invalido.\n";
                    continue;
                }
                seleccion.push_back(n1);
                seleccion.push_back(n2);
            } else if (tipoApuesta == 3) {
                cout << "    Ingrese el primer numero de la calle (1-" << PRIMERA_CALLE_MAX << "): ";
                num = leerEntero();
                if (num < 1 || num > PRIMERA_CALLE_MAX) {
                    cout << "    Invalido.\n";
                    continue;
                }
                seleccion.push_back(num);
            } else if (tipoApuesta == 4) {
                cout << "    Ingrese la esquina (numero inferior izquierdo 1-" << PRIMERA_ESQUINA_MAX << "): ";
                num = leerEntero();
                if (num < 1 || num > PRIMERA_ESQUINA_MAX) {
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
            montoApuesta = leerDouble();

            // Validar: monto > 0 y no excede el capital disponible
            double totalExistente = 0.0;
            for (const auto& a : apuestas) totalExistente += a.monto;
            double disponible = usuario.getCapital() - totalExistente;

            if (montoApuesta <= 0 || montoApuesta > disponible) {
                cout << "    Monto invalido. Disponible: $"
                     << fixed << setprecision(2) << disponible << "\n";
                continue;
            }

            apuestas.push_back({tipoApuesta, seleccion, montoApuesta});
            cout << "    Apuesta agregada. Total apostado: $"
                 << fixed << setprecision(2) << (totalExistente + montoApuesta) << "\n";
        }

        if (salir) return;

        // Descontar todas las apuestas de una vez
        double totalApostado = 0.0;
        double capitalOriginal = usuario.getCapital();
        for (const auto& a : apuestas) totalApostado += a.monto;
        usuario.setCapital(usuario.getCapital() - totalApostado);

        // Girar
        system("cls");
        simularGiro();
        
		
		//Mostrar capital original antes de calcular ganancias y perdidas
        cout << "     +============================================+\n";
        cout << "     | Capital original: $" << capitalOriginal << "                   |\n";
        cout << "     +============================================+\n";

        // Evaluar todas las apuestas
        double totalGanado = 0.0;
        struct Resultado {
            Apuesta apuesta;
            bool gano;
            double pago;
        };
        vector<Resultado> resultados;

        for (const auto& a : apuestas) {
            bool gano = evaluarApuesta(a.tipo, a.seleccion, numeroGanador, colorGanador);
            double pago = 0.0;
            if (gano) {
                pago = a.monto * obtenerPago(a.tipo);
                totalGanado += a.monto + pago;
            }
            resultados.push_back({a, gano, pago});
        }

        // Actualizar capital con ganancias
        usuario.setCapital(usuario.getCapital() + totalGanado);

        // Mostrar tabla de resultados
        cout << "\n\n";
        cout << "     +============================================+\n";
        cout << "     |               RESULTADOS                   |\n";
        cout << "     +============================================+\n";
        string codColor = colorGanador.substr(0, 1);
        cout << "     | Numero ganador: ";
        establecerColor(codColor);
        cout << setw(2) << numeroGanador << " " << colorGanador;
        establecerColor("default");
        cout << "                       |\n";
        cout << "     +------+-----------+----------+--------+----------+\n";
        cout << "     |  #   | Tipo      | Monto    | Result | Pago     |\n";
        cout << "     +------+-----------+----------+--------+----------+\n";

        for (size_t i = 0; i < resultados.size(); i++) {
            string res = resultados[i].gano ? "GANO" : "PERDIO";
            cout << "     | " << setw(2) << (i+1) << "  | " << setw(9) << left
                 << obtenerNombreApuesta(resultados[i].apuesta.tipo) << right
                 << " | $" << setw(7) << fixed << setprecision(2)
                 << resultados[i].apuesta.monto << " | " << setw(6) << res
                 << " | $" << setw(7) << resultados[i].pago << " |\n";
        }

        cout << "     +------+-----------+----------+--------+----------+\n";
        cout << "     | Total apostado: $" << fixed << setprecision(2)
             << totalApostado << "\n";
        cout << "     | Total ganado:   $" << totalGanado << "\n";
        cout << "     | Neto:           $";
        double neto = totalGanado - totalApostado;
        if (neto >= 0) cout << "+";
        cout << neto << "\n";
        cout << "     +============================================+\n";
        cout << "     | Nuevo capital: $" << usuario.getCapital() << "                   |\n";
        cout << "     +============================================+\n";
        cout << "\n";
        this_thread::sleep_for(chrono::milliseconds(3000));

        // Registrar cada resultado
        for (const auto& r : resultados) {
            if (r.gano) {
                registrarMovimiento(usuario.getNombre(), "Ruleta",r.pago, true);
            } else {
                registrarMovimiento(usuario.getNombre(), "Ruleta", r.apuesta.monto, false);
            }
        }

        cout << "\n    Desea jugar otra vez? (s/n): ";
        cin >> jugarOtra;
    }
}
