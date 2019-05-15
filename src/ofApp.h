#pragma once

#include <direct.h>
#include "ofMain.h"
#include "track.h"
#include "car-inputs.h"
#include "car.h"
#include "learning-model.h"

class ofApp : public ofBaseApp{

public:
    
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

private:

  // Delay in ms between attempts to change track size as window resizes
  const long kResizeDelayMs = 100;

  // Default number of times FrameUpdate is called in the app's LearningModel
  const int kDefaultUpdatesPerFrame = 10;

  // Maximum possible car updates per frame
  const int kMaxUpdatesPerFrame = 100;

  // Input magnitude sent to the CarInputs controlling the user's car
  const float kMaxUserInput = 4.0;

  const string kMenuTip = "M: open menu";

  const string kResetCarTip = "N: reset car positions";

  const vector<string> kMenuOptions = {
    "N: Next Generation (always available)",
    "C: Change Track",
    "R: Toggle Racing Mode",
    "S: Increase Simulation Speed",
    "A: Decrease Simulation Speed",
    "Q: Reset Population"
  };

  // Absolute path to project assets folder
  string assets_path;

  // True if menu is open (temporarily disables all Cars)
  bool menu_is_open_;

  // True if user-controlled Car is on track
  bool racing_mode_;

  // Number of times to call FrameUpdate in this app's LearningModel
  int updates_per_frame_;

  // Font used to write text to screen
  ofTrueTypeFont forced_square_ttf_;

  // Current inputs to user's car; changes with key presses/releases
  CarInputs user_inputs_;

  // LearningModel object for this app
  LearningModel learning_model_;

  // Car object controlled by user when racing_mode_ is true
  Car user_car_;

  // Draws a single Car on the screen with correct position and rotation
  void DrawCar(Car* to_draw);

  // Creates user-controlled car on track
  void ToggleRaceMode(bool new_setting);

  // Returns user's car to track's starting position
  void ResetUserCar();
};
