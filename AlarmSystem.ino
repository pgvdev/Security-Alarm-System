#include <LiquidCrystal.h>
#include <IRremote.h>
#include <Servo.h>
#include <EEPROM.h>



// Pin Setup
const int trigPin = 4; // Ultrasonic sensor trigger pin
const int echoPin = 6; // Ultrasonic sensor echo pin
const int buzzerPin = 3; // Buzzer pin
const int receiverPin = 13; // IR receiver pin
const int redPin = A2; // RGB LED Red channel
const int greenPin = A1; // RGB LED Green channel
const int bluePin = A0; // RGB LED Blue channel
const int servoPin = 5; // Servo motor control pin

// IR Remote Setup
int btnValue = 0; // Holds the value of the IR button pressed
IRrecv receiver(receiverPin); // Create an IR receiver object for the specified pin

// LCD Setup
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); 

// Servo Motor Setup
Servo myServo; // Create a servo object for controlling the Alarm lock

// Button Values
const int TOGGLE_LOCK_BUTTON = 162; // Button value to toggle lock state
const int SET_CODE_BUTTON = 194; // Button value to set a new lock code
const int NUM_BUTTONS[] = {104, 48, 24, 122, 16, 56, 90, 66, 74, 82}; // Button values corresponding to digits 0-9

// System States
bool isLocked = true; // Tracks whether the Alarm is locked or unlocked
char currentCode[5]; // Stores the current lock code
char enteredCode[5]; // Temporary storage for entered code
unsigned long lastButtonPressTime = 0; // Tracks the last IR button press
const unsigned long debounceDelay = 200; // Debounce delay to avoid multiple triggers

// Ultrasonic Sensor Variables
unsigned long lastSensorCheckTime = 0; // Tracks the last ultrasonic sensor check
unsigned long sensorDelay = 100; // Time interval between sensor checks
bool objectDetected = false; // Tracks if an object is detected
unsigned long objectDetectionStartTime = 0; // Start time of object detection
unsigned long objectDetectionDuration = 2000; // Buzzer duration when an object is detected


void displayMessage(const char* message) {
  lcd.clear(); // Clear LCD screen
  lcd.setCursor(0, 0); // Set cursor to the first row
  lcd.print(message); // Print message on LCD
}

void readCode(char* codeBuffer) {
  lcd.setCursor(5, 1); // Set cursor to the second row
  lcd.print("[____]"); // Display the input format
  int index = 0; // Index to track code entry position
  while (index < 4) {
    if (receiver.decode()) { // If an IR signal is received
      int digit = -1; // Temporary variable to hold the detected digit
      for (int i = 0; i < 10; i++) {
        if (receiver.decodedIRData.command == NUM_BUTTONS[i]) { // Match IR command with predefined button values
          digit = i;
          break;
        }
      }
      if (digit != -1) {
        codeBuffer[index++] = '0' + digit; // Append the digit to the code buffer
        lcd.setCursor(5 + index, 1); // Move cursor to second row
        lcd.print('*'); // Display masked input
      }
      receiver.resume(); // Resume IR receiver for the next signal
    }
  }
  codeBuffer[4] = '\0'; // Null-terminate the code buffer
}

bool isValidCode(const char* code) {
  if (strlen(code) != 4) return false; // Ensure the code has exactly 4 digits
  for (int i = 0; i < 4; i++) {
    if (code[i] < '0' || code[i] > '9') return false; // Ensure all characters are digits
  }
  return true;
}

void lockAlarm() {
  if (!isLocked) {
    isLocked = true; // Update lock state
    myServo.write(90); // Move servo to lock position
    GreenColour(); // Set RGB LED to Green (armed state)
    displayMessage("Alarm System ON"); // Notify user
    delay(2000); // Wait for user to read message
  }
}

void unlockAlarm() {
  while (true) {
    lcd.clear();
    lcd.print("Enter Password:"); // Prompt user for code
    readCode(enteredCode); // Read entered code

    if (strcmp(enteredCode, currentCode) == 0) { // Compare entered code with stored code
      isLocked = false; // Update lock state
      myServo.write(0); // Move servo to unlock position
      RedColour(); // Set RGB LED to Red (disarmed state)
      displayMessage("Alarm System OFF");
      delay(2000);
      break;
    } else {
      lcd.clear();
      displayMessage("Access Denied!"); // Notify user of incorrect code
      RedColour(); // Set RGB LED to Red (warning state)
      digitalWrite(buzzerPin, HIGH); // Sound buzzer
      delay(1000);
      digitalWrite(buzzerPin, LOW); // Stop buzzer
      lcd.clear();
      showWaitScreen(500);
    }
  }
}

void setNewCode() {
  while (true) {
    displayMessage("Set New Password:");
    char newCode[5];
    readCode(newCode); // Read the new code
    YellowColour();

    if (!isValidCode(newCode)) { // Validate the new code
      RedColour();
      displayMessage("Invalid Password!");
      delay(2000);
      return;
    }

    displayMessage("Confirm Password:");
    char confirmCode[5];
    readCode(confirmCode); // Read the confirmation code

    if (strcmp(newCode, confirmCode) == 0) { // Compare new code and confirmation code
      strcpy(currentCode, newCode); // Save the new code
      EEPROM.put(0, currentCode); // Store the code in EEPROM
      GreenColour();
      showPasswordSetMessage();
      showStatusOffMessage();
      break;
    } else {
    displayMessage("Password Mismatch!"); // Notify user of mismatch
    RedColour();
    }
    delay(2000);
    lcd.clear();
  }  
}

void handleUltrasonicSensor() {
  if (!isLocked) { // Check if the alarm system is OFF
    return; // Exit the function, do nothing
  }

  long duration, distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH); // Measure the time of echo return
  distance = duration * 0.034 / 2; // Calculate distance in cm

  if (distance < 10 && !objectDetected) {
    objectDetected = true;
    objectDetectionStartTime = millis(); // Start timing object detection
    displayMessage("Motion Detected!");
    YellowColour(); // Set RGB LED to Yellow (alert state)
    digitalWrite(buzzerPin, HIGH); // Sound buzzer for alert
    delay(2000); // Alert duration
  }
  if (distance > 10 && objectDetected && millis() - objectDetectionStartTime > objectDetectionDuration) {
    objectDetected = false;
    lcd.clear();
    displayMessage("Monitoring Area");
    delay(2000);
    YellowColour(); // Set RGB LED to Yellow (monitoring state)
    showWaitScreen(100);
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("No intrusion!");
    delay(2000);
    GreenColour(); // Reset RGB LED to Red (armed state)
    lcd.clear();
    showStatusOnMessage();
  }
}

void showStartupMessage() {
  lcd.setCursor(5, 0);
  lcd.print("Welcome!");
  delay(2000);
  lcd.setCursor(0, 2);
  String message = "AlarmSystem v1.0";
  for (byte i = 0; i < message.length(); i++) {
    lcd.print(message[i]);
    delay(100);
  }
  delay(500);
  lcd.clear();
}

void showWaitScreen(int delayMillis) {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("System Loading");
  lcd.setCursor(2, 1);
  lcd.print("[..........]");
  lcd.setCursor(3, 1);
  for (byte i = 0; i < 10; i++) {
    delay(700);
    lcd.print("=");
  }
}

void showPasswordSetMessage() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Password Set!");
  delay(1000);
}

void showStatusOffMessage() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Press POWER");
  lcd.setCursor(0, 1);
  lcd.print("Alarm System OFF");
  delay(1000);
}

void showStatusOnMessage() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Press POWER");
  lcd.setCursor(0, 1);
  lcd.print("Alarm System ON");
  delay(1000);
}

void showStatusMessage() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Press POWER");
  lcd.setCursor(0, 1);
  lcd.print("AlarmSystem Idle");
  delay(1000);
}

void RedColour() {
  analogWrite(redPin, 255);  // Full brightness for red
  analogWrite(greenPin, 0);  // Green off
  analogWrite(bluePin, 0);   // Blue off
}

void GreenColour() {
  analogWrite(redPin, 0);    // Red off
  analogWrite(greenPin, 255); // Full brightness for green
  analogWrite(bluePin, 0);   // Blue off
}

void YellowColour() {
  analogWrite(redPin, 255);
  analogWrite(greenPin, 255);
  analogWrite(bluePin, 0);
}


void setup() {

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  lcd.begin(16, 2); // Initialize LCD dimensions
  receiver.enableIRIn(); // Enable IR receiver
  Serial.begin(9600); // Start serial communication

  myServo.attach(servoPin); // Attach servo to pin

  showStartupMessage();

  YellowColour();

  if (EEPROM.read(0) == 255) { // Check if EEPROM is uninitialized
    displayMessage("Set Password:");
    readCode(currentCode);
    EEPROM.put(0, currentCode); // Save the initial code to EEPROM
  } else {
    EEPROM.get(0, currentCode); // Retrieve the stored code
  }
  showPasswordSetMessage();
  showStatusOffMessage();

  myServo.write(isLocked ? 90 : 0); // Set servo position based on lock state
}

void loop() {
  if (receiver.decode()) { // Check for IR signal
    btnValue = receiver.decodedIRData.command;
    receiver.resume(); // Prepare IR receiver for next signal

    unsigned long currentMillis = millis();
    if (currentMillis - lastButtonPressTime > debounceDelay) {
      lastButtonPressTime = currentMillis;

      if (btnValue == TOGGLE_LOCK_BUTTON) {
        if (isLocked) {
          showWaitScreen(100);
          unlockAlarm();
        } else {
          showWaitScreen(100);
          lockAlarm();
        }
      } else if (btnValue == SET_CODE_BUTTON && !isLocked) {
        setNewCode();
      }
    }
  }

  unsigned long currentMillis = millis();
  if (currentMillis - lastSensorCheckTime > sensorDelay) {
    lastSensorCheckTime = currentMillis;
    handleUltrasonicSensor();
  }
}
