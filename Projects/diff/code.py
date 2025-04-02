#Testing code

import board
import time
import digitalio

pin12 = digitalio.DigitalInOut(board.D12) # Neutral Pin
pin13 = digitalio.DigitalInOut(board.D13) # Lock Pin

# Configure Pins as Inputs
pin12.direction = digitalio.Direction.INPUT
pin13.direction = digitalio.Direction.INPUT

# Configure Pins as Pull Down
pin12.pull = digitalio.Pull.DOWN
pin13.pull = digitalio.Pull.DOWN

pinA = digitalio.DigitalInOut(board.D5)
pinB = digitalio.DigitalInOut(board.D6)
pinD = digitalio.DigitalInOut(board.D7)

# Configure Pins as Inputs
pinA.direction = digitalio.Direction.INPUT
pinB.direction = digitalio.Direction.INPUT
pinD.direction = digitalio.Direction.INPUT

# Configure Pins as Pull Down
pinA.pull = digitalio.Pull.DOWN
pinB.pull = digitalio.Pull.DOWN
pinD.pull = digitalio.Pull.DOWN

out1 = digitalio.DigitalInOut(board.D8) # Voltage Output 1
out2 = digitalio.DigitalInOut(board.D9) # Voltage Output 2

# Configure Pins as Outputs
out1.direction = digitalio.Direction.OUTPUT
out2.direction = digitalio.Direction.OUTPUT

last_Gear = None
target_Gear = None
current_Gear = None

def get_Switch(neutral_Pin, lock_Pin):
    """ Gets the position of the DPDT switch connected to the Feather M0

    Args:
        neutral_Pin (bool): State of neutral_Pin
        lock_Pin (bool): State of lock_Pin

    Returns:
        int: returns the state of the switch as an integer
            0: Neutral
            2: Drive
            4: Lock
    """
  neutral_Value = neutral_Pin.value
  lock_Value = lock_Pin.value

  # print(f"Pin 12: {neutral_Value}, Pin 13: {lock_Value}") Commented out for Implementation

  if neutral_Value == True and lock_Value == False:
    return 4 # Diff Lock
  elif neutral_Value == False and lock_Value == True:
    return 0 # Diff Neutral
  else:
    return 2 # Diff Drive

def current_Gear(PinA, PinB, PinD):
    """ Determines the current gear of the diff based on the state of 3 input pins

    Args:
        PinA (bool): State of Pin A
        PinB (bool): State of Pin B
        PinD (bool): State of Pin C

    Returns:
        int: (0-4) which represents the gear the diff is currently in
            0: Neutral
            1: Between Neutral and Drive
            2: Drive
            3: Between Drive and Lock
            4: Lock
    """
    if PinA.value and not PinB.value:
        return 0
    elif PinA.value and PinB.value:
        return 1
    elif PinB.value and not (PinA.value or PinD.value):
        return 2
    elif PinB.value and PinD.value:
        return 3
    elif PinD.value and not PinB.value:
        return 4
    else:
        return None

def move_Motor(last_Gear, taget_Gear, current_Gear,PinA, PinB, PinD, neutral_Pin, lock_Pin, Out1, Out2):
    """ Moves the motor to the position selected by the user

    Args:
        last_Gear (int): State of last_Gear (0,2,4)
        taget_Gear (int): State of target_Gear determined by switch (0,2,4)
        current_Gear (int): Current position of gear (0-4)
        PinA (bool): State of Pin A for current_Gear
        PinB (bool): State of Pin B for current_Gear
        PinD (bool): State of Pin C for current_Gear
        neutral_Pin (bool): State of neutral_Pin for target_Gear
        lock_Pin (bool): State of lock_Pin for target_Gear
        Out1 (bool): Output toggle for voltage to motor
        Out2 (bool): Output toggle for voltage to motor
    """
    target_Gear = get_Switch(neutral_Pin, lock_Pin)
    current_Gear = current_Gear(PinA, PinB, PinD)

    if taget_Gear == last_Gear:
        Out1 = 0
        Out2 = 0
    else:
        if current_Gear != target_Gear and current_Gear is not None:
            if taget_Gear < current_Gear:
                Out1.value = True
                Out2.value = False
            elif target_Gear > current_Gear:
                Out1.value = False
                Out2.value = True
        else:
            last_Gear = target_Gear
    return last_Gear, target_Gear

while True:
    target_Gear = get_Switch(pin12, pin13)
    current_Gear = current_Gear(pinA, pinB, pinD)

    if last_Gear is None:
        last_Gear = target_Gear

    last_Gear, target_Gear = move_Motor(last_Gear, target_Gear, current_Gear, pinA, pinB, pinD, pin12, pin13, out1, out2)

    print(f"Last Gear: {last_Gear}, Target Gear: {target_Gear}, Current Gear: {current_Gear}")
    time.sleep(0.5)