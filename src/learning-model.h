#pragma once

#include "car.h"
#include "opennn.h"

using namespace OpenNN;

class LearningModel {

public:

  // Default constructor
  LearningModel() { }

  // Construct LearningModel on a Track
  LearningModel(string assets_dir, int track_number);

  // Get pointer to LearningModel's current Track
  Track* GetTrack() const;

  // Start next generation on new track
  void SetTrack(string track_folder);

  // Set to false to prevent LearningModel from automatically calling
  // StartNextGeneration at kMaxGenerationFrames or when all Cars are disabled.
  void SetAutoAdvanceGeneration(bool auto_advance_generation);

  // Generation kDefaultPopulationSize Cars with random NeuralNetworks
  void GenerateRandom();

  // Calculate next inputs and call FrameUpdate on each Car in the population
  void FrameUpdate();

  // (debug) Returns number of frames completed in the current generation
  int GetFrameCount() const;

  // Returns generation number of cars in this learning model
  int GetGenerationNumber() const;

  // Returns fitness of most-fit Car in population
  float GetTopFitness() const;

  // Return pointer to population
  vector<Car>* GetCars();

  // Reset generation-specific variables and recombine most fit Cars to create
  // next generation of Cars
  void StartNextGeneration();

  // Reduces population_size to kCopyToNextGeneration. No learning will occur
  void SetPopulationSize(int new_size);

private:

  // Number of Cars in each generation
  int kDefaultPopulationSize = 50;

  // Standard deviation of Car rankings chosen as parents for next generation's
  // offspring. Should be no more than kDefaultPopulationSize / 3
  float kSelectionStandardDeviation = 6;

  // Number of top-performing Cars to directly copy to next generation
  int kCopyToNextGeneration = 8;

  // Input to NeuralNetwork's perturbate_paramters method when mutating
  // offspring. Higher is more mutation.
  float kMutationRate = 1.0;

  // Maximum number of frames to run a single generation
  int kMaxGenerationFrames = 6000;

  // Architecture of Car NeuralNetworks. Each int represents the number of
  // nodes in a layer
  vector<int> kArchitecture = { 4, 3, 3, 2 };

  // Path of assets directory
  string assets_path;

  // Pointer to Track the Cars in this LearningModel are driving on. Used only
  // when constructing new Cars
  Track* track_ = nullptr;

  // All Cars in the current generation's population
  vector<Car> population_;

  // Current size of population
  int population_size_;

  // NeuralNetwork architecture with which to generate new Car NeuralNetworks
  Vector<unsigned> architecture_;

  // Number of Cars that have been disabled/crashed in the current generation
  int disabled_count_ = 0;

  // Advance generation when generation_frame_count_ reaches
  // kMaxGenerationFrames or all Cars are disabled
  bool auto_advance_generation = true;

  int generation_frame_count_ = 0;
  int generation_number_ = 1;

  // Combine two NeuralNetworks into a rough average of the two with mutations
  // according to the NeuralNetwork perturbate_parameters method.
  NeuralNetwork* RecombineNeuralNetworks(NeuralNetwork* first,
    NeuralNetwork* second);

  // Deletes NeuralNetwork pointers in each Car of the current generation
  void DeleteOldGeneration();
};