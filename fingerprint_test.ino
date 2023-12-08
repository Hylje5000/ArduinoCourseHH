#include <Servo.h>
#include <Adafruit_Fingerprint.h>

Servo servo1; // Declare a servo object
int pos = 0; // Initialize a variable to store servo position
int servoPin = 8; // Define the pin for the servo

bool isLocked = true; // Variable to track the lock state

// Function to open the lock by rotating the servo
void openLock() {
  for (pos = 0; pos <= 90; pos += 1) {
    servo1.write(pos); // Rotate the servo to the open position
  }
  isLocked = false; // Update lock state to open
}

// Function to close the lock by rotating the servo
void closeLock() {
  for (pos = 90; pos >= 0; pos -= 1) {
    servo1.write(pos); // Rotate the servo to the closed position
  }
  isLocked = true; // Update lock state to closed
}

volatile int finger_status = -1; // Variable to store fingerprint status

SoftwareSerial mySerial(2, 3); // Define software serial pins for the fingerprint sensor

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial); // Create a fingerprint sensor object

void setup() {
  servo1.attach(servoPin); // Attach the servo to the defined pin
  Serial.begin(57600); // Initialize serial communication
  while (!Serial); // Wait for serial connection to be established
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  finger.begin(57600); // Initialize the fingerprint sensor
  
  if (finger.verifyPassword()) { // Check if the fingerprint sensor is detected
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); } // Stay in an infinite loop if sensor not found
  }

  finger.getTemplateCount(); // Retrieve the number of stored fingerprints
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial.println("Waiting for valid finger...");
}

void loop() {
  finger_status = getFingerprintIDez(); // Check for a fingerprint match
  if (finger_status > 0) {
    Serial.print("Match");
    if (isLocked) {
      openLock(); // Open the lock if it's currently locked
    } else {
      closeLock(); // Close the lock if it's currently open
    }
  } else if (finger_status == -2) {
    for (int ii = 0; ii < 5; ii++) {
      Serial.print("Not Match");
    }
  }
  delay(50); // Delay for stability, doesn't need to run at full speed
}

// Function to get fingerprint ID; returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage(); // Capture a fingerprint image
  if (p != FINGERPRINT_OK) {
    Serial.println(p); // Print error message if image capture fails
    return -1;
  }
  
  p = finger.image2Tz(); // Convert the captured image to a template
  if (p != FINGERPRINT_OK) {
    Serial.println(p); // Print error message if conversion fails
    return -1;
  }

  p = finger.fingerFastSearch(); // Search for a matching fingerprint in the database
  if (p != FINGERPRINT_OK) {
    return -2; // Return -2 if no match found
  }
  
  // Found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; // Return the matched fingerprint ID
}
