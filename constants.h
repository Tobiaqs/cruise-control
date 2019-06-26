/**
   Cruise Control software
   Written by @Tobiaqs
*/

#define TPS1 A0
#define TPS2 A1
#define IDL A2
#define BRK A3
#define TCH A4
#define DIR1 2
#define SPD 3
#define DIR2 4
#define BTN3 5
#define BTN1 6
#define BTN2 7
#define BTN4 8

// Might need some tweaking
#define SIGNIFICANCE_THROTTLE_POSITION 5
#define SIGNIFICANCE_RPM 50

#define MIN_RPM 1250
#define STEP_RPM 100

#define STEP_THROTTLE_POSITION 20
#define MAX_THROTTLE_POSITION 800
#define MIN_THROTTLE_POSITION 25

#define RELEASE_SPEED 255
#define TIGHTEN_SPEED 150
#define MODULATE_UP_SPEED 100
#define MODULATE_DOWN_SPEED 25
#define MODULATE_DELAY 1250

#define THRESHOLD_TCH_HIGH 200
#define THRESHOLD_BRK_HIGH 300

#define CONVERSION_FACTOR 30000000

enum direction { more, less, halt };
