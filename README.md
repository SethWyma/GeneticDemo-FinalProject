# Genetic Algorithm for 2D Driving

This project demonstrates how a genetic algorithm can learn to drive on a 2-dimensional racetrack. The repository includes a simple driving simulator made in openFrameworks and a learning model which evolves cars controlled by feedforward neural networks. The applet visualizes the cars' improvement over many generations on a variety of tracks and allows the user to race the algorithm's cars.

## Getting Started

### Prerequisites

You will need the openFrameworks and OpenNN libraries to run this applet. openFrameworks can be downloaded on [their website](https://openframeworks.cc/download/), and OpenNN can be downloaded from [SourceForge](https://sourceforge.net/projects/opennn/) or [GitHub](https://github.com/Artelnics/OpenNN).

### Installing

In VisualStudio, the demo can be built as a solution with four projects (openFrameworks, OpenNN, tinyxml, and this project). Ensure openFrameworks, OpenNN, and tinyxml are set up as dependencies and references of this project. The applet's startup item is main.cpp in the src directory.

### Running the tests

Tests can be run by excluding main.cpp in the src folder from the project and changing the startup item to test-main.cpp in the test directory.

## Authors
**Seth Wyma**