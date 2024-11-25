// Include

#include <Wire.h>
#include <SD.h>
  File dataFile;
#include <RTClib.h>
  RTC_DS1307 rtc;
#include <Adafruit_LSM6DSO32.h>
  Adafruit_LSM6DSO32 dso32;
#include <Adafruit_MLX90614.h>
  Adafruit_MLX90614 mlx;

// Function Prototypes
void waitForSDCard();
void waitForButtonPress();
void recordData();
bool isButtonPressed();

// Variables
bool isRecording = false;
bool buttonState = false;  // Current Button State
bool lastButtonState = false; // Recall Last Button State

volatile unsigned long lastPulseTimeCVT = 0;
volatile unsigned long lastPulseTimeWheel = 0;
volatile unsigned long pulseIntervalCVT = 0;
volatile unsigned long pulseIntervalWheel = 0;
const float pulsesPerRevolutionCVT = 1.0;
const float pulsesPerRevolutionWheel = 1.0;

// Setup

void setup(){
  
  waitForSDCard(); // Check for SD Card
  
  // Pin Definitions 
  pinMode(2, INPUT_PULLUP); // CVT Hall Effect
    attachInterrupt(digitalPinToInterrupt(2), handleCVTPulse, FALLING);
  pinMode(3, INPUT_PULLUP); // Wheel Hall Effect
    attachInterrupt(digitalPinToInterrupt(3), handleWheelPulse, FALLING);
  pinMode(22, INPUT_PULLUP); // Button
  pinMode(23, OUTPUT); // SD Activity Light
    digitalWrite(23, LOW);
  
  // Serial Debug
  Serial.begin(9600);

    // Modules
    if (!rtc.begin()) { // RTC
    Serial.println("Error connecting to RTC Module. Check wiring.");
    while (1);
    }
    if (!rtc.isrunning()) {
    Serial.println("RTC is not running! Setting the RTC time...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Set RTC to compile date and time
    }
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Uncomment to manually set RTC Module
    
    if (!SD.begin(53)) { // SD
    Serial.println("Error connecting to SD Module. Check wiring and Insert SD Card.");
    return;
    }

    //Sensors
    if (!mlx.begin()) { //Temp
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1);
    }
    
    if (!dso32.begin_I2C()) { //Accel
    Serial.println("Error connecting to DSO32 sensor. Check wiring.");
    while (1) {
      delay(10);
      }
    }
    dso32.setAccelRange(LSM6DSO32_ACCEL_RANGE_16_G); // Configure Accel
    dso32.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
    dso32.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
    dso32.setGyroDataRate(LSM6DS_RATE_12_5_HZ);

    // Read Data Test
    DateTime now = rtc.now();
    
    float ambientTempF = mlx.readAmbientTempF();
    float ambientTempC = mlx.readAmbientTempC();
    float objectTempF = mlx.readObjectTempF();
    float objectTempC = mlx.readObjectTempC();

    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    dso32.getEvent(&accel, &gyro, &temp);

    // Print to Serial
    Serial.println("====================================================");
    Serial.println("© 2024 Aiden Cherniske");
    Serial.println("https://github.com/ACherniske ");
    Serial.println("https://www.linkedin.com/in/aiden-cherniske/ ");
    
    // This code is provided under the MIT License. Permission is hereby granted, 
    // free of charge, to any person obtaining a copy of this software and 
    // associated documentation files (the "Software"), to deal in the Software 
    // without restriction, including without limitation the rights to use, copy, 
    // modify, merge, publish, distribute, sublicense, and/or sell copies of the 
    // Software, and to permit persons to whom the Software is furnished to do so, 
    // subject to the following conditions:
    //
    // The above copyright notice and this permission notice shall be included in 
    // all copies or substantial portions of the Software.
    //
    // THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
    // IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
    // FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
    // THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
    // LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    // FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    // DEALINGS IN THE SOFTWARE.
    
    Serial.println("----------------------------------------------------");
    Serial.print("Date: ");
    Serial.print(now.year());
    Serial.print("-");
    Serial.print(now.month());
    Serial.print("-");
    Serial.print(now.day());
    Serial.print("  Time: ");
    Serial.print(now.hour());
    Serial.print(":");
    Serial.print(now.minute());
    Serial.print(":");
    Serial.println(now.second());

    Serial.println("----------------------------------------------------");
    Serial.println("MLX90614 Temperature Sensor:");
    Serial.print("  Ambient Temp: ");
    Serial.print(ambientTempF);
    Serial.println(" °F");
    Serial.print("  Object Temp:  ");
    Serial.print(objectTempF);
    Serial.println(" °F");

    Serial.println("----------------------------------------------------");
    Serial.println("LSM6DSO32 Accelerometer:");
    Serial.print("  X: ");
    Serial.print(accel.acceleration.x);
    Serial.println(" m/s^2");
    Serial.print("  Y: ");
    Serial.print(accel.acceleration.y);
    Serial.println(" m/s^2");
    Serial.print("  Z: ");
    Serial.print(accel.acceleration.z);
    Serial.println(" m/s^2");

    Serial.println("====================================================\n");
}

// Loop

void loop() {
 
  waitForSDCard(); // Check for SD Card
  waitForButtonPress();

  if (isRecording) {
    recordData();           // Record data if the system is in recording mode
  }

  Serial.println(isRecording);
  
}

// Functions

//System
  bool isButtonPressed() {
    return digitalRead(22) == LOW; // Button pressed when pin reads LOW
  }
  void waitForSDCard() {
    while (!SD.begin(53)) {
        Serial.println("Waiting for SD card...");
        delay(1000);
        isRecording = false;
        digitalWrite(23, LOW);  // Turn off the LED
    }
  }
  void waitForButtonPress() {
      static bool buttonState = false;
      bool currentButtonState = isButtonPressed();
  
      // Check if the button has been toggled
      if (currentButtonState && !buttonState) { // Button press detected
          if (!isRecording) {
              DateTime now = rtc.now(); // Get current time
                char filename[20];        // Buffer for filename
                snprintf(filename, sizeof(filename), "%02d%02d%02d%02d.csv",
                now.month(), now.day(), now.hour(), now.minute());
              dataFile = SD.open(filename, FILE_WRITE); // Open the file for writing
              digitalWrite(23, HIGH);  // Turn on the LED
              if (dataFile) {
                  Serial.println("Recording started!");
                  isRecording = true;
                  dataFile.println(F("Time,Accel(m/s^2),CVTTemp(F),CVTRPM")); // Add CSV header
              } else {
                  Serial.println("Error opening file!");
              }
          } else {
              Serial.println("Recording stopped!");
              digitalWrite(23, LOW);  // Turn off the LED
              if (dataFile) {
                  dataFile.close(); // Close the file
              }
              isRecording = false;
          }
      }
      buttonState = currentButtonState;
  }
  void recordData() {
    static unsigned long lastRecordTime = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastRecordTime >= 1000) { // Log data every second
        lastRecordTime = currentTime;
    
        noInterrupts(); // Temporarily disable interrupts to safely access shared variables
        unsigned long intervalCVT = pulseIntervalCVT;
        unsigned long intervalWheel = pulseIntervalWheel;
        interrupts(); // Re-enable interrupts
        float rpmCVT = calculateRPM(intervalCVT, pulsesPerRevolutionCVT);
        float rpmWheel = calculateRPM(intervalWheel, pulsesPerRevolutionWheel);
        
        DateTime now = rtc.now();
        sensors_event_t accel, gyro, temp; // Accel/Gyro
        dso32.getEvent(&accel, &gyro, &temp); // Accel/Gyro

        float ambientTempF = mlx.readAmbientTempF();
        float ambientTempC = mlx.readAmbientTempC();
        float objectTempF = mlx.readObjectTempF();
        float objectTempC = mlx.readObjectTempC();

        if (dataFile) {
            char currentTime[10];
            snprintf(currentTime,sizeof(currentTime), "%02d:%02d:%02d",
            now.hour(),now.minute(),now.second());
            dataFile.print(currentTime);
            dataFile.print(",");
            dataFile.print(accel.acceleration.x, 2);  // Acceleration X
            dataFile.print(",");
            dataFile.print(ambientTempF, 2);  // Ambient Temperature
            dataFile.print(",");
            dataFile.println(rpmCVT, 2);  // CVT RPM
            dataFile.flush(); // Ensure data is written to the SD card

            //debug
            Serial.println("====================================================");
            Serial.print("Date: ");
            Serial.print(now.year());
            Serial.print("-");
            Serial.print(now.month());
            Serial.print("-");
            Serial.print(now.day());
            Serial.print("  Time: ");
            Serial.print(now.hour());
            Serial.print(":");
            Serial.print(now.minute());
            Serial.print(":");
            Serial.println(now.second());
        
            Serial.println("----------------------------------------------------");
            Serial.println("MLX90614 Temperature Sensor:");
            Serial.print("  Ambient Temp: ");
            Serial.print(ambientTempF);
            Serial.println(" °F");
            Serial.print("  Object Temp:  ");
            Serial.print(objectTempF);
            Serial.println(" °F");
        
            Serial.println("----------------------------------------------------");
            Serial.println("LSM6DSO32 Accelerometer:");
            Serial.print("  X: ");
            Serial.print(accel.acceleration.x);
            Serial.println(" m/s^2");
            Serial.print("  Y: ");
            Serial.print(accel.acceleration.y);
            Serial.println(" m/s^2");
            Serial.print("  Z: ");
            Serial.print(accel.acceleration.z);
            Serial.println(" m/s^2");

            Serial.println("----------------------------------------------------");
            Serial.println("CVT Hall Effect Sensor:");
            Serial.print("  RPM: ");
            Serial.println(rpmCVT);
        
            Serial.println("====================================================\n");
        } else {
            Serial.println("File not open, stopping recording!");
            isRecording = false;
        }
    }
}

// Hall Effect Sensors
  float calculateRPM(unsigned long interval, float pulsesPerRevolution) {
      if (interval == 0) {
          return 0; // Avoid division by zero; sensor not triggered yet
      }
      float frequency = 1000000.0 / interval; // Frequency in Hz (microseconds to seconds)
      return (frequency / pulsesPerRevolution) * 60.0; // Convert to RPM
  }
  // Interrupt Service Routine for CVT pulse
  void handleCVTPulse() {
      unsigned long currentTime = micros();
      pulseIntervalCVT = currentTime - lastPulseTimeCVT;
      lastPulseTimeCVT = currentTime;
  }
  // Interrupt Service Routine for Wheel pulse
  void handleWheelPulse() {
      unsigned long currentTime = micros();
      pulseIntervalWheel = currentTime - lastPulseTimeWheel;
      lastPulseTimeWheel = currentTime;
  }
