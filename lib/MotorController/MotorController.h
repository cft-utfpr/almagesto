#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <Arduino.h>
#include <TMCStepper.h>

enum class MotorAxis : uint8_t {
    ALTITUDE,
    AZIMUTH
};

class MotorController {
public:
    MotorController(MotorAxis axis, uint8_t step_pin, uint8_t dir_pin, uint8_t diag_pin, uint8_t driver_address);
    ~MotorController();

    // Inicializa o driver TMC2209 e ajusta os registos via UART
    void begin(HardwareSerial* serial_port = &Serial1, uint16_t current_ma = 600);

    // Executa o Sensorless Homing via StallGuard4 até detetar o batente mecânico
    void calibrate(bool homing_direction = LOW);

    // Move o motor para o ângulo alvo em graus
    int moveToAngle(double target_angle, uint32_t step_delay_us = 600);

    // Getters de estado
    double getCurrentAngle() const { return current_angle; }
    bool isCalibrated() const { return calibrated; }

private:
    // NEMA 17 (200 passos/volta) * 16 micropassos = 3200 micropassos/volta
    static constexpr uint16_t FULL_STEPS_PER_REV = 200;
    static constexpr uint8_t MICROSTEPS = 16;
    static constexpr uint32_t STEPS_PER_REV = FULL_STEPS_PER_REV * MICROSTEPS; // 3200

    static constexpr float R_SENSE = 0.11f;
    static constexpr uint8_t STALL_VALUE = 9; // Sensibilidade calibrada para a sua montagem

    MotorAxis axis;
    uint8_t step_pin;
    uint8_t dir_pin;
    uint8_t diag_pin;
    uint8_t driver_address;

    TMC2209Stepper* driver = nullptr;

    bool calibrated = false;
    double current_angle = 0.0;

    void stepPulse(uint32_t delay_us = 600);
    int32_t countSteps(double target_angle);
};

#endif // MOTOR_CONTROLLER_H