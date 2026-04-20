% César Florez Franco
% Camilo Andres Monsalve Martinez
% Santiago Muñoz Zapata

%% Test de análisis – Control de servo por Serial (Arduino + MATLAB)
% Profesor Juan Guillermo Patiño Vengoechea 
% Microcontroladores  ---- 24/02/2026
%
% Archivo: Test_Analisis_Servo_Serial.m
%
% Uso:
%   1) Ejecuta el archivo.
%   2) Soluciona cada punto es este mismo archivo.
%
% Nota:
%   Este test evalúa comprensión de comunicación Serial, Servo.h, validaciones,
%   y depuración básica (puertos, alimentación, sincronización).

clear; clc;

disp("==============================================================");
disp("  TEST DE ANÁLISIS – Control de servo por Serial (Arduino + MATLAB)");
disp("  Duración sugerida: 120 min | Puntaje total: 100");
disp("==============================================================");
disp("INSTRUCCIONES: Responde con claridad. Justifica cuando se pida.");
disp(" ");

%% Sección A — Comprensión conceptual (10 preguntas, 3 pts c/u = 30 pts)
disp("SECCIÓN A — Comprensión conceptual (30 pts)");
disp("1) ¿Cuál es el propósito principal de Serial.begin(9600) en Arduino?");
disp("   a) Activar PWM en el pin del servo");
disp("   b) Inicializar comunicación serial para recibir/enviar datos con el PC"); % RESPUESTA
disp("   c) Encender el servo automáticamente");
disp("   d) Definir la velocidad del servo");
disp(" ");

disp("2) ¿Qué hace Griper.attach(Pin_Servo_Griper)?");
disp("   a) Convierte el ángulo a microsegundos");
disp("   b) Asocia el objeto Servo a un pin digital para generar los pulsos"); % RESPUESTA
disp("   c) Lee el ángulo desde el puerto serial");
disp("   d) Limpia el buffer serial");
disp(" ");

disp("3) En Arduino, Serial.available() > 0 se usa para:");
disp("   a) Asegurar que el servo esté alimentado");
disp("   b) Evitar leer cuando no hay datos entrantes"); % RESPUESTA
disp("   c) Aumentar la velocidad del puerto serial");
disp("   d) Cambiar el sentido del barrido");
disp(" ");

disp("4) Serial.parseInt() en Arduino:");
disp("   a) Lee un número entero del flujo serial buscando dígitos"); % RESPUESTA
disp("   b) Lee siempre exactamente un byte");
disp("   c) Solo funciona si llega un float");
disp("   d) Solo funciona si no hay saltos de línea");
disp(" ");

disp("5) ¿Por qué el código valida if (Angle >= 0 && Angle <= 180)?");
disp("   a) Porque el servo solo existe entre 0 y 180");
disp("   b) Para evitar mandar comandos fuera del rango típico de un servo"); % RESPUESTA
disp("   c) Para que MATLAB no tenga que validar");
disp("   d) Para que el puerto serial no se bloquee");
disp(" ");

disp("6) En MATLAB, ¿qué hace serialport('COM6',9600)?");
disp("   a) Crea un archivo de texto en COM6");
disp("   b) Abre una conexión serial por el puerto COM6 a 9600 baudios"); % RESPUESTA
disp("   c) Enciende el Arduino");
disp("   d) Convierte el servo a modo PWM");
disp(" ");

disp("7) ¿Por qué se usa pause(2) después de abrir el puerto serial en MATLAB?");
disp("   a) Para que MATLAB calcule el rango");
disp("   b) Para dar tiempo a que el microcontrolador inicie (a veces se reinicia al abrir el serial)");% RESPUESTA
disp("   c) Para estabilizar el servo mecánicamente");
disp("   d) Para vaciar el workspace");
disp(" ");

disp("8) ¿Qué hace writeline(MCU, num2str(angle))?");
disp("   a) Envía el ángulo como texto y añade terminador (salto de línea)"); % RESPUESTA
disp("   b) Envía el ángulo como binario puro");
disp("   c) Dibuja el ángulo en una gráfica");
disp("   d) Lee el ángulo desde Arduino");
disp(" ");

disp("9) En el barrido Theta_Min:5:Theta_Max, el número 5 significa:");
disp("   a) 5 segundos de espera");
disp("   b) Paso de 5 grados entre cada comando enviado"); % RESPUESTA
disp("   c) 5 bytes enviados");
disp("   d) 5 veces se repite el ciclo total");
disp(" ");

disp("10) Causa MUY común de que el servo no se mueva aunque el código esté bien:");
disp("    a) El puerto serial está muy rápido");
disp("    b) Falta alimentación adecuada del servo o falta GND común con Arduino"); % RESPUESTA
disp("    c) MATLAB no soporta servos"); 
disp("    d) clear en MATLAB impide que el servo funcione");
disp(" ");

%% Sección B — Lectura y trazado (5 preguntas, 7 pts c/u = 35 pts)
disp("SECCIÓN B — Lectura y trazado (35 pts)");
disp("11) Si Theta_Min = 0 y Theta_Max = 180, ¿cuántos comandos de ángulo se envían en el barrido de subida?");
disp(" barrido ascendente desde el minimo hasta el maximo en pasos de 5 grados y -5 grados descendentescal, la cantidad de comandos serian: 180/5 = 35 ");
disp(" ");

disp("12) Si MATLAB envía el texto '90\\n' (90 con salto de línea), ¿qué valor obtiene Angle = Serial.parseInt() en Arduino?");
disp(" obtiene un valor de 90 grados de inmediato debido al salto de linea. el movimiento se hace sin retraso");
disp(" ");

disp("13) Si el estudiante ingresa Theta_Min = 170 y Theta_Max = 160, ¿qué ocurre en MATLAB y por qué?");
disp(" ocurre un error. debido a que el código tiene condiciones. * que ambos sean números. *que estén en un rango de 0 a 180. *que el mínimo sea menor que el máximo.");
disp("son 37 comandos")
disp(" ");

disp("14) En Arduino, ¿qué pasa si MATLAB envía un valor fuera de rango como 250?");
disp("Servo NO se mueve. Programa continúa esperando siguiente comando.");
disp(" En la linea: ‘if (Angle >= 0 && Angle <= 180)’ es FALSE por lo tanto ‘Griper.write(250)’ no se ejecuta.");
disp(" ");

disp("15) Describe, en forma de flujo de ejecución, la secuencia completa desde que MATLAB envía un ángulo hasta que el servo se mueve.");
disp(" MATLAB writeline('90') envía '90\\n' por COM6.");
disp(" Arduino Serial.available() > 0 detecta datos.");
disp(" Serial.parseInt() obtiene Angle = 90.");
disp(" if (90 >= 0 && 90 <= 180) valida TRUE.");
disp(" Griper.write(90) genera pulsos ~1500µs.");
disp(" Servo se mueve a 90°.");
disp(" Arduino Serial.println('Ángulo: 90') confirma.");
disp(" ");

%% Sección C — Depuración y mejora (5 preguntas, 7 pts c/u = 35 pts)
disp("SECCIÓN C — Depuración y mejora (35 pts)");
disp("16) Se detecta que el programa Arduino NO compila por un error de escritura en una función del servo.");
disp("El error es que escribieron 'Griper.write(angle)' con 'angle' en minuscula.");
disp("La correction es cambiar por 'Griper.write(Angle)' (la A mayúscula como la variable).");
disp(" ");

disp("17) El servo 'tiembla' o se mueve con saltos bruscos. Propón dos ajustes en MATLAB para suavizar el movimiento y justifica cada uno.");
disp("Pasos mas pequeños de angulos: Theta_Min:1:Theta_Max y reducir la pausa pause(0.2) ");
disp(" ");

disp("18) Si el puerto COM6 no existe, ¿qué harías en MATLAB para identificar los puertos disponibles?");
disp(" con el comando 'serialportlist' puedo ver los puertos disponibles, lo asigno a una variable y");
disp(" dentro de un if-else puedo capturar el error en caso de no detectar puertos o mostar los existentes o con 'serialportlist(available)' veo los  puertos libres ");
disp(" ");

disp("19) Propón una mejora para hacer la comunicación más confiable: que Arduino confirme cada ángulo recibido.");
disp(" luego de recibir el angulo por 'Serial.parseInt' para que arduino devuelva un mensaje OK, asi matlab sabra que fue procesado");
disp(" matlab puede leer el puerto serial y verificar que la confirmacion llego antes de enviar el otro ángulo.");

disp("20) Seguridad y hardware: menciona dos buenas prácticas para conectar el servo y evitar problemas eléctricos o reinicios del Arduino.");
disp("1. Alimentar servo con fuente externa 5V (no del USB Arduino)");
disp("2. Conectar GND del servo con GND del Arduino");
disp(" ");