//Fabis wakeup system
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include <boarddefs.h>
#include <IRremoteInt.h>



/**************************
 * Define needed Variables and Objects
 */
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
IRrecv irrecv(4);
decode_results results;
IRsend irsend;
// buttonstates
unsigned int button1  = 0;
unsigned int button2  = 0;
unsigned int button3  = 0;
// interval count the loops and is used as time
unsigned int interval = 0;
// startpoints and snoozetime
unsigned int startLight1 = 20;  // [in Secounds] twenty secounds
unsigned int startLight2 = 60;  // [in Secounds] one minute
unsigned int startLight3 = 120; // [in Secounds] two minutes
unsigned int startTV     = 180; // [in Secounds] three minutes
unsigned int startAlarm  = 360; // [in Secounds] six minutes
unsigned int snoozetime  = 600; // [in Secounds] ten minutes
unsigned int sleepTime   = 510; // [in Minutes]  eight hours and 30 minutes
// Controlvars (the alarm variable control the piezobuzzer)
boolean wait        = true;
boolean alarm       = false;
boolean calibration = false;
// msg for later concats
String  msg   = "";



/**************************
 * Initialize some stuff
 */
void setup() {
  // initialize display
  lcd.init();
  //lcd.backlight();
  show("Wakeup Fabi    ", "");
  
  // initialize needed pins
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, OUTPUT);
  
}



/**************************
 * Main Loop
 */
void loop() {
  
  // Read Buttonstate
  button1 = digitalRead(5);
  button2 = digitalRead(6);
  button3 = digitalRead(7);
  
  // set calibration mode
  if (button1 == LOW && button3 == LOW) {
    lcd.backlight(); // start lcd backlight
    show("  Calibration   ", "                ");
    calibration = true;
  }
  
  // this replace a timer
  if (calibration) {
    if (button1 == LOW) light(1);  // Light on
    if (button2 == LOW) light(13); // Light Max
    if (button3 == LOW) light(20); // Light Party
  } else if (wait) {
    if ((millis()/1000/60) < sleepTime) {
      msg = "Wakeup in ";
      msg.concat(sleepTime - (millis()/1000/60));
      msg.concat("min");
      show("Fabi Dudus .*o*O", msg);
    } else {
      wait = false;
    }
  } else {
    msg = "Interval: ";
    msg.concat(interval);
    show("", msg);
    
    // Execute buttonfunction
    button1 = digitalRead(5);
    button2 = digitalRead(6);
    button3 = digitalRead(7);
    if (button1 == LOW) snooze();
    if (button2 == LOW) wocken();
    if (button3 == LOW) fuckOff();
    
    // main pcrocedur
    if (alarm) {
      makeNoise();
    } else {
      if (interval == 5) {
        lcd.backlight(); // start lcd backlight
      }
      if (interval == startLight1) {
        show("Start Light    *", "");
        light(1);
        delay(200);
        light(11);
      }
      if (interval == startLight2) {
        show("Increase Light**", "");
        light(12);
      }
      if (interval == startLight3) {
        show("Max Light    ***", "");
        light(13);
      }
      if (interval == startTV) {
        show("Start TV  ***[#]", "");
        tv(11);
      }
      if (interval == startAlarm) {
        show("Start Alarm  (o)", "");
        alarm = true;
      }
    }
  }
  delay(1000);
  interval++;
}



/**************************
 * Button Functions
 */
void snooze() {
  light(11); // dim light
  alarm = false; // stop alarm
  startLight2 = (interval+snoozetime+30);
  startLight3 = (interval+snoozetime+90);
  startAlarm  = (interval+snoozetime+150);
  msg = "SNOOZE till:";
  msg.concat(startAlarm);
  show(msg, "");
}
void wocken() {
  light(20); // party light
  alarm = false; // stop alarm
  show("Fabi woken      ", "");
}
void fuckOff() {
  light(0); // turnoff Light
  alarm = false; // stop alarm
  tv(0);
  show("   F@#K OFF MODE", "");
}



/**************************
 * Wakeup Functions
 */
void light(int level) {
  switch (level) {
    case 0:  irsend.sendNEC(1082081471, 32); break; // off
    case 1:  irsend.sendNEC(1082097791, 32); break; // on
    case 11: irsend.sendNEC(1082083511, 32); break; // light 1
    case 12: irsend.sendNEC(1082124311, 32); break; // light 2
    case 13: irsend.sendNEC(1082118191, 32); break; // light 3
    case 20: irsend.sendNEC(1082077391, 32); break; // partymode
    default: show("====  ERROR ====", "  invalid Level ");
  }
  // TEST: fire empty code after every command (Handremote makes this by buttonhold to mark between repeats)
  //irsend.sendNEC(4294967295, 32);
}
void tv(int func) {
  switch (func) {
    case 0:  irsend.sendNEC(8323837, 32); break; // on/off
    case 1:  irsend.sendNEC(8323837, 32); break; // on/off
    case 11: irsend.sendNEC(8376367, 32); break; // P+
    //case 12: irsend.sendNEC(0, 32); break;       // P-
    case 13: irsend.sendNEC(8335567, 32); break; // Vol+
    //case 14: irsend.sendNEC(0, 32); break;       // Vol-
    case 21: irsend.sendNEC(8331997, 32); break; // play/pause
    //case 22: irsend.sendNEC(0, 32); break;       // next
    //case 23: irsend.sendNEC(0, 32); break;       // Prev
    //case 24: irsend.sendNEC(0, 32); break;       // stop
    default: show("====  ERROR ====", "invalid Function");
  }
  
}
void makeNoise() {
  unsigned int onTime = 40;   // Use this variables to change the sound
  unsigned int offTime = 80; // (onTime can be used to decrease the Volume)
  for (int i = 0; i <= 2; i++) {
    digitalWrite(8, HIGH);
    delay(onTime);
    digitalWrite(8, LOW);
    delay(offTime);
  }
}

/**************************
 * Basic Functions
 */
void show(String line1, String line2) {
  if (line1.length() > 0) {
    lcd.setCursor(0,0);
    lcd.print(line1);
  }
  if (line2.length() > 0) {
    lcd.setCursor(0,1);
    lcd.print(line2);
  }
}
