# LED-Serial-Test
NUCELO-L053R8 serial test program for PWM-driven LEDs

Written for UVic Rocketry

---

# Setup

Flash the .bin to the Nucelo using ST-Link.

Send commands via serial port terminal. Enter “help” for a list of commands. Enter “status” for the current state of all PWM pins.

Serial port parameters: 
- Baud rate 9600.
- 8 bit data
- No parity
- No flow control

---

# List of commands:

on # -> Turns LED # on (e.x. on 1)
off # -> Turns LED # off (e.x. off 2)
allon -> Turns all LEDs on
alloff -> Turns all LEDs off

duty # % -> Sets LED # duty cycle to % (0 to 100) (e.x. duty 1 50)
allduty % -> Sets all LED duty cycles to % (0 to 100) (e.x. dutyall 25)

status -> Prints LED status
help -> Prints help menu

kill -> Turns off all LEDs and ends the program

Notes:
- All command parameters must be entered as integers (no decimals)
- LEDs are numbered 1 to (max # leds) 
- Duty percentages are numbered 0 to 100 
- Command lines with extra words/characters are invalid

---

# License

GNU LGPL 3.0