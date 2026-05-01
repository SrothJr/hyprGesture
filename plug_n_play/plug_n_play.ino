#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RevEng_PAJ7620.h>
#include <SoftwareSerial.h>

// --- Hardware Initialization ---
LiquidCrystal_I2C lcd(0x27, 16, 2); 
SoftwareSerial CH9350(4, 3);        // RX, TX (Arduino Pin 3 sends data to CH9350 RX)

RevEng_PAJ7620 sensor = RevEng_PAJ7620(); 

// --- State Machine Variables ---
const int BUTTON_PIN = 2;
volatile bool buttonFlag = false;
int currentMode = 0; // 0: Nav, 1: Media, 2: Reading
const char* modeNames[] = {"NAV", "MEDIA", "READING"};

void setup() {
  Serial.begin(9600);   
  CH9350.begin(9600);   

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
    buttonFlag = false;
  }

  // --- Poll Gesture Sensor ---
  Gesture gesture = sensor.readGesture(); 

  if (gesture == GES_LEFT) {
    routeGesture("LEFT");
  } else if (gesture == GES_RIGHT) {
    routeGesture("RIGHT");
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

// Routing Logic
void routeGesture(const char* direction) {
  updateLCD(direction);

  if (currentMode == 0) { 
    // Modifier 0x0E = SUPER (0x08) + SHIFT (0x02) + ALT (0x04)
    if (strcmp(direction, "LEFT") == 0) sendKeystroke(0x0E, 0x50);  // Left Arrow
    if (strcmp(direction, "RIGHT") == 0) sendKeystroke(0x0E, 0x4F); // Right Arrow
  }
  else if (currentMode == 1) { // Media Mode 
    if (strcmp(direction, "LEFT") == 0) sendKeystroke(0x00, 0x4B); // Page Up (Used as placeholder)
  }
  
  delay(400);
  updateLCD("READY");
}

// USB HID Protocol Handler
void sendKeystroke(byte modifier, byte keycode) {
  // Data frame required by CH9350 
  byte packet[11] = {0x57, 0xAB, 0x00, 0x02, 0x08, modifier, 0x00, keycode, 0x00, 0x00, 0x00};
  CH9350.write(packet, 11); 
  
  delay(20);
  
  // Release frame 
  byte releasePacket[11] = {0x57, 0xAB, 0x00, 0x02, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
  CH9350.write(releasePacket, 11); 
}