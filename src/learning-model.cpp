#include "learning-model.h"

LearningModel::LearningModel(string assets_path, int track_number) {
  // OpenNN::Vector cannot be initialized from literal values
  for (unsigned layer_size : kArchitecture) {
    architecture_.push_back(layer_size);
  }
  population_size_ = kDefaultPopulationSize;
  
  this->assets_path = assets_path;
  SetTrack(assets_path + "/track" + std::to_string(track_number));
}

Track* LearningModel::GetTrack() const {
  return track_;
}

void LearningModel::SetTrack(string track_folder) {
  if (track_ != nullptr) {
    delete track_;
  }
  track_ = new Track(track_folder);
  if (!population_.empty()) {
    StartNextGeneration();
  }
}

void LearningModel::SetAutoAdvanceGeneration(bool auto_advance_generation) {
  this->auto_advance_generation = auto_advance_generation;
}

void LearningModel::GenerateRandom() {
  DeleteOldGeneration();
  population_.clear();

  for (int i = 0; i < population_size_; i++) {
    NeuralNetwork* random_network = new NeuralNetwork(architecture_);
    random_network->randomize_parameters_uniform();
    random_network->construct_scaling_layer();

    population_.push_back(Car(track_, i, random_network, assets_path));
  }

  generation_number_ = 1;
  disabled_count_ = 0;
}

vector<Car>* LearningModel::GetCars() {
  return &population_;
}

void LearningModel::FrameUpdate() {
  if (!auto_advance_generation
    || (generation_frame_count_ < kMaxGenerationFrames
      && disabled_count_ < population_size_)
    ) {
    for (Car &car : population_) {
      if (car.IsDisabled()) {
        continue;
      }

      CarInputs inputs = car.CalculateCarInputs();
      car.FrameUpdate(inputs);
      if (car.IsDisabled()) {
        disabled_count_++;
      }
    }
  }
  else {
    StartNextGeneration();
  }
  generation_frame_count_++;
}

int LearningModel::GetFrameCount() const {
  return generation_frame_count_;
}

int LearningModel::GetGenerationNumber() const {
  return generation_number_;
}

float LearningModel::GetTopFitness() const {
  Car most_fit = *std::max_element(population_.begin(),
    population_.end(), std::less<Car>());

  return most_fit.GetFitness();
}

void LearningModel::StartNextGeneration() {
  std::sort(population_.begin(), population_.end(), std::greater<Car>());
  vector<Car> new_population(population_size_);

  Vector<int> random_indices(population_size_ * 2);
  random_indices.randomize_normal(0, kSelectionStandardDeviation *
    population_.size() / kDefaultPopulationSize);
  random_indices.apply_absolute_value();
  random_indices.apply_upper_bound(population_.size() - 1);

  for (unsigned i = 0; i < kCopyToNextGeneration
    && i < new_population.size(); i++) {
    NeuralNetwork* offspring_network = new NeuralNetwork(
      *population_[i % population_.size()].GetNeuralNetworkPointer());

    new_population[i] = Car(track_,
      generation_number_ * population_size_ + i,
      offspring_network, assets_path);
  }

  for (int i = kCopyToNextGeneration; i < population_size_; i++) {

    NeuralNetwork* parent_one = population_[random_indices[i * 2]]
      .GetNeuralNetworkPointer();
    NeuralNetwork* parent_two = population_[random_indices[i * 2 + 1]]
      .GetNeuralNetworkPointer();
    NeuralNetwork* offspring_network =
      RecombineNeuralNetworks(parent_one, parent_two);

    new_population[i] = Car(track_, generation_number_ * population_size_ + i,
      offspring_network, assets_path);
  }

  DeleteOldGeneration();
  population_ = new_population;
  generation_frame_count_ = 0;
  disabled_count_ = 0;
  generation_number_++;
}

void LearningModel::SetPopulationSize(int new_size) {
  if (new_size < 0) {
    population_size_ = kDefaultPopulationSize;
  } else {
    population_size_ = new_size;
  }
  StartNextGeneration();
}

NeuralNetwork* LearningModel::RecombineNeuralNetworks(NeuralNetwork* first,
  NeuralNetwork* second) {

  Vector<Vector<double>> new_params;

  Vector<Vector<double>> first_params =
    first->get_multilayer_perceptron_pointer()->get_layers_parameters();
  Vector<Vector<double>> second_params =
    second->get_multilayer_perceptron_pointer()->get_layers_parameters();
  // Simple algorithm, likely to change.
  for (unsigned layer = 0; layer < first_params.size(); layer++) {
    new_params.push_back(Vector<double>(first_params[layer].size()));
    for (unsigned node = 0; node < first_params[layer].size(); node++) {
      new_params[layer][node] =
        (first_params[layer][node] + second_params[layer][node]) / 2;
    }
  }

  MultilayerPerceptron recombined_perceptrons(architecture_);
  recombined_perceptrons.set_layers_parameters(new_params);
  recombined_perceptrons.perturbate_parameters(kMutationRate);
  
  return new NeuralNetwork(recombined_perceptrons);
}

void LearningModel::DeleteOldGeneration() {
  for (unsigned i = 0; i < population_.size(); i++) {
    delete population_[i].GetNeuralNetworkPointer();
  }
}