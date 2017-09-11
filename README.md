# sawboard
Saw your PS2 keyboard and remove the QWERTY part. Reprogram the keypad with an Arduino Pro Micro

Why do keyboard have so many keys that are of no use at all? Why do F6, F8, F9, F10, F11, F12, Scroll Lock and Pause Break exist? Why are there two backslash/vertical bar keys on so many keyboards (check yours: there may be one backslash near the Enter key and one near the left Alt key!). Why do we have two Windows Starte Keys and two Ctrl keys (and two Shift, two Alt, two / two *, two -, two +, two ., two Enters?) Why?

And why are there NO KEYS for some of the most often used functions such as COPY, CUT, PASTE, ALL, PRINT, CLOSE, SAVE, UNDO, REDO? Let's put an end to this nonsense!

I used a saw, knife and scissors to remove the middle part of an old ps2 keyboard. Then I glued the right end (including the keypad) and the left end of the keyboard together to create a self contained keypad. Replaced the keycaps. I Added in an Arduino Pro Micro which serves as a usb interface and there I created my own custom programmable keypad!

Step by step instructions.

Required
An old PS2 (DIN or mini DIN) keyboard
An arduino micro
1m flexible isolated wire (23AWG or 0.25mm2)
solder
soldering iron
pc with Arduino software

Remove the screws and take the PS2 keyboard apart. You get a top part, a bottom part, a whole bunch of keycaps, a stack of foils, a pcb, a cable with ps2 connector.The latter three are connected together. Take the saw and cut the top and bottom parts between the arrows keypad and the numeric keypad. Repeat this left from the QWERTY part. Remove the middle part and make sure that the left and right parts fit together. Glue the parts together.
Using scissors, cut the unwanted part of the foils. Take care that the pcb remains connected to the foils.
If the pcb does not fit in the case any more, find a suitable place to cut the pcb in two (where there are not too many pcb traces). Connect the two pcb halves with wires (10cm).

Remove the cable with the PS2 plug and make connections between the Arduino Pro Micro and where the cable used to be as follows:
*  DIN-connector PINs (AT keyboard) | MINI-DIN (PS2) | Arduino pin | function
*  1  |  5  | D9 | clock
*  2  |  1  | D8 | data
*  3  | 2,6 |    | n.c.
*  4  |  3  |    | gnd
*  5  |  4  |VCC | vcc (+5V)
*     |     |D10| debug-line
  
Thanks to http://www.computer-engineering.org/ps2protocol/
 
Put the keypad together. Place the following keycaps in stead of the original ones. Each key functions as its CTRL-version, so for example the X, C and V keys on the bottom row will generate CTRL/X, CTRL/C and CTRL/V.
 
Keypad layout
 
* F1 F2 F3 F4
* Y  U  P  +
* F  A  S  +
* X  C  V  
* 0  0  .  Enter
 
Connect the USB cable, load the arduino sketch Controlboard.ino into the Arduino Pro Micro and you are ready to go!
 
