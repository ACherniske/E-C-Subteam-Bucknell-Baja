# Code Description and Functionality

This Python code, designed for a microcontroller platform like the Adafruit Feather M0, controls the movement of a motor to position a differential gear into one of three states: Neutral, Drive, or Lock. It reads the desired state from a DPDT switch and monitors the current gear position using three digital input pins.

## Functionality Breakdown:

1.  **Pin Initialization:**
    * It imports necessary libraries: `board` for accessing microcontroller pins, `time` for introducing delays, and `digitalio` for controlling digital input/output.
    * It initializes several `DigitalInOut` objects, assigning them to specific pins on the microcontroller:
        * `pin12` (Neutral Pin) and `pin13` (Lock Pin) are configured as inputs with pull-down resistors. These pins are intended to be connected to a DPDT switch that determines the target gear state.
        * `pinA` (D11), `pinB` (D10), and `pinD` (D9) are also configured as inputs with pull-down resistors. These pins read the current position of the differential gear.
        * `out1` (Voltage Output 1 on D6) and `out2` (Voltage Output 2 on D5) are configured as outputs. These pins control the direction of the motor by supplying voltage.

2.  **Variables:**
    * `target_Gear`: Initialized to `None`, this variable will store the desired gear state (0 for Neutral, 2 for Drive, 4 for Lock) as determined by the DPDT switch.
    * `curr_Gear`: Initialized to `None`, this variable will store the current gear position (0-4) as read from the three input pins (`pinA`, `pinB`, `pinD`).

3.  **`get_Switch(neutral_Pin, lock_Pin)` Function:**
    * This function reads the states of the `neutral_Pin` (pin 12) and `lock_Pin` (pin 13).
    * It interprets the combination of these pin values to determine the desired gear state:
        * If `neutral_Pin` is HIGH (True) and `lock_Pin` is LOW (False), it returns `4` (Diff Lock).
        * If `neutral_Pin` is LOW (False) and `lock_Pin` is HIGH (True), it returns `0` (Diff Neutral).
        * In any other case (likely both LOW or an unexpected state), it returns `2` (Diff Drive).

4.  **`current_Gear(PinA, PinB, PinD)` Function:**
    * This function reads the states of the three input pins (`PinA`, `PinB`, `PinD`).
    * It uses a series of `if/elif` conditions to map the combinations of these pin states to a numerical representation of the current gear position (0 to 4):
        * `PinA` HIGH and `PinB` LOW: returns `0` (Neutral).
        * `PinA` HIGH and `PinB` HIGH: returns `1` (Between Neutral and Drive).
        * `PinB` HIGH, `PinA` LOW, and `PinD` LOW: returns `2` (Drive).
        * `PinB` HIGH and `PinD` HIGH: returns `3` (Between Drive and Lock).
        * `PinD` HIGH and `PinB` LOW: returns `4` (Lock).
        * If none of the above conditions are met, it returns `None`, indicating an undefined or intermediate gear state.

5.  **`move_Motor(taget_Gear, curr_Gear, PinA, PinB, PinD, neutral_Pin, lock_Pin, Out1, Out2)` Function:**
    * This function takes the target gear, current gear, the input pins for the current gear, the input pins for the target gear switch, and the two output pins for the motor as arguments.
    * It first updates the `target_Gear` and `curr_Gear` by calling `get_Switch()` and `current_Gear()` respectively.
    * It then compares the `curr_Gear` with the `target_Gear`:
        * If they are equal, it sets both output pins (`out1` and `out2`) to LOW (False), likely stopping the motor.
        * If they are different and `curr_Gear` is not `None`:
            * If `target_Gear` is less than `curr_Gear`, it sets `out1` HIGH (True) and `out2` LOW (False), likely driving the motor in one direction.
            * If `target_Gear` is greater than `curr_Gear`, it sets `out1` LOW (False) and `out2` HIGH (True), likely driving the motor in the opposite direction.
        * If `curr_Gear` is `None` (undefined), it defaults to setting `out1` HIGH and `out2` LOW, possibly as an initial movement attempt.
    * Finally, it returns the updated `target_Gear`.

6.  **Main Loop (`while True:`):**
    * This loop runs continuously.
    * In each iteration:
        * It reads the state of the DPDT switch using `get_Switch()` and updates `target_Gear`.
        * It reads the current gear position using `current_Gear()` and updates `curr_Gear`.
        * It calls `move_Motor()` to adjust the motor outputs (`out1` and `out2`) based on the `target_Gear` and `curr_Gear`.
        * It prints the current `target_Gear`, `curr_Gear`, and the states of the output pins to the serial console for monitoring and debugging.
        * It introduces a small delay of 0.1 seconds using `time.sleep(0.1)`.

In essence, this code implements a closed-loop control system for positioning a differential gear. It reads a desired position from a switch, senses the current position using digital inputs, and controls a motor to move the gear towards the desired position. The system continuously monitors and adjusts the motor until the current gear position matches the target gear selected by the user.