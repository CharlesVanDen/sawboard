/* Connect a PS2 or AT keyboard via an Arduino (pro) micro (ATMEGA32U4) to your pc and have an extra,
 *  hardware programmable keyboard
 *  DIN-connector PINs (AT keyboard) | MINI-DIN (PS2) | Arduino pin | function
 *  1  |  5  | D9 | clock
 *  2  |  1  | D8 | data
 *  3  | 2,6 |    | n.c.
 *  4  |  3  |    | gnd
 *  5  |  4  |    | vcc (+5V)
 *     |     |D10| debug-line
 *  
 *  Thanks to http://www.computer-engineering.org/ps2protocol/
 */
 
#include <avr/pgmspace.h>
#include <HID-Project.h>
//#include <HID-Settings.h>

const int pinLed   = 17;
const int pinClock =  9;
const int pinData  =  8;
const int pinDebug = 10;
const int LED_scroll_lock = 1;
const int LED_num_lock    = 2;
const int LED_caps_lock   = 4;

//                                    kp0   kp1   kp2   kp3   kp4   kp5   kp6   kp7   kp8   kp9  NUML  STAR   MIN  PLUS   DOT SLASH, ENTER
const uint8_t keymapA[] PROGMEM = {  0x70, 0x69, 0x72, 0x7a, 0x6b, 0x73, 0x74, 0x6c, 0x75, 0x7d, 0x77, 0x7c, 0x7b, 0x79, 0x71, 0};
//const uint8_t ctlmapA[] PROGMEM = { '0',  'x',  'c',  'v',  'f',   'a',  's',  'y',  'u',  'p',  '1',  '3',  '4',  '+',  '.',  '/',  10 };

uint8_t nibble_to_hex(uint16_t nibble);
uint16_t receive_from_keyboard();
void send_to_keyboard(uint8_t leds);

/*
uint8_t shift_pressed=0;
uint8_t ctrl_pressed=0;
uint8_t caps_pressed=0;
uint8_t numlock_pressed=0;
uint8_t lalt_pressed=0;
uint8_t ralt_pressed=0;
*/

uint8_t size_keymapA=0;
uint8_t leds=1;

void setup()
{
  //initialize the pins connected to the keyboard
  pinMode(pinClock, INPUT_PULLUP);
  pinMode(pinData, INPUT_PULLUP);
  pinMode(pinDebug, OUTPUT);

  // Sends a clean report to the host. This is important on any Arduino type.
  Consumer.begin(); 
  Keyboard.begin();
  //determine size of keymapA
  while (pgm_read_byte_near(keymapA + size_keymapA)) {
    size_keymapA++;
  }
}

void loop() {
  uint8_t ch[5];
  uint8_t num_ch=1;
  uint16_t buf=receive_from_keyboard();
  ch[0]=0xff&(buf>>4);
  if(ch[0]==0xE0||ch[0]==0xE1||ch[0]==0xf0){//expect a second byte
    num_ch++;
    buf=receive_from_keyboard();
    ch[1]=0xff&(buf>>4);
    if(ch[1]==0xf0||(ch[0]==0xE0&&ch[1]==0x12)||(ch[0]==0xE1&&ch[1]==0x14)){//expect a third byte  
      num_ch++;
      buf=receive_from_keyboard();
      ch[2]=0xff&(buf>>4);
      if(ch[0]==0xE0 && ch[1]==0x12 && ch[2]==0xe0){//expect a fourth byte  
        num_ch++;
        buf=receive_from_keyboard();
        
        ch[3]=0xff&(buf>>4);
      }
    }
  }
/*
  if(ch[0]==0x12||ch[0]==0x59){//shift depressed
    shift_pressed=1;
    //Keyboard.print("=SHIFT");
  }

  if((ch[0]==0xf0)&&(ch[1]==0x12||ch[1]==0x59)){//shift released
    shift_pressed=0;
    //Keyboard.print("=UNSHIFT");
  }

  if(ch[0]==0x14||(ch[0]==0xe0&&ch[1]==0x14)){//ctrl depressed
    ctrl_pressed=1;
  }

  if(ch[0]==0x58){//caps pressed
    caps_pressed=!caps_pressed;
    send_to_keyboard(0xed);//the set LEDs command
    send_to_keyboard((caps_pressed*LED_caps_lock)|(numlock_pressed*LED_num_lock));
    //if(caps_pressed) Keyboard.print("=CAPS");
    //else Keyboard.print("=UNCAPS");
  }

  if(ch[0]==0x77){//numlock pressed
    numlock_pressed=!numlock_pressed;
    send_to_keyboard(0xed);//the set LEDs command
    send_to_keyboard((caps_pressed*LED_caps_lock)|(numlock_pressed*LED_num_lock));
  }

  if(ch[0]==0x11){//left alt
    lalt_pressed=1;
  }

  if(ch[0]==0xf0&&ch[1]==0x11){//left alt release
    lalt_pressed=0;
  }

  if(ch[0]==0xe0){
    if((ch[1]==0xf0&&ch[2]==0x14)||(ch[0]==0xf0&&ch[1]==0x14)){//ctrl released
      ctrl_pressed=0;
    }
  
    if(ch[1]==0x11){//right alt
      ralt_pressed=1;
    }
  
    if(ch[1]==0xf0&&ch[2]==0x11){//right alt release
      ralt_pressed=0;
    }
  }

  Keyboard.print("[");
  if(!shift_pressed) Keyboard.print("no ");
  Keyboard.print("shift;");
  if(!ctrl_pressed) Keyboard.print("no ");
  Keyboard.print("ctrl;");
  if(!caps_pressed) Keyboard.print("no ");
  Keyboard.print("caps;");
  Keyboard.print("]");
  */

  uint8_t index=0;
  uint8_t code;
  if(num_ch==1){//decode the keypress using keymapA
    do{
      code=pgm_read_byte_near(keymapA + index);
      index++;
    } while(code!=0 && code!=ch[0]);
    if(code!=0){
      index--;
      //uint8_t ascy=pgm_read_byte_near(ctlmapA + index);
      //if((shift_pressed == caps_pressed)&&(ascy>='A')&&(ascy<='Z')) {ascy+='a'-'A';}
      //Keyboard.write(ascy);
    }
  }
  
  if(ch[0]==0xe0){
    if(num_ch==4){ch[1]=ch[3];}//4 byte codes like E0,12,E0,70 are equivalent to E0,70 (INS)
    if(ch[1]==0x4a){code=index=size_keymapA;}   //KP/
    else
    if(ch[1]==0x5a){code=index=size_keymapA+1;} //KPENTER
  }

  if(code==0){
    if (ch[0]==0xf0 || ch[1]==0xf0){
      //Consumer.releaseAll();
      Keyboard.releaseAll();
    }else if(ch[0]==0xfa){
      //Ack received from the keyboard in response to something I sent
      //ignore it
    }else{//unknown key pressed, log it.
      Keyboard.write('<');
      Keyboard.write(nibble_to_hex(ch[0]>>4));
      Keyboard.write(nibble_to_hex(ch[0]));
      for(uint8_t i=1; i<num_ch; i++){
        Keyboard.write(';');
        Keyboard.write(nibble_to_hex(ch[i]>>4));
        Keyboard.write(nibble_to_hex(ch[i]));
      }
      Keyboard.write('>');
    }
  }else{
    //do something with the recognized key
    switch(index){
      case 0: Keyboard.press(KEY_LEFT_GUI); Keyboard.press('e');Keyboard.releaseAll(); break;//KP0 = Windows Explorer
      case 1: Keyboard.press(KEY_LEFT_CTRL);Keyboard.press('x');Keyboard.releaseAll(); break;//KP1 = CTRL/X (cut)
      case 2: Keyboard.press(KEY_LEFT_CTRL);Keyboard.press('c');Keyboard.releaseAll(); break;//KP2 = CTRL/C (copy)
      case 3: Keyboard.press(KEY_LEFT_CTRL);Keyboard.press('v');Keyboard.releaseAll(); break;//KP3 = CTRL/V (paste)
      case 4: Keyboard.press(KEY_LEFT_CTRL);Keyboard.press('f');Keyboard.releaseAll(); break;//KP4 = CTRL/F (find)
      case 5: Keyboard.press(KEY_LEFT_CTRL);Keyboard.press('a');Keyboard.releaseAll(); break;//KP5 = CTRL/A (all)
      case 6: Keyboard.press(KEY_LEFT_CTRL);Keyboard.press('s');Keyboard.releaseAll(); break;//KP6 = CTRL/S (save)
      case 7: Keyboard.press(KEY_LEFT_CTRL);Keyboard.press('y');Keyboard.releaseAll(); break;//KP7 = CTRL/Y (redo)
      case 8: Keyboard.press(KEY_LEFT_CTRL);Keyboard.press('z');Keyboard.releaseAll(); break;//KP8 = CTRL/Z (undo)
      case 9: Keyboard.press(KEY_LEFT_CTRL);Keyboard.press('p');Keyboard.releaseAll(); break;//KP9 = CTRL/P (print)
      case 10: Consumer.write(MEDIA_PREVIOUS);                                         break;//NLCK= F1 = MEDIA PREVIOUS
      case 11: Consumer.write(MEDIA_PLAY_PAUSE);                                       break;//KP* = F3 = MEDIA PLAY/PAUSE
      case 12: Consumer.write(MEDIA_NEXT);                                             break;//KP- = F4 = MEDIA NEXT
      case 13: Keyboard.press(KEY_LEFT_ALT);Keyboard.press(' ');Keyboard.releaseAll(); break;//KP+ = ALT/SPACE (window menu)
      case 14: Consumer.write(CONSUMER_CALCULATOR);                                    break;//KP. = CALC
      //case 15: Consumer.press(MEDIA_STOP);     break;//KP/     = F2 = MEDIA STOP doesn't seem to work so well
      case 15: Consumer.write(MEDIA_VOLUME_MUTE);                                      break; // KP/ F2  = MUTE
      case 16: Keyboard.press(KEY_LEFT_CTRL);Keyboard.press('j');Keyboard.releaseAll(); break; //KPENTER = CTRL/J (linefeed)
      default: Keyboard.write('?'); 
    }
    //do something interseting with the leds on the keyboard
    delay(10);
    send_to_keyboard(0xed);
    uint16_t ack=receive_from_keyboard();
    send_to_keyboard(leds);
    if(leds==4)leds=1; else leds*=2;
    delay(100);
  }
}

/* receive_from_keyboard() returns a 16 bit value:
 * 0 0 STOP PARITY D7 D6 D5 D4 D3 D2 D1 D0 START 0 0 0
 * 
 */

uint16_t receive_from_keyboard(){
  uint16_t buff=0;
  for (uint8_t ctr=11; ctr--; ctr!=0){
    buff= buff>>1;
    while(digitalRead(pinClock)) {
      //wait for the clock signal to go low
    }
    delayMicroseconds(20);
    //now sample the data
    if(digitalRead(pinData)){
      buff|=0x2000;
    }
    while(!digitalRead(pinClock)) {
      //just wait for the clock signal to go high again
    }
    delayMicroseconds(20);
  }
  return buff;
}

void send_to_keyboard(uint8_t leds){ // 1=scroll lock led; 2=num lock led; 4 = caps lock led
  delay(1);
  digitalWrite(pinDebug,1);
  //must be odd parity
  uint8_t parity=leds ^ (leds>>1);
  parity= parity^(parity>>2);
  parity= parity^(parity>>4);
  parity= ~parity;
  parity&= 1;
  //parity=0;//testing!
  //pull clock low
  //pinMode(pinClock, INPUT);//remove the pullup, write '0'
  pinMode(pinClock, OUTPUT);//pull clock low
  digitalWrite(pinClock,0);

  delayMicroseconds(100);
  //pinMode(pinData, INPUT);//remove the pullup, write '0'
  pinMode(pinData, OUTPUT);//pull data low
  digitalWrite(pinData,0);
  pinMode(pinClock, INPUT);//release the clock line
  pinMode(pinClock, INPUT_PULLUP);//put the pullup resistor back
  delayMicroseconds(10);
  digitalWrite(pinDebug,0);
  //send LSB first
  for(uint8_t i=0;i<8;i++){
    while(digitalRead(pinClock)) {} //wait for the clock signal to go low
    digitalWrite(pinData, leds&1);
    leds= leds>>1;
    while(!digitalRead(pinClock)) {} //wait for the clock signal to go high
  }
  while(digitalRead(pinClock)) {} //wait for the clock signal to go low
  digitalWrite(pinData, parity);
  while(!digitalRead(pinClock)) {} //wait for the clock signal to go high
  while(digitalRead(pinClock)) {} //wait for the clock signal to go low
  pinMode(pinData, INPUT);//release the data line
  pinMode(pinData, INPUT_PULLUP);//
  while(digitalRead(pinData)) {} //wait for the data signal to go low
  while(digitalRead(pinClock)) {} //wait for the clock signal to go low
  while(!digitalRead(pinClock)) {} //wait for the clock signal to go high
  while(!digitalRead(pinData)) {} //wait for the data signal to go high
}

uint8_t nibble_to_hex(uint16_t nibble){
  uint8_t n=nibble&0xf;
  if(n<10) return n+'0';
  else return n-10+'A';
}
/*<fa;69><fa;69><fa;69><fa;72><fa;7a><fa;6b><fa;73><fa;74><fa;6c><fa;75><fa;7d><fa;70><fa;71><fa;e0><fa;e0><fa;79><fa;7b><fa;7c><fa;e0><fa;e0><fa;77><fa;77>
      case 8: Keyboard.press(KEY_LEFT_CTRL);Keyboard.press('z');Keyboard.releaseAll(); break;//KP8 = CTRL/Z (undo)
      case 8: Keyboard.press(KEY_LEFT_CTRL);Keyboard.press('z');Keyboard.releaseAll(); break;//KP8 = CTRL/Z (undo)
8: Keyboard.press(KEY_LEFT_CTRL);Keyboard.press('z');Keyboard.releaseAll(); break;//KP8 = CTRL/Z (undo)
*/
