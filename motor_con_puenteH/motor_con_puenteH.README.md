# motor_con_puenteH.ino

DC motor with gradual acceleration/deceleration via pushbuttons. Status LEDs indicate motor state.

**Key Features:** Smooth ramp (70ms steps), dual-button control, 3 status LEDs, PWM 0-255, serial feedback.

**Pins:** Accel Button (10), Brake Button (11), LEDs (13,12,8), Motor PWM (9).

**States:** Off → Accelerating → Running → Braking.

**Board:** Arduino Mega 2560 | **Baud:** 9600
