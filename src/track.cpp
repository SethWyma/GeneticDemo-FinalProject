#include "track.h"

Track::Track(string folder_path) {
  background_.load(folder_path + "/track.png");
  background_pixels_ = background_.getPixels();
  InitializePath(folder_path + "/checkpoints.txt");
  assert(path_points_.size() > 1);

  scale_ = 1;
  float path_length = 0;
  vector<float> previous = start_position_;
  for (vector<float> point : path_points_) {
    path_length += sqrt(GetSquareDist(previous, point));
    previous = point;
  }
  path_length += sqrt(GetSquareDist(start_position_,
    path_points_[path_points_.size() - 1]));
  track_length_ = path_length;
}

ofColor Track::GetBackgroundColor() const {
  return background_pixels_.getColor(0, 0);
}

ofColor Track::GetTrackColor() const {
  return background_pixels_.getPixelIndex(start_position_[0], start_position_[1]);
}

vector<float> Track::GetStartPosition() const {
  return start_position_;
}

float Track::GetTrackFriction() const {
  return kFriction;
}

float Track::GetTrackLength() const {
  return track_length_;
}

float Track::GetScale() const {
  return scale_;
}

void Track::SetScale(float scale) {
  if (scale > 0) {
    this->scale_ = scale;
  }
}

bool Track::PointIsOnTrack(int x, int y) const {
  if (x < 0 || y < 0
    || x >= background_.getWidth()
    || y >= background_.getHeight()) {
    return false;
  }
  ofColor color = background_pixels_.getColor(x, y);
  return color.g <= color.b + 100;
}

void Track::InitializePath(string data_filepath) {
  std::ifstream points(data_filepath);
  string data;
  vector<float> point(2);

  while (points >> data) {
    point[0] = std::stoi(data);
    points >> data;
    point[1] = std::stoi(data);
    path_points_.push_back(point);
  }

  assert(!path_points_.empty());

  start_position_ = path_points_[0];
}

float Track::FindDistAlongTrack(vector<float> position) const {
  vector<vector<float>> closest_vertices = FindClosestTwoPathPoints(position);

  vector<float> closest_path_point =
    FindClosestPointOnPath(closest_vertices, position);

  float distance = 0;
  vector<float> previous = start_position_;
  for (vector<float> point : path_points_) {
    distance += sqrt(GetSquareDist(point, previous));
    previous = point;

    if (fabs(point[0] - closest_vertices[0][0]) < kEpsilon
      && fabs(point[1] - closest_vertices[0][1]) < kEpsilon) {
      distance += sqrt(GetSquareDist(closest_path_point, point));
      return distance;
    }
  }
  return 0;
}

vector<vector<float>> Track::FindClosestTwoPathPoints(
  vector<float> point) const {

  // Find closest point, then use closest adjacent path point as point 2
  if (path_points_.size() <= 2) {
    return path_points_;
  }

  vector<float> closest = path_points_[0];
  float smallest_dist = GetSquareDist(point, path_points_[0]);
  int index_of_nearest = 0;

  for (unsigned i = 1; i < path_points_.size(); i++) {
    int path_point_dist = GetSquareDist(point, path_points_[i]);
    if (path_point_dist < smallest_dist) {
      closest = path_points_[i];
      smallest_dist = path_point_dist;
      index_of_nearest = i;
    }
  }

  int next_path_pt_index = (index_of_nearest + 1) % path_points_.size();
  int prev_path_pt_index = (index_of_nearest - 1
    + path_points_.size()) % path_points_.size();

  if (GetSquareDist(point, path_points_[prev_path_pt_index])
    < GetSquareDist(point, path_points_[next_path_pt_index])) {
    return vector<vector<float>>{ path_points_[prev_path_pt_index],
      path_points_[index_of_nearest] };
  }
  else {
    return vector<vector<float>>{ path_points_[index_of_nearest], 
      path_points_[next_path_pt_index] };
  }
}

vector<float> Track::FindClosestPointOnPath(
  vector<vector<float>> line_endpoints, vector<float> point) const {

  // These vectors treat the first point in closest_vertices as the origin
  vector<float> path_vector{ line_endpoints[1][0] - line_endpoints[0][0],
  line_endpoints[1][1] - line_endpoints[0][1] };
  vector<float> position_vector{ point[0] - line_endpoints[0][0],
    point[1] - line_endpoints[0][1] };

  // Coefficient of segment being projected onto in projection equation:
  // ( u dot v) / (norm(v)^2)
  float projection_coefficient =
    (path_vector[0] * position_vector[0] + path_vector[1] * position_vector[1])
    / (pow(path_vector[0], 2) + pow(path_vector[1], 2));

  vector<float> projection{ path_vector[0] * projection_coefficient,
    path_vector[1] * projection_coefficient };

  vector<float> closest_path_point{ projection[0] + line_endpoints[0][0],
    projection[1] + line_endpoints[0][1] };

  return closest_path_point;
}

float Track::GetSquareDist(vector<float> first, vector<float> second) const {
  return pow(first[0] - second[0], 2) + pow(first[1] - second[1], 2);
}