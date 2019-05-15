#pragma once

#include <vector>
#include "track.h"
#include "car-inputs.h"
#include "opennn.h"

using namespace OpenNN;

class Car {

public:

  // openFrameworks image of car
  ofImage image_;
  
  // Default constructor
  Car() { };

  // Constructs Car on a track with a unique id (only controlled manually)
  Car(Track* track, int id);

  // Constructs Car controlled by neural network
  Car(Track* track, int id, NeuralNetwork* neural_network, string image_path);

  // Constructs Car with defualt texture
  Car(Track* track, int id, string texture_path);

  // Sets image_ to blue car
  void ChangeImage(string image_path);

  // Updates position, rotation, velocity, fitness, and position validity
  void FrameUpdate(CarInputs current_inputs);

  // Calculates CarInputs for a given frame using ray casts and the Car's NN
  CarInputs CalculateCarInputs() const;

  // Returns ID of car
  int GetId() const;

  // Returns x-coordinate of Car's position
  int GetX() const;

  // Returns y-coordinate of Car's position
  int GetY() const;

  // Returns scale of this Car
  float GetScale() const;

  // Returns fitness of car as number of pixels around track
  float GetFitness() const;

  // Returns number of laps completed by the Car
  int GetLaps() const;

  // Returns pointer to Car's neural network
  NeuralNetwork* GetNeuralNetworkPointer();

  // Returns rotation of Car in radians
  float GetRotation() const;

  // Sets Car's position to Track's starting position
  void ResetPosition();

  // Returns true if Car has crashed
  bool IsDisabled() const;

  // Disables car
  void Disable();

  // Returns true if this Car's fitness is greater than the other's
  bool operator> (const Car& other) const;

  // Returns true if the other Car's fitness is greater than this Car's fitness
  bool operator< (const Car& other) const;

private:

  // Default scale of Car's image
  float kDefaultScale = 0.25;

  // Proportion of image's main diagonals filled by car matter
  float kImageFill = 0.86;

  // Largest meaningful input for turning and acceleration. Larger
  // inputs will be capped at this value.
  float kMaxEffectiveInput = 4.0f;

  // Radians to rotate car when turning (at input level 1.0)
  float kDefaultTurning = 0.005f;

  // Default increase to velocity in one frame (at input level 1.0)
  float kDefaultAcceleration = 0.4f;

  // Decrease to velocity in one frame when braking
  float kMaxBrake = 0.12f;

  // Minimum velocity required to turn
  float kMinTurningVelocity = 0.5f;

  // Largest negative velocity the car can have
  float kMinVelocity = -1.0f;

  // Step size of raycast in pixels
  int kRayCastGranularity = 15;

  // Bearing in radians of each corner of car sprite
  vector<float> kCornerBearings = { -0.3735, 0.3735, 2.7869, 3.4963 };

  // Bearings to cast ray for neural network input
  vector<double> kNnInputBearings = { -1, -0.5, 0, 0.5, 1 };

  // Number of frames to wait before updating fitness
  int kFitnessUpdateFrequency = 10;

  // Pointer to Track this Car is on
  Track* track_;

  // Identifying number of Car. Should be unique
  int id_;

  // Distance from center of car to one of its corners
  float car_radius_;

  // Pointer to NeuralNetwork this car uses to determine how to drive
  NeuralNetwork* neural_network_ = nullptr;

  // Initializes Car object from Track and id. Called by both constructors.
  void init(Track* track, int id, string image_path);

  // Returns distance to wall in a particular bearing (in radians).
  int CastRay(float bearing) const;

  // Rotation of the Car in radians (0 is due east)
  float rotation_rads_ = 0;

  // Position of the Car as two floats
  vector<float> position_;

  // Velocity of Car (increment in position each frame)
  float velocity_ = 0;

  // Distance the Car has made it around its track. A measure of the fitness
  // (how good) this Car's NeuralNetwork can drive.
  float fitness_ = 0;

  // Number of laps the Car has completed around track
  int laps_completed_ = 0;

  // Number of times FrameUpdate() has been called since Car initialization
  int frame_count_ = 0;

  // True if Car can no longer drive (crashed or manually disabled)
  bool disabled_ = false;
};