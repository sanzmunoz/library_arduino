clear;
% 1) Borra variables del workspace.
%    ¿Para qué? Evita que queden objetos serial viejos o valores anteriores.

close all;
% 2) Cierra figuras.
%    ¿Para qué? Limpia la sesión si antes se graficó algo.

% Ingresamos el rango de movimiento
Inputs = {'Ingrese el ángulo mínimo (0-180°)','Ingrese el ángulo máximo (0-180°):'};
% 3) Crea las preguntas que saldrán en el cuadro de diálogo.

Title = 'Rango de Movimiento del Servo';
% 4) Título de la ventana de entrada.

Dimdlg = [1 60];
% 5) Define tamaño del cuadro de diálogo: 1 fila, 60 caracteres aprox.

Inputdefault = {'0','180'};
% 6) Valores por defecto para que el usuario no tenga que escribir siempre lo mismo.

Answer = inputdlg(Inputs, Title, Dimdlg, Inputdefault);
% 7) Abre la ventana y captura respuestas como celdas de texto (strings).

if isempty(Answer)
    disp('Ejecución Cancelada')
    % 8) Si el usuario presiona “Cancel”, Answer queda vacío.
    %    ¿Para qué? Para salir sin error.
    return;
end

Theta_Min = str2double(Answer{1});
% 9) Convierte el texto del ángulo mínimo a número (double).
%    ¿Por qué str2double? Porque inputdlg devuelve texto.

Theta_Max = str2double(Answer{2});
% 10) Convierte el texto del ángulo máximo a número.

% Validación de las entradas
if isnan(Theta_Min) || isnan(Theta_Max) || Theta_Min < 0 || ...
        Theta_Max > 180 || Theta_Min >= Theta_Max
    % 11) Valida:
    %     - Que ambos sean números (no NaN)
    %     - Que estén en rango 0..180
    %     - Que el mínimo sea menor que el máximo
    error('Valores invalidos. Verifique que 0 <= Theta_Min < Theta_Max <= 180');
end

% Configuramos la conexión serial con el ESP32 (o Arduino)
MCU = serialport("COMX", 9600); % IMPORTANTE: X ---> Número de puerto según asignación del PC
% 12) Abre el puerto serial COMX a 9600 baudios.

pause(2);
% 13) Espera para que el microcontrolador “termine de iniciar”.
%     (En muchos Arduino, al abrir el serial se reinician; esta pausa evita perder los primeros datos.)

disp('Servo en movimiento')

% Paso pequeño + pausa pequeña → movimiento más “suave” pero más datos.
% Paso grande + pausa grande → movimiento “a saltos” pero más simple.

for angle = Theta_Min:5:Theta_Max
    % 14) Barrido ascendente desde el mínimo al máximo en pasos de 5 grados.
    
    writeline(MCU, num2str(angle));
    % 15) Envía el número como texto por el serial y añade terminador (newline).
    %     ¿Para qué writeline? Porque facilita mandar líneas completas (ej: "90\n").
    %     Esto combina bien con Serial.parseInt() en Arduino.

    pause(0.5);
    % 16) Da tiempo al servo a moverse antes de enviar el siguiente ángulo.
end

for angle = Theta_Max:-5:Theta_Min
    % 17) Barrido descendente desde el máximo al mínimo en pasos de -5.

    writeline(MCU, num2str(angle));
    % 18) Envía el ángulo nuevamente.

    pause(0.5);
    % 19) Espera para permitir el movimiento físico.
end

clear MCU
% 20) Elimina el objeto serial del workspace y libera el puerto COM.
%     ¿Para qué? Para que no quede bloqueado el COMX.

disp('Ejecución Finalizada')