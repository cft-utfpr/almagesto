#include "MotorController.h"

MotorController::MotorController(MotorAxis axis, uint8_t step_pin, uint8_t dir_pin, uint8_t diag_pin, uint8_t driver_address) {
    this->axis = axis;
    this->step_pin = step_pin;
    this->dir_pin = dir_pin;
    this->diag_pin = diag_pin;
    this->driver_address = driver_address;
}

MotorController::~MotorController() {
    if (driver != nullptr) {
        delete driver;
    }
}

void MotorController::begin(HardwareSerial* serial_port, uint16_t current_ma) {
    pinMode(step_pin, OUTPUT);
    pinMode(dir_pin, OUTPUT);
    pinMode(diag_pin, INPUT_PULLDOWN);

    digitalWrite(step_pin, LOW);
    digitalWrite(dir_pin, LOW);

    // Aloca dinamicamente o driver apontando para a Serial1 configurada no main
    driver = new TMC2209Stepper(serial_port, R_SENSE, driver_address);
    
    driver->begin();
    driver->toff(5);                   // Ativa os MOSFETs
    driver->rms_current(current_ma);   // Corrente do NEMA 17 (ex: 600mA)
    driver->microsteps(MICROSTEPS);    // Configura 1/16 micropassos por software

    // Configuração obrigatória do StallGuard4
    driver->en_spreadCycle(false);     // StealthChop2 ativo
    driver->pwm_autoscale(true);       // Autotuning PWM
    driver->TCOOLTHRS(0xFFFFF);        // Ativa StallGuard em qualquer velocidade
    driver->SGTHRS(STALL_VALUE);       // Limiar de disparo (9)
}

void MotorController::stepPulse(uint32_t delay_us) {
    digitalWrite(step_pin, HIGH);
    delayMicroseconds(delay_us);
    digitalWrite(step_pin, LOW);
    delayMicroseconds(delay_us);
}

void MotorController::calibrate(bool homing_direction) {
    digitalWrite(dir_pin, homing_direction);

    // 1. Gira até o StallGuard detetar o bloqueio mecânico
    while (true) {
        stepPulse(600);

        uint16_t sg_result = driver->SG_RESULT();
        bool diag_state = digitalRead(diag_pin);

        // Disparo por Hardware (DIAG HIGH) ou por Software (SG_RESULT <= 10)
        if (diag_state == HIGH || (sg_result > 0 && sg_result <= 10)) {
            break;
        }
    }

    // 2. Recuo de segurança no sentido oposto (100 passos) para aliviar a tensão mecânica
    digitalWrite(dir_pin, !homing_direction);
    for (int i = 0; i < 100; i++) {
        stepPulse(600);
    }

    // 3. Define a posição inicial como calibrada
    current_angle = 0.0;
    calibrated = true;
}

int32_t MotorController::countSteps(double target_angle) {
    double angle_variation = target_angle - current_angle;
    double steps_per_degree = static_cast<double>(STEPS_PER_REV) / 360.0; // 3200 / 360 = ~8.888 passos/grau

    return static_cast<int32_t>(angle_variation * steps_per_degree);
}

int MotorController::moveToAngle(double target_angle, uint32_t step_delay_us) {
    if (!calibrated) {
        return -1; // Retorna erro se tentar mover antes de calibrar (homing)
    }

    int32_t step_count = countSteps(target_angle);
    if (step_count == 0) return 0;

    // Define o sentido de rotação com base no sinal dos passos
    digitalWrite(dir_pin, (step_count > 0) ? HIGH : LOW);

    int32_t total_steps = abs(step_count);
    for (int32_t i = 0; i < total_steps; i++) {
        stepPulse(step_delay_us);
    }

    current_angle = target_angle;
    return 0;
}