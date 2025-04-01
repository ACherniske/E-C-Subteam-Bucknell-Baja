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

def get_Switch(Neutral, Drive):

  neutral_Value = Neutral.value
  drive_Value = Drive.value

  # print(f"Pin 12: {neutral_Value}, Pin 13: {drive_Value}") Commented out for Implementation

  if neutral_Value == True and drive_Value == False:
    return "Diff Lock"
  elif neutral_Value == False and drive_Value == True:
    return "Diff Neutral"
  else:
    return "Diff Drive"

while True:
    gear = get_Switch(pin12,pin13)
    print(gear)
    time.sleep(0.5)