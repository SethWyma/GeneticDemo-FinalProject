#pragma once

struct CarInputs {
  CarInputs() {
    this->acceleration = 0;
    this->turning = 0;
  }

  CarInputs(float acceleration, float turning) {
    this->acceleration = acceleration;
    this->turning = turning;
  }

  float acceleration;
  float turning;
};