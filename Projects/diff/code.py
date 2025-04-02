#Testing code

import board
import time
import digitalio

""" 
Configure Input Pins
- Pin 12 High: Neutral
- Pin 13 High: Forward
- Both Low: Drive
"""
pin12 = digitalio.DigitalInOut(board.D12)
pin13 = digitalio.DigitalInOut(board.D13)

pin12.direction = digitalio.Direction.INPUT
pin13.direction = digitalio.Direction.INPUT

pin12.pull = digitalio.Pull.DOWN
pin13.pull = digitalio.Pull.DOWN

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
    if PinA and not PinB:
        return 0
    elif PinA and PinB:
        return 1
    elif PinB and not (PinA or PinD):
        return 2
    elif PinB and PinD:
        return 3
    elif PinD and not PinB
        return 4
    else:
        return None

def move_Motor(last_Gear, taget_Gear, current_Gear, neutral_Pin, lock_Pin, Out1, Out2):
    """ Moves the motor to the position selected by the user

    Args:
        last_Gear (int): State of last_Gear (0,2,4)
        taget_Gear (int): State of target_Gear determined by switch (0,2,4)
        current_Gear (int): Current position of gear (0-4)
        neutral_Pin (bool): State of neutral_Pin
        lock_Pin (bool): State of lock_Pin
        Out1 (bool): Output toggle for voltage to motor
        Out2 (bool): Output toggle for voltage to motor
    """
    target_Gear = get_Switch(neutral_Pin, lock_Pin)

    if taget_Gear == last_Gear:
        Out1 = 0
        Out2 = 0
    else:
        if current_Gear != target_Gear:
            if taget_Gear < current_Gear:
                Out1 = 1
                Out2 = 0
            elif target_Gear > current_Gear:
                Out1 = 1
                Out2 = 0
        else:
            last_Gear = target_Gear

while True:
    time.sleep(0.5)