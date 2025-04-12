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


// Data structure to hold sensor information
struct SensorData {
  volatile byte half_revolutions;
  volatile unsigned int revolutions;
  unsigned int rpm;
  unsigned long lastTime;
};

// Docstring Outline
/**
 * Summary:
 * @param: 
 * Output (if there is one, doesnt seem to be many)
 */



// Declare SensorData for two sensors
SensorData HESensor1 = {0, 0, 0, 0};
SensorData HESensor2 = {0, 0, 0, 0};

// Function Prototypes
void waitForSDCard();
void waitForButtonPress();
void recordData();
bool isButtonPressed();
void initializeSensor(int interruptPin, void (*isr)());
void updateRPM(SensorData &sensorData);
void printSensorData(const char* label, const SensorData &sensorData);
void HESensor1Detect(); // ISR for axle sensor
void HESensor2Detect();  // ISR for CVT sensor

// Variables
bool isRecording = false;
bool buttonState = false;  // Current Button State
bool lastButtonState = false; // Recall Last Button State


/**
  * SETUP
  */
void setup(){
  
  waitForSDCard(); // Check for SD Card
  
  // Pin Definitions 
  pinMode(22, INPUT_PULLUP); // Button
  pinMode(43, OUTPUT); // SD Activity Light
  digitalWrite(43, LOW);
  initializeSensor(2, HESensor1Detect);
  initializeSensor(3, HESensor2Detect);
  
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
    
    if (!dso32.begin_I2C(0x6B)) { //Accel
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
/**
  * loop (MAIN)
  */
void loop() {
 
  waitForSDCard(); // Check for SD Card
  waitForButtonPress();
  updateRPM(HESensor1); // Update RPM

  if (isRecording) {
    recordData();           // Record data if the system is in recording mode
  }  
  else{
    Serial.println("Waiting");
  }
}

// Functions

//System
bool isButtonPressed() {
  return digitalRead(7) == LOW; // Button pressed when pin reads LOW
}

void waitForSDCard() {
  while (!SD.begin(53)) {
      Serial.println("Waiting for SD card...");
      delay(1000);
      isRecording = false;
      digitalWrite(43, LOW);  // Turn off the LED
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
            digitalWrite(43, HIGH);  // Turn on the LED
            if (dataFile) {
                Serial.println("Recording started!");
                isRecording = true;
                dataFile.println(F("Time,Accel(m/s^2),CVTTemp(F),RPM1")); // Add CSV header
            } else {
                Serial.println("Error opening file!");
            }
        } else {
            Serial.println("Recording stopped!");
            digitalWrite(43, LOW);  // Turn off the LED
            if (dataFile) {
                dataFile.close(); // Close the file
            }
            isRecording = false;
        }
    }
    buttonState = currentButtonState;
}

/**
  * recordData
  *
  */
void recordData() {
      
      DateTime now = rtc.now();
      sensors_event_t accel, gyro, temp; // Accel/Gyro
      dso32.getEvent(&accel, &gyro, &temp); // Accel/Gyro

      float ambientTempF = mlx.readAmbientTempF();
      // float ambientTempC = mlx.readAmbientTempC(); <-- For Celsius collection
      float objectTempF = mlx.readObjectTempF();
      // float objectTempC = mlx.readObjectTempC();

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
          dataFile.println(HESensor1.rpm, 2);  // Hall Effect 1
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
          Serial.println(HESensor1.rpm);
      
          Serial.println("====================================================\n");
          // Delay
          delay(100);
      } else {
          Serial.println("File not open, stopping recording!");
          isRecording = false;
      }
  }

/**
 * 
 * @param: int interruptPin
 *        void *isr (Pointer to location where interupt occured)
 */
void initializeSensor(int interruptPin, void (*isr)()) {
  attachInterrupt(digitalPinToInterrupt(interruptPin), isr, RISING);
}

/**
 * Update the RPM for a given sensor
 * @param: obj SensorDatapassed by reference to sensorData
 */
void updateRPM(SensorData &sensorData) {
  unsigned long currentTime = millis();
  unsigned long timeDiff = currentTime - sensorData.lastTime;

  if (timeDiff > 0) { // Avoid division by zero
    sensorData.rpm = 30 * 1000 / timeDiff * sensorData.half_revolutions;
    sensorData.lastTime = currentTime;
    sensorData.half_revolutions = 0; // Reset for the next calculation
  }
}

/**
 * Interrupt Service Routine for axle sensor
 * 
 */
void HESensor1Detect() {
  HESensor1.half_revolutions++;
  if (HESensor1.half_revolutions % 2 == 0) {
    HESensor1.revolutions++;
  }
}

/**
 * Interrupt Service Routine for CVT sensor
 * 
 */
void HESensor2Detect() {
  HESensor2.half_revolutions++;
  if (HESensor2.half_revolutions % 2 == 0) {
    HESensor2.revolutions++;
  }
}
