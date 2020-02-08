//utilizes 2 buttons now

#include <IRremote.h>
#include <avr/sleep.h>
#include <avr/power.h>

int RECV_PIN = 10;
byte powerPin = 11;
byte gndPin = 12;
const byte onButtonPin = 6;
const byte offButtonPin = 5;
const byte buttonPressPin = 2;
int STATUS_PIN = 13;

byte lastOnButtonState;
byte lastOffButtonState;

unsigned long lastOnDebounceTime = 0;  // the last time the output pin was toggled
unsigned long wakeDuration = 5000;      //duration of time to stay awake and handle button presses/debounces (in ms)
unsigned long wakeUpTime;
unsigned long lastOffDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

// Storage for the recorded code
IRsend irsend;

void setup()
{
  pinMode(gndPin, OUTPUT);
  pinMode(powerPin, OUTPUT);
  digitalWrite(gndPin, LOW);
  digitalWrite(powerPin, HIGH);
  
  Serial.begin(115200);
  pinMode(onButtonPin, INPUT);
  pinMode(offButtonPin, INPUT);
  pinMode(buttonPressPin, INPUT);
  pinMode(STATUS_PIN, OUTPUT);

}

void loop() {
  wakeUpTime = millis();
  while((millis() - wakeUpTime) < wakeDuration){        /*while we need to stay awake, handle button presses*/
    // If button pressed, send the code.
    byte onButtonState = digitalRead(onButtonPin);
    byte offButtonState = digitalRead(offButtonPin);
  
  // If the switch changed, due to noise or pressing:
    if (onButtonState != lastOnButtonState) {
      // reset the debouncing timer
      lastOnDebounceTime = millis();
    }
    if (offButtonState != lastOffButtonState) {
      // reset the debouncing timer
      lastOffDebounceTime = millis();
    }
  
    if ((millis() - lastOnDebounceTime) > debounceDelay) {
      // whatever the reading is at, it's been there for longer than the debounce
      // delay, so take it as the actual current state:
  
      if (onButtonState) {
        Serial.println("Pressed on, sending");
        digitalWrite(STATUS_PIN, HIGH);
        sendOnCode(lastOnButtonState == onButtonState);
        digitalWrite(STATUS_PIN, LOW);
        delay(100); // Wait a bit between retransmissions
      }     
   }
  
   if ((millis() - lastOffDebounceTime) > debounceDelay) {
      // whatever the reading is at, it's been there for longer than the debounce
      // delay, so take it as the actual current state:
  
      if (offButtonState) {
        Serial.println("Pressed off, sending");
        digitalWrite(STATUS_PIN, HIGH);
        sendOffCode(lastOffButtonState == offButtonState);
        digitalWrite(STATUS_PIN, LOW);
        delay(100); // Wait a bit between retransmissions
      }     
   }
    lastOnButtonState = onButtonState;
    lastOffButtonState = offButtonState;
  }
  enterSleep();                                               /*after wakeDuration, go back to sleep*/
}

void sendOnCode(int repeat) {
  irsend.sendNEC(0x20DF10EF, 32);   /*TV power*/
  delay(250);
  irsend.sendNEC(0x5EA1B847, 32);   /*Reciever/computer power*/
  delay(250);
  Serial.println("Sent on code");
}

void sendOffCode(int repeat) {
  irsend.sendNEC(0x20DF10EF, 32);   /*TV power*/
  delay(250);
  irsend.sendNEC(0x5EA17887, 32);   /*Reciever/computer power*/
  delay(250);
  Serial.println("Sent off code");
}

//Description: Service routine for pin2 interrupt  
void pin2Interrupt(void)
{
  /* This will bring us back from sleep. */
}
//Description: Enters the arduino into sleep mode.
void enterSleep(void)
{  
  /* Setup pin2 as an interrupt and attach handler. */
  attachInterrupt(0, pin2Interrupt, HIGH);
  delay(100);  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  
  sleep_enable();  
  sleep_mode();  
  /* The program will continue from here. */  
  /* First thing to do is disable sleep. */
  sleep_disable(); 
  detachInterrupt(0);
}
