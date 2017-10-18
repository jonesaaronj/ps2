const byte clockPin = 1;
const byte attentionPin = 4;
const byte commandPin = 2;
const byte dataPin = 3;
const byte ackPin = 5;

byte state = 0;
byte tick = 0;

byte attention = 0;

void setup() {
  // put your setup code here, to run once:

  // set the clock interupt
  pinMode(clockPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(clockPin), handleClock, CHANGE);

  // set command and attention pins as input
  pinMode(commandPin, INPUT);
  pinMode(attentionPin, INPUT);

  // set data and ack pins as output
  pinMode(dataPin, OUTPUT);
  pinMode(ackPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
}

void handleClock() {
  
  // if attention pin is low do our thing otherwise reset tick to 0
  attention = digitalRead(attentionPin);
  if ( attention = HIGH ){
    tick = 0;
  } else {
    
  }
  
}
