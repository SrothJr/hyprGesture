#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RevEng_PAJ7620.h>

// --- Hardware Initialization ---
LiquidCrystal_I2C lcd(0x27, 16, 2); 
RevEng_PAJ7620 sensor = RevEng_PAJ7620(); 

// --- State Machine Variables ---
const int BUTTON_PIN = 2;
volatile bool buttonFlag = false;
int currentMode = 0; // 0: Nav, 1: Media, 2: Reading
const char* modeNames[] = {"NAV", "MEDIA", "READING"};

unsigned long lastActionTime = 0;
const unsigned long COOLDOWN_PERIOD = 500;
bool isReadyState = true;

void setup() {
  Serial.begin(9600);   

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  updateLCD("READY");

  // Initialize Gesture Sensor
  if (!sensor.begin()) { 
    lcd.setCursor(0, 1);
    lcd.print("PAJ ERROR!");
    while (1); 
  }

  //sensor.setGestureEntry(0, GES_ALL)
  // Initialize Button Interrupt
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), isr_buttonPress, FALLING); 
}

void loop() {
  // --- Check State Machine (Mode Switching) ---
  if (buttonFlag) {
    delay(200);
    currentMode = (currentMode + 1) % 3; 
    updateLCD("MODE SWITCHED");
    lastActionTime = millis();
    isReadyState = false;
    buttonFlag = false;
  }

  // --- Poll Gesture Sensor ---
  Gesture gesture = sensor.readGesture(); 

  if (millis() - lastActionTime > COOLDOWN_PERIOD) {
    if (!isReadyState) {
      updateLCD("READY");
      isReadyState = true;
    }


    if (gesture == GES_LEFT) {
      routeGesture("LEFT");
    } else if (gesture == GES_RIGHT) {
      routeGesture("RIGHT");
    } else if (gesture == GES_UP) {
      routeGesture("UP");
    } else if (gesture == GES_DOWN) {
      routeGesture("DOWN");
    } else if (gesture == GES_FORWARD) {
      routeGesture("FORWARD");
    } else if (gesture == GES_BACKWARD) {
      routeGesture("BACKWARD");
    } else if (gesture == GES_CLOCKWISE) {
      routeGesture("CLOCKWISE");
    } else if (gesture == GES_ANTICLOCKWISE) {
      routeGesture("ANTI-CW");
    } else if (gesture == GES_WAVE) {
      routeGesture("WAVE");
    } 
  }
}
// Interrupt Service Routine
void isr_buttonPress() {
  buttonFlag = true;
}

// UI Manager
void updateLCD(const char* action) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("[");
  lcd.print(modeNames[currentMode]);
  lcd.print(": CONN ]"); 
  
  lcd.setCursor(0, 1);
  lcd.print("Gest: ");
  lcd.print(action); 
}

// Routing Logic (Now sends simple strings)
void routeGesture(const char* direction) {
  updateLCD(direction);

  // Send a formatted string to the PC over USB (e.g., "NAV:LEFT")
  Serial.print(modeNames[currentMode]);
  Serial.print(":");
  Serial.println(direction);
  
  lastActionTime = millis();
  isReadyState = false;
}