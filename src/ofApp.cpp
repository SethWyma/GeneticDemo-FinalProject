#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  char buff[FILENAME_MAX];
  _getcwd(buff, FILENAME_MAX);
  assets_path = string(buff) + "\\assets";

  forced_square_ttf_.load(assets_path + "/forced_square.ttf", 32);
  updates_per_frame_ = kDefaultUpdatesPerFrame;

  learning_model_ = LearningModel(assets_path, 1);
  learning_model_.GenerateRandom();

  ofSetBackgroundColor(learning_model_.GetTrack()->GetBackgroundColor());
}

//--------------------------------------------------------------
void ofApp::update(){
  if (!menu_is_open_) {
    for (int i = 0; i < updates_per_frame_; i++) {
      learning_model_.FrameUpdate();
    }
    if (racing_mode_) {
      user_car_.FrameUpdate(user_inputs_);
    }
  }
}

//--------------------------------------------------------------
void ofApp::draw() {
  Track* track = learning_model_.GetTrack();
  track->background_.draw(0, 0,
    track->background_.getWidth() * track->GetScale(),
    track->background_.getHeight() * track->GetScale());

  for (Car car : *learning_model_.GetCars()) {
    DrawCar(&car);
  }

  if (racing_mode_) {
    DrawCar(&user_car_);
  }

  if (menu_is_open_) {
    int height = 50;

    // tell user how many car updates per frame
    forced_square_ttf_.drawString("Current car updates per frame: "
      + std::to_string(updates_per_frame_), 50, height);
    height += 50;

    // render options
    for (string option : kMenuOptions) {
      forced_square_ttf_.drawString(option, 50, height);
      height += 50;
    }
  } else {
    if (!racing_mode_) {
      forced_square_ttf_.drawString(kMenuTip, 50, 50);
    } else {
      forced_square_ttf_.drawString(kResetCarTip, 50, 50);
    }

    string top_fitness_string = "Top fitness: "
      + std::to_string((int)learning_model_.GetTopFitness());
    forced_square_ttf_.drawString(top_fitness_string, 50, 100);

    string generation_number_string = "Generation "
      + std::to_string(learning_model_.GetGenerationNumber());
    forced_square_ttf_.drawString(generation_number_string, 50, 150);

    if (racing_mode_) {
      string user_fitness = "Your fitness: "
        + std::to_string((int)user_car_.GetFitness());
      forced_square_ttf_.drawString(user_fitness, 50, 200);
    }
  }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if (key == 'w') {
    user_inputs_.acceleration = kMaxUserInput;
  } else if (key == 's') {
    user_inputs_.acceleration = -kMaxUserInput;
  } else if (key == 'a') {
    user_inputs_.turning = -kMaxUserInput;
  } else if (key == 'd') {
    user_inputs_.turning = kMaxUserInput;
  }

  if (key == 'n') {
    learning_model_.StartNextGeneration();
    ResetUserCar();
  }

  if (key == 'm') {
    menu_is_open_ = !menu_is_open_;
  }
  // menu options
  if (menu_is_open_) {
    if (key == 'c') {
      ofFileDialogResult folder = ofSystemLoadDialog("Select track folder",
        true, assets_path);
      if (folder.bSuccess) {
        learning_model_.SetTrack(folder.filePath);
        user_car_ = Car(learning_model_.GetTrack(), -1, assets_path);
        user_car_.ChangeImage(assets_path + "/car-recolored.png");
      }
    }
    if (key == 'r') {
      ToggleRaceMode(!racing_mode_);
    }
    if (key == 's') {
      updates_per_frame_++;
    }
    if (key == 'a') {
      updates_per_frame_--;
    }
    if (key == 'q') {
      learning_model_.GenerateRandom();
    }

    updates_per_frame_ = CLAMP(updates_per_frame_, 1, kMaxUpdatesPerFrame);
  }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
  if (key == 'w') {
    user_inputs_.acceleration = 0;
  } else if (key == 's') {
    user_inputs_.acceleration = 0;
  } else if (key == 'a') {
    user_inputs_.turning = 0;
  } else if (key == 'd') {
    user_inputs_.turning = 0;
  }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
  
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
  float min_dim = MIN(w, h);
  float new_scale = min_dim / learning_model_.GetTrack()->background_.getWidth();
  learning_model_.GetTrack()->SetScale(new_scale);
  std::this_thread::sleep_for(std::chrono::milliseconds(kResizeDelayMs));
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::DrawCar(Car* to_draw) {
  ofPushMatrix();

  float track_scale = learning_model_.GetTrack()->GetScale();
  ofTranslate(to_draw->GetX() * track_scale, to_draw->GetY() * track_scale);
  ofRotateRad(to_draw->GetRotation());
  to_draw->image_.draw(
    -to_draw->image_.getWidth() * to_draw->GetScale() / 2,
    -to_draw->image_.getHeight() * to_draw->GetScale() / 2,
    to_draw->image_.getWidth() * to_draw->GetScale(),
    to_draw->image_.getHeight() * to_draw->GetScale());
  ofPopMatrix();
}

void ofApp::ToggleRaceMode(bool new_setting) {
  racing_mode_ = new_setting;

  if (racing_mode_) {
    user_car_ = Car(learning_model_.GetTrack(), -1, assets_path);
    user_car_.ChangeImage(assets_path + "/car-recolored.png");
    updates_per_frame_ = 1;
    learning_model_.SetPopulationSize(5);
    learning_model_.SetAutoAdvanceGeneration(false);
  } else {
    user_car_.Disable();
    updates_per_frame_ = kDefaultUpdatesPerFrame;
    learning_model_.SetPopulationSize(-1);
    learning_model_.SetAutoAdvanceGeneration(true);
  }
}

void ofApp::ResetUserCar() {
  if (racing_mode_) {
    user_car_.ResetPosition();
  }
}