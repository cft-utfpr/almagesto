#include <Arduino.h>

#include "MotorController.h"

MotorController::MotorController(MotorAxis axis, uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4) {
    this->axis = axis;
    this->in1 = in1;
    this->in2 = in2;
    this->in3 = in3;
    this->in4 = in4;
}

void MotorController::begin() {
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    stopPins();
}

void MotorController::calibrate() {
    calibrated = true;
    current_angle = 0.0;
}

int MotorController::moveToAngle(double target_angle) {
    int32_t step_count = countSteps(target_angle);
    int8_t step_direction = (step_count >= 0) ? 1 : -1;

    for (int32_t i = 0; i < abs(step_count); i++) {
        step(step_direction > 0 ? i : -i); 
        delay(5);
    }

    current_angle = target_angle;

    return 0;
}

void MotorController::stopPins() {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}

void MotorController::step(int32_t step_number) {  
    int step_index = abs(step_number) % 4;
    
    if (step_number > 0) {
        step_index = 3 - step_index;
    }
    
    digitalWrite(in1, STEP_SEQUENCE[step_index][0]);
    digitalWrite(in2, STEP_SEQUENCE[step_index][1]);
    digitalWrite(in3, STEP_SEQUENCE[step_index][2]);
    digitalWrite(in4, STEP_SEQUENCE[step_index][3]);
}

constexpr uint16_t MotorController::STEPS_PER_REV;
constexpr uint8_t MotorController::STEP_SEQUENCE[4][4];

int32_t MotorController::countSteps(double target_angle) {
    double angle_variation = target_angle - current_angle;
    int32_t step_count = angle_variation / (360.0 / static_cast<double>(STEPS_PER_REV));

    return step_count;
}