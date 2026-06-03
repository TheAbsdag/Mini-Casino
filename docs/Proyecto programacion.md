Universidad Nacional de Colombia

Ingeniería de sistemas

Semestre 2026-1

Programación básica

Propuesta proyecto final programación

Nombre: Mini casino

Integrantes

* Santiago Esteban Castelblanco Castiblanco: [scastelblancoc@unal.edu.co](mailto:scastelblancoc@unal.edu.co)  
* Matthew Nielsen Guequeta: [mnielsen@unal.edu.co](mailto:mnielsen@unal.edu.co)   
* Juan Manuel Russi Martinez: [jrussim@unal.edu.co](mailto:jrussim@unal.edu.co) 

Descripción  
Se busca realizar una plataforma tipo casino, con término “mini” debido a que solo se presentarán dos juegos, específicamente: tragamonedas y ruleta, con ganancia, pérdidas y registros de x jugador

Objetivo general  
Realizar un sistema básico de casino donde se registre el nombre del integrante y el capital inicial, por medio de dos juegos puede aumentar o disminuir dicho capital, si el usuario pierde todo el capital, deberá realizar “ingreso” de nuevo capital para continuar jugando

Objetivos específicos

1. Desarrollar un sistema básico de registro de usuarios que permite crear y mantener registro de cada usuario registrado  
2. Entender y adaptar el funcionamiento de cada uno de los juegos presentes en el sistema y la interacción usuario-sistema, en los posibles casos de uso del mismo (validación de apuesta, resultado, cálculo de ganancia/pérdida, etc)  
3. Comprender y aplicar los fundamentos de programación en C++ (estructuras de control, funciones, validación de entradas y manejo de archivos) para estructurar el código de manera modular, legible y con persistencia de datos

Funcionalidades principales:

* Gestión de usuarios: Registro y eliminación de jugador, inicio de sesión, consulta de datos.  
* Control de capital: Asignación inicial, validación usuario-sistema (ganancia, pérdida, apuesta no sea mayor al capital), recarga cuando se llegue a cero  
* Tragamonedas: Generación de símbolos, calculo de premios, y visualización de resultados y proceso en la consola  
* Ruleta: Opciones de apuesta (número, color, paridad, rangos), simulación de giro con visualización relativa en consola, determinación de ganador y pago  
* Persistencia: Almacenamiento local en archivos de texto, de los posibles datos como los usuarios y capital, historial de jugadas.  
* Manejo de errores: Validación ingreso adecuado, control números negativos, control de variables y finalización adecuada del programa

Diagrama de flujo:  

```mermaid
    flowchart TD
    Start([Inicio]) --> Load[Cargar datos de usuarios desde archivo]
    Load --> MainMenu{Menú Principal}
    
    MainMenu -->|1. Registrar Usuario| Reg[Ingresar nombre y capital inicial]
    Reg --> SaveReg[Validar y guardar en archivo]
    SaveReg --> MainMenu
    
    MainMenu -->|2. Iniciar Sesión| Login[Ingresar nombre/ID]
    Login --> CheckLogin{¿Usuario válido?}
    CheckLogin -->|No| ErrorMsg[Mostrar error]
    ErrorMsg --> MainMenu
    CheckLogin -->|Sí| PlayerMenu{Menú Jugador}
    
    PlayerMenu -->|1. Jugar Tragamonedas| CheckFunds{¿Capital > 0?}
    PlayerMenu -->|2. Jugar Ruleta| CheckFunds
    PlayerMenu -->|3. Recargar Capital| Recharge[Ingresar monto a recargar]
    Recharge --> UpdateFunds[Actualizar capital]
    UpdateFunds --> PlayerMenu
    PlayerMenu -->|4. Ver Historial| View[Mostrar movimientos guardados]
    View --> PlayerMenu
    PlayerMenu -->|5. Cerrar Sesión| SaveSession[Guardar estado actual]
    SaveSession --> MainMenu
    
    CheckFunds -->|Sí| Bet[Ingresar apuesta]
    CheckFunds -->|No| ForceRecharge[Mostrar: 'Saldo agotado. Debe recargar.']
    ForceRecharge --> Recharge
    
    Bet --> ValidBet{¿Apuesta válida?}
    ValidBet -->|No| Bet
    ValidBet -->|Sí| RunGame[Ejecutar lógica del juego, Tragamonedas o Ruleta]
    RunGame --> Calc[Calcular ganancia/pérdida]
    Calc --> Update[Actualizar capital del usuario]
    Update --> LogMove[Registrar movimiento en archivo]
    LogMove --> PlayAgain{¿Jugar otra vez?}
    PlayAgain -->|Sí| CheckFunds
    PlayAgain -->|No| PlayerMenu
    
    MainMenu -->|3. Salir del Sistema| SaveAll[Guardar todos los datos]
    SaveAll --> End([Fin])
```