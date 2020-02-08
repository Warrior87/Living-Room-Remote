//utilizes 2 buttons now

#include <IRremote.h>

int RECV_PIN = 10;
byte powerPin = 11;
byte gndPin = 12;
byte onButtonPin = 6;
byte offButtonPin = 5;
byte buttonPressPin = 2;
int STATUS_PIN = 13;

byte lastOnButtonState;
byte lastOffButtonState;

unsigned long lastOnDebounceTime = 0;  // the last time the output pin was toggled
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
  // If button pressed, send the code.
  byte onButtonState = digitalRead(onButtonPin);
  byte offButtonState = digitalRead(offButtonPin);
//  if (lastOnButtonState == HIGH && onButtonState == LOW) {
//    Serial.println("on button pressed");
//  }
//  if (lastOffButtonState == HIGH && offButtonState == LOW) {
//    Serial.println("off button pressed");
//  }

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

//  if (onButtonState) {
//    Serial.println("Pressed on, sending");
//    digitalWrite(STATUS_PIN, HIGH);
//    sendOnCode(lastOnButtonState == onButtonState);
//    digitalWrite(STATUS_PIN, LOW);
//    delay(100); // Wait a bit between retransmissions
//  } 
//  if (offButtonState) {
//    Serial.println("Pressed off, sending");
//    digitalWrite(STATUS_PIN, HIGH);
//    sendOffCode(lastOffButtonState == offButtonState);
//    digitalWrite(STATUS_PIN, LOW);
//    delay(100); // Wait a bit between retransmissions
//  } 
  lastOnButtonState = onButtonState;
  lastOffButtonState = offButtonState;
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
