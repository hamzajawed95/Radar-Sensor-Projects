import serial
import time
from pynput.keyboard import Key, Controller

keyboard = Controller()

arduino = serial.Serial("COM16", 9600)
time.sleep = 1 

while 1:
    incoming = str(
        arduino.readline())  
    print(incoming)

    if 'right' in incoming:
        for i in range(1):
            keyboard.press(Key.right)    
            keyboard.release(Key.right)  
    if 'left' in incoming:
        for i in range(1):
            keyboard.press(Key.left)
            keyboard.release(Key.left)
    incoming = " "  
