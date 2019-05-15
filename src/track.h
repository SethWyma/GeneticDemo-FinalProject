#pragma once

#include "ofImage.h"
#include <string>

using std::string;
using std::vector;

class Track {
public:

  // Track background image
  ofImage background_;

  // Constructs track from folder path relative to src folder. Requires
  // background image and text file with path points.
  Track(string folder_path);

  // Returns color of top-left pixel
  ofColor GetBackgroundColor() const;

  // Returns color of pixel at start position
  ofColor GetTrackColor() const;

  // Returns track's start position
  vector<float> GetStartPosition() const;

  // Returns track's friction coefficient
  float GetTrackFriction() const;
    
  // Returns track length in pixels
  float GetTrackLength() const;

  // Returns scale of track as float
  float GetScale() const;

  // Sets track scale for path points, start position, and background image
  void SetScale(float scale);

  // Fills path_points with path coordinates from text file path
  void InitializePath(string data_filepath);

  // Returns true if the given coordinate is a legal pixel for a Car to be on
  bool PointIsOnTrack(int x, int y) const;

  // Calculates distance along path to a given point on the track.
  float FindDistAlongTrack(vector<float> position) const;

private:

  // Epsilon for comparing floats
  const float kEpsilon = 0.000001;

  // Amount to decrease car velocity each frame
  const float kFriction = 0.02f;

  // openFrameworks object with background image pixels. Used in PointIsOnTrack
  ofPixels background_pixels_;

  // List of 2D points defining track's path
  vector<vector<float>> path_points_;

  // Starting point of cars on this track
  vector<float> start_position_;

  // Track length in pixels
  int track_length_;

  // Scale of track background and path points
  float scale_;

  // Finds the two points in path_points that are nearest to a given point.
  vector<vector<float>> FindClosestTwoPathPoints(vector<float> point) const;

  // Finds closest point on the line defined by two given points to another
  // given point.
  vector<float> FindClosestPointOnPath(
    vector<vector<float>> line_endpoints, vector<float> point) const;

  // Calculates the square of the distance between two points.
  float GetSquareDist(vector<float> first, vector<float> second) const;
};