# Code Description and Functionality

This Arduino code is designed to collect and record sensor data to an SD card. It utilizes several external sensors and modules:

* **Real-Time Clock (RTC) DS1307:** To provide accurate timestamps for the recorded data.
* **SD Card Module:** To store the collected sensor data in CSV format.
* **LSM6DSO32 Inertial Measurement Unit (IMU):** To measure acceleration and potentially gyroscope data (though only acceleration is currently recorded).
* **MLX90614 Infrared Thermometer:** To measure ambient and object temperatures.
* **Two Hall Effect Sensors:** Configured to detect rotations and calculate Revolutions Per Minute (RPM) for two different systems (likely an axle and a Continuously Variable Transmission - CVT).
* **A Push Button:** To start and stop the data recording process.
* **An LED:** To indicate the recording status.

## Functionality Breakdown:

1.  **Initialization (`setup()`):**
    * **SD Card Check:** The code first waits for the SD card to be properly initialized and inserted.
    * **Pin Configuration:** Configures the button pin as an input with a pull-up resistor and the LED pin as an output, initially turned off.
    * **Sensor Initialization:** Initializes the two Hall Effect sensors by attaching interrupt service routines (ISRs) to specific interrupt pins. These ISRs (`HESensor1Detect` and `HESensor2Detect`) increment counters whenever a rising edge is detected from the sensors.
    * **Serial Communication:** Begins serial communication for debugging and providing feedback.
    * **Module Initialization:** Initializes the RTC, SD card, temperature sensor (MLX90614), and IMU (LSM6DSO32). Error handling is included for each module to check for successful connection. The IMU's acceleration and gyroscope ranges, as well as data rates, are configured.
    * **Initial Data Read and Print:** Reads a sample of data from the RTC, temperature sensor, and IMU, and prints it to the serial monitor for verification. This includes the current date and time, ambient and object temperatures, and acceleration values. It also prints copyright and licensing information.

2.  **Main Loop (`loop()`):**
    * **SD Card Check:** Continuously checks if the SD card is still initialized. If not, it waits and sets the recording status to false.
    * **Button Press Detection:** Calls the `waitForButtonPress()` function to handle toggling the recording state based on button presses.
    * **RPM Update:** Calls the `updateRPM()` function for `HESensor1` to calculate and update its RPM value based on the pulses detected by its interrupt routine.
    * **Recording Logic:**
        * If `isRecording` is true, it calls the `recordData()` function to collect sensor data and write it to the SD card.
        * If `isRecording` is false, it prints "Waiting" to the serial monitor.

3.  **Functions:**
    * **`waitForSDCard()`:** Continuously checks if the SD card is initialized. If not, it prints a message to the serial monitor and keeps the LED off, preventing recording.
    * **`waitForButtonPress()`:** Detects button presses using a simple debouncing mechanism (by comparing the current and previous button states). On each press (transition from unpressed to pressed):
        * If not currently recording, it gets the current time from the RTC, generates a unique filename based on the date and time, opens a new CSV file on the SD card, turns on the recording LED, sets `isRecording` to true, and writes a header row to the CSV file.
        * If currently recording, it prints a "Recording stopped!" message, turns off the LED, closes the currently open SD card file, and sets `isRecording` to false.
    * **`recordData()`:** This function is called when `isRecording` is true. It:
        * Gets the current time from the RTC.
        * Reads acceleration data from the LSM6DSO32.
        * Reads ambient and object temperatures from the MLX90614.
        * If the data file is open, it formats the current time, acceleration X-axis value, ambient temperature (in Fahrenheit), and the RPM of `HESensor1` into a CSV row and writes it to the SD card. It also prints this data to the serial monitor for debugging.
        * Includes a small delay (100ms) before the next data point is recorded.
        * If the data file is not open, it prints an error message and stops recording.
    * **`initializeSensor(int interruptPin, void (*isr)())`:** A utility function to attach an interrupt service routine (`isr`) to a specific digital pin (`interruptPin`) to trigger on a rising edge.
    * **`updateRPM(SensorData &sensorData)`:** Calculates the RPM based on the number of half-revolutions detected by the Hall Effect sensor within a certain time period. It uses the formula: $\text{RPM} = \frac{30 \times 1000}{\text{time difference (ms)}} \times \text{half\_revolutions}$. After calculation, it resets the `half_revolutions` counter and updates the `lastTime`.
    * **`HESensor1Detect()` and `HESensor2Detect()`:** These are Interrupt Service Routines (ISRs) that are called automatically whenever a rising edge is detected on the interrupt pins connected to the respective Hall Effect sensors. They increment the `half_revolutions` counter for the corresponding `SensorData` structure. They also increment a `revolutions` counter every two half-revolutions.

4.  **Data Structures and Variables:**
    * **`SensorData` struct:** Holds the data related to each Hall Effect sensor, including the count of half-revolutions, total revolutions, calculated RPM, and the last time a pulse was detected.
    * **`HESensor1` and `HESensor2`:** Instances of the `SensorData` struct for the two Hall Effect sensors.
    * **`isRecording`:** A boolean flag indicating whether data recording is currently active.
    * **`buttonState` and `lastButtonState`:** Variables used for debouncing the button press detection.
    * **`dataFile`:** A `File` object used to interact with the SD card file.