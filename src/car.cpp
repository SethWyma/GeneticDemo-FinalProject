#include "car.h"

Car::Car(Track* track, int id, NeuralNetwork* neural_network,
  string image_dir) {
  init(track, id, image_dir);

  if (neural_network != nullptr) {
    this->neural_network_ = neural_network;
  }
}

Car::Car(Track* track, int id, string image_dir) {
  init(track, id, image_dir);
}

void Car::init(Track* track, int id, string image_dir) {
  image_.load(image_dir + "/car.png");
  this->id_ = id;
  this->track_ = track;

  vector<float> start_position = track->GetStartPosition();
  position_.push_back(start_position[0]);
  position_.push_back(start_position[1]);
  car_radius_ = kImageFill * sqrt(pow(image_.getWidth(), 2)
    + pow(image_.getHeight(), 2)) / 2;
}

void Car::ChangeImage(string image_path) {
  image_.load(image_path);
}

int Car::GetId() const {
  return id_;
}

void Car::FrameUpdate(CarInputs inputs) {
  if (disabled_) return;
  
  inputs.acceleration = CLAMP(inputs.acceleration,
    -kMaxEffectiveInput, kMaxEffectiveInput);
  inputs.turning = CLAMP(inputs.turning,
    -kMaxEffectiveInput, kMaxEffectiveInput);

  if (velocity_ > kMinVelocity) {
    velocity_ += inputs.acceleration * kDefaultAcceleration / (abs(velocity_) + 1);
  }

  if (fabs(velocity_) > kMinTurningVelocity) {
    rotation_rads_ += inputs.turning * kDefaultTurning;
  }

  position_[0] += velocity_ * cos(rotation_rads_) * GetScale();
  position_[1] += velocity_ * sin(rotation_rads_) * GetScale();

  if (abs(velocity_) < track_->GetTrackFriction()) {
    velocity_ = 0;
  }
  else {
    velocity_ > 0 ? velocity_ -= track_->GetTrackFriction()
      : velocity_ += track_->GetTrackFriction();
  }

  if (frame_count_ % kFitnessUpdateFrequency == 0) {
    float curr_lap_dist = track_->FindDistAlongTrack(position_);

    float fitness_diff = laps_completed_ * track_->GetTrackLength()
      + curr_lap_dist - fitness_;
    if (fabs(fitness_diff) > track_->GetTrackLength() / 2) {
      fitness_diff > 0 ? laps_completed_ -= 1 : laps_completed_ += 1;
    }

    fitness_ = laps_completed_ * track_->GetTrackLength() + curr_lap_dist;
  }

  for (float bearing : kCornerBearings) {
    if (CastRay(bearing) < car_radius_ * GetScale()) {
      disabled_ = true;
    }
  }

  frame_count_++;
}

CarInputs Car::CalculateCarInputs() const {
  CarInputs car_inputs(0, 0);
  if (neural_network_ == nullptr) {
    return car_inputs;
  }

  // copy from std::vector to OpenNN::Vector
  Vector<double> nn_inputs(kNnInputBearings.size() + 1);
  for (unsigned i = 0; i < kNnInputBearings.size(); i++) {
    nn_inputs[i] = CastRay(kNnInputBearings[i]);
  }
  nn_inputs[nn_inputs.size() - 1] = velocity_;
  Vector<double> nn_outputs = neural_network_->calculate_outputs(nn_inputs);

  assert(nn_outputs.size() == 2);
  car_inputs.acceleration = nn_outputs[0];
  car_inputs.turning = nn_outputs[1];

  return car_inputs;
}

int Car::GetX() const {
  return (int)position_[0];
}

int Car::GetY() const {
  return (int)position_[1];
}

float Car::GetScale() const {
  return kDefaultScale * track_->GetScale();
}

float Car::GetFitness() const {
  return fitness_;
}

int Car::GetLaps() const {
  // The notion of negative laps completed does not make sense.
  // laps_completed can store negative values so fitness is tracked properly,
  // but the UI will not show negative laps completed.
  return CLAMP(laps_completed_, 0, INT32_MAX);
}

NeuralNetwork* Car::GetNeuralNetworkPointer() {
  return neural_network_;
}

int Car::CastRay(float bearing) const {
  float direction = rotation_rads_ + bearing; //radians cw from straight east
  vector<float> direction_ray{ cos(direction), sin(direction) };
  
  int distance = 0;
  vector<float> ray_tip(position_);
  while (track_->PointIsOnTrack(ray_tip[0], ray_tip[1])) {
    ray_tip[0] += kRayCastGranularity * direction_ray[0];
    ray_tip[1] += kRayCastGranularity * direction_ray[1];
    distance += kRayCastGranularity;
  }
  while (!track_->PointIsOnTrack(ray_tip[0], ray_tip[1])) {
    ray_tip[0] -= direction_ray[0];
    ray_tip[1] -= direction_ray[1];
    distance -= 1;
  }
  return distance * track_->GetScale() + 1;
}

float Car::GetRotation() const {
  return rotation_rads_;
}

void Car::ResetPosition() {
  position_ = track_->GetStartPosition();
  fitness_ = 0;
  laps_completed_ = 0;
  rotation_rads_ = 0;
  velocity_ = 0;
  disabled_ = false;
}

bool Car::IsDisabled() const {
  return disabled_;
}

void Car::Disable() {
  disabled_ = true;
}

bool Car::operator> (const Car& other) const {
  return this->fitness_ > other.GetFitness();
}

bool Car::operator< (const Car& other) const {
  return this->fitness_ < other.GetFitness();
}