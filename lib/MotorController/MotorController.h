#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <Arduino.h>

enum class MotorAxis : uint8_t {
    ALTITUDE,
    AZIMUTH
};

class MotorController {
public:
    MotorController(MotorAxis axis, uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4);

    // Configure motor
    void begin();

    // Step the motor until it stops
    void calibrate();

    // Step the motor to the target angle in degrees
    int moveToAngle(double target_angle);

private:
    static constexpr uint16_t STEPS_PER_REV = 2048;
    static constexpr uint8_t STEP_SEQUENCE[4][4] = {
        {HIGH, LOW,  LOW,  LOW},
        {LOW,  HIGH, LOW,  LOW},
        {LOW,  LOW,  HIGH, LOW},
        {LOW,  LOW,  LOW,  HIGH}
    };

    MotorAxis axis;
    uint8_t in1, in2, in3, in4;

    bool calibrated = false;
    double current_angle = 0.0;
    int32_t current_step;

    void stopPins();
    void step(int32_t step_number);
    int32_t countSteps(double target_angle);
};

#endif // MOTOR_CONTROLLER_H