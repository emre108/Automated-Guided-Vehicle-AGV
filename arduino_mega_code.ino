#include <TimerOne.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

SoftwareSerial comm(12, 11);
LiquidCrystal lcd(27, 26, 25, 24, 23, 22);

int msg[1];
RF24 radio(7, 8);
const uint64_t pipe = 0xE8E8F0F0E1LL;

#define S0 43
#define S1 42
#define S2 41
#define S3 40
#define sensorOut 38
#define buzzer 29
#define ledY 28
#define trigger 44
#define echo 45
#define LDR_Pin A1
#define lm35Pin A0
#define lift1 20
#define lift2 21
#define pwmLift 4
#define tcrt5000 3
#define tcrt50001 5
#define tcrt50002 6
#define a 30
#define b 31
#define c 32
#define d 33
#define e 34
#define f 35
#define g 37
int numx = 0;
int K, Y, M = 0;
int tcrt, tcrt1, tcrt2 = 0;
int tcrtV, tcrtV1, tcrtV2 = 0;
int bit1 = 1;
int cnt, rotation, Hwave, TotalWay, Distance, lm35Reading, LDRReading;
int tempInC;
int liftFlag = 0;
int serialAgain = 0;
int up1, down1 = 0;
int manualLiftBit, error1, error2 = 0;
void Bluetooht(), HCSR(), ColorDetect(), colorEffect(), LDR(), LM35(), Lifting(), stopL(), upL(), downL(), manualLift(), radioGaga();

void intF()  {
  cnt++;
}

void timerInt() {
  Timer1.detachInterrupt();
  rotation = cnt / 20;
  //Serial.println(rotation);
  cnt = 0;
  Timer1.attachInterrupt(timerInt);
}

void setup() {
  lcd.begin(16, 2);
  pinMode(a, OUTPUT); // Pinlerin çıkış pini olduğunu belirtiyoruz.
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(lift1, OUTPUT);
  pinMode(lift2, OUTPUT);
  pinMode(pwmLift, OUTPUT);
  pinMode(tcrt5000, INPUT);
  pinMode(tcrt50001, INPUT);
  pinMode(tcrt50002, INPUT);
  attachInterrupt(0, intF, RISING);
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(timerInt);
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);


  Serial.begin(9600);
  comm.begin(9600);
  Serial3.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();
}
void loop() {
  Bluetooht();
  radioGaga();
  Segment();
  LDR();
  LM35();
  if (liftFlag == 0) {
    HCSR();
    ColorDetect();
    colorEffect();
    if (manualLiftBit == 1) {
      Serial.println("manual lift");
      manualLift();
    }
  }
  else if (liftFlag == 1) {
    Lifting();
  }
}
void ColorDetect() {
  digitalWrite(S2, LOW); //for red
  digitalWrite(S3, LOW);
  K = pulseIn(sensorOut, LOW);
  delay(50);
  digitalWrite(S2, HIGH); //for green
  digitalWrite(S3, HIGH);
  Y = pulseIn(sensorOut, LOW);
  delay(50);
  digitalWrite(S2, LOW); //for blue
  digitalWrite(S3, HIGH);
  M = pulseIn(sensorOut, LOW);
  delay(50);
}
void colorEffect() {
  lcd.clear();
  lcd.setCursor(0, 0);
  if (K < 30 && M < 30 && Y < 30) {
    lcd.print("WHITE");
  }
  else if (K < 100 && K < Y && K < M && M < Y) {
    lcd.print("RED");
  }
  else if (Y < 130 && Y < M && Y < K && M < K) {
    liftFlag = 1;
    lcd.print("LIFTING");
    Serial.println("green");
    Serial3.write('1');
    Serial3.end();
  }
  else if (M < 100 && M < Y && M < K && K < Y && K > 30 && M > 30 && Y > 30) {
    liftFlag = 1;
    lcd.print("BLUE");
    digitalWrite(buzzer, HIGH);
    Serial.println("blue");
    Serial3.write('1');
    Serial3.end();

  }
}
void manualLift() {
  tcrt = digitalRead(tcrt5000);
  tcrt1 = digitalRead(tcrt50001);
  tcrt2 = digitalRead(tcrt50002);
  if (up1 == 1 && error1 == 0 ) {
    upL();
    if (tcrt2 == 1) {
      stopL();
      up1 = 0;
      error2 = 0;
      error1 = 1;
      tcrt2 = 0;
    }
  }
  else if (down1 == 1 && error2 == 0 ) {
    downL();
    if (tcrt1 == 1) {
      stopL();
      down1 = 0;
      error1 = 0;
      error2 = 1;
      tcrt1 = 0;
    }
  }
}
void Lifting() {
  delay(50);
  tcrt = digitalRead(tcrt5000);
  tcrt1 = digitalRead(tcrt50001);
  tcrt2 = digitalRead(tcrt50002);
  Serial.println(tcrt1);
  Serial.println(tcrt);
  Serial.println(tcrt2);

  if (tcrt2 == 1) {
    tcrtV2 = 1;
  }

  else if (tcrt == 1) {
    tcrtV = 1;
  }
  else if (tcrt1 == 1) {
    tcrtV1 = 1;
  }

  else if (bit1 == 2) {
    Serial3.write('0');
    Serial.println("go fool");
    bit1 = 1;
    liftFlag = 0;
  }
  else if (bit1 == 1) {
    upL();
    lcd.setCursor(8, 0);
    lcd.print("UP");
    Serial.println("up up");
    if (tcrtV2 == 1) {
      lcd.setCursor(8, 0);
      lcd.print("STOP");
      Serial.println("stop");
      stopL();
      bit1 = 0;
      tcrtV2 = 0;
    }
  }
  else if (tcrtV == 1) {
    Serial.println("pressed");
    if (bit1 == 0) {
      Serial.println("down down");
      lcd.setCursor(8, 0);
      lcd.print("DOWN");
      downL();
      if (tcrtV1 == 1) {
        Serial.println("stop");
        stopL();
        tcrtV = 0;
        tcrt = 0;
        tcrt1 = 0;
        tcrt2 = 0;
        tcrtV1 = 0;
        bit1 = 2;
        Serial3.begin(9600);
      }
    }
  }
}

void upL() {
  analogWrite(pwmLift, 240);
  digitalWrite(lift1, HIGH);
  digitalWrite(lift2, LOW);
}
void stopL() {
  analogWrite(pwmLift, 0);
}
void downL() {
  analogWrite(pwmLift, 240);
  digitalWrite(lift1, LOW);
  digitalWrite(lift2, HIGH);
}
void HCSR() {
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  Hwave = pulseIn(echo, HIGH);
  TotalWay = (double)Hwave * 0.034;
  Distance = TotalWay / 2;
  lcd.setCursor(7, 0);
  lcd.print("D:");
  lcd.print(Distance);
  Serial.println(Distance);
  if (Distance <= 10) {
    digitalWrite(buzzer, HIGH);
    Serial3.write('1');
    lcd.setCursor(0, 0);
    lcd.print("!-!-!");
    lcd.setCursor(0, 1);
    lcd.print("CAUTION");
    lcd.setCursor(8, 1);
    lcd.print("GIVE WAY");
    Serial.println("stopMotor");
  }
  else if (Distance >= 10 ) {
    lcd.setCursor(0, 1);
    lcd.print("AUTONOMOUS DRIVE");
    lcd.setCursor(0, 0);
    lcd.print("--->");
    digitalWrite(buzzer, LOW);
    Serial3.write('0');
    Serial.println("RUN!!");
  }
}
void Bluetooht() {
  if (comm.available()) {
    Serial.println("yes");
    char data = comm.read();
    Serial.println(data);
    delay(50);
    if (data == 'w') {
      digitalWrite(buzzer, HIGH);
      digitalWrite(ledY, LOW);
      Serial3.write('w');
    }
    else if (data == 's') {
      digitalWrite(buzzer, HIGH);
      digitalWrite(ledY, HIGH);
      Serial3.write('s');
    }
    else if (data == 'a') {
      digitalWrite(buzzer, HIGH);
      digitalWrite(ledY, HIGH);
      Serial3.write('a');
    }
    else if (data == 'd') {
      digitalWrite(buzzer, HIGH);
      digitalWrite(ledY, HIGH);
      Serial3.write('d');
    }
    else if (data == 'm') {
      digitalWrite(buzzer, HIGH);
      digitalWrite(ledY, HIGH);
      Serial3.write('m');
    }
    else if (data == 'o') {
      digitalWrite(buzzer, HIGH);
      digitalWrite(ledY, HIGH);
      Serial3.write('o');
    }
    else if (data == 'q') {
      digitalWrite(buzzer, HIGH);
      digitalWrite(ledY, HIGH);
      Serial3.write('q');
    }
    else if (data == 'e') {
      digitalWrite(buzzer, HIGH);
      digitalWrite(ledY, HIGH);
      Serial3.write('e');
    }
    else if (data == 'z') {
      digitalWrite(buzzer, HIGH);
      digitalWrite(ledY, HIGH);
      Serial3.write('z');
    }
    else if (data == '3') {
      digitalWrite(buzzer, HIGH);
      digitalWrite(ledY, HIGH);
      Serial3.write('3');
      Serial.println(data);

    }
    else if (data == '7') {
      manualLiftBit = 1;
      Serial.println(manualLiftBit);
    }
    else if (data == '9') {
      manualLiftBit = 0;
      down1 = 0;
      up1 = 0;
    }
    else if (data == '8') {
      down1 = 0;
      up1 = 1;
    }
    else if (data == '5') {
      up1 = 0;
      down1 = 1;
    }
  }
}
void LDR() {
  LDRReading = analogRead(LDR_Pin);
  LDRReading = map(LDRReading, 0, 1023, 0, 100);
  Serial.println(LDRReading);
  lcd.setCursor(0, 1);
  lcd.print("LUX: ");
  lcd.print(LDRReading);
}
void LM35() {
  lm35Reading = analogRead(lm35Pin);
  int tempInC = map(lm35Reading, 0, 255, 0, 1023) / 9.31;
  lcd.setCursor(12, 0);
  lcd.print("C:");
  lcd.print(tempInC);
}
void radioGaga() {
  if (radio.available()) {
    bool done = false;
    done = radio.read(msg, 1);
    Serial.println(msg[0]);
    if (msg[0] == 1) {
      numx = 3;
      digitalWrite(buzzer, HIGH);
      delay(10);
    }
    /* else {
       digitalWrite(buzzer, LOW);
       Serial.println("No radio available");
      }*/
  }
}

void Segment() {
  if (Serial.available() > 0) {
    int w = Serial.read() - 48;
    Serial.println(w);
    sevenSegment(w);
    delay(10);
  }
}
void sevenSegment(int x) {
  switch (x) {
    case 0: {
        digitalWrite(a, LOW);
        digitalWrite(b, LOW);
        digitalWrite(c, LOW);
        digitalWrite(d, LOW);
        digitalWrite(e, LOW);
        digitalWrite(f, LOW);
        digitalWrite(g, HIGH);
      }
      break;
    case 1: {
        digitalWrite(a, HIGH);
        digitalWrite(b, LOW);
        digitalWrite(c, LOW);
        digitalWrite(d, HIGH);
        digitalWrite(e, HIGH);
        digitalWrite(f, HIGH);
        digitalWrite(g, HIGH);
      }
      break;

    case 2:
      {
        digitalWrite(a, LOW);
        digitalWrite(b, LOW);
        digitalWrite(c, HIGH);
        digitalWrite(d, LOW);
        digitalWrite(e, LOW);
        digitalWrite(f, HIGH);
        digitalWrite(g, LOW);
      }
      break;
    case 3:
      {
        digitalWrite(a, LOW);
        digitalWrite(b, LOW);
        digitalWrite(c, LOW);
        digitalWrite(d, LOW);
        digitalWrite(e, HIGH);
        digitalWrite(f, HIGH);
        digitalWrite(g, LOW);
      }
      break;

    case 4:
      {
        digitalWrite(a, HIGH);
        digitalWrite(b, LOW);
        digitalWrite(c, LOW);
        digitalWrite(d, HIGH);
        digitalWrite(e, HIGH);
        digitalWrite(f, LOW);
        digitalWrite(g, LOW);
      }
      break;
    case 5:
      {
        digitalWrite(a, LOW);
        digitalWrite(b, HIGH);
        digitalWrite(c, LOW);
        digitalWrite(d, LOW);
        digitalWrite(e, HIGH);
        digitalWrite(f, LOW);
        digitalWrite(g, LOW);
      }
      break;
    case 6:
      {
        digitalWrite(a, LOW);
        digitalWrite(b, HIGH);
        digitalWrite(c, LOW);
        digitalWrite(d, LOW);
        digitalWrite(e, LOW);
        digitalWrite(f, LOW);
        digitalWrite(g, LOW);
      }
      break;
    case 7:
      {
        digitalWrite(a, LOW);
        digitalWrite(b, LOW);
        digitalWrite(c, LOW);
        digitalWrite(d, HIGH);
        digitalWrite(e, HIGH);
        digitalWrite(f, HIGH);
        digitalWrite(g, HIGH);
      }
      break;
    case 8:
      {
        digitalWrite(a, LOW);
        digitalWrite(b, LOW);
        digitalWrite(c, LOW);
        digitalWrite(d, LOW);
        digitalWrite(e, LOW);
        digitalWrite(f, LOW);
        digitalWrite(g, LOW);
      }
      break;
    case 9:
      {
        digitalWrite(a, LOW);
        digitalWrite(b, LOW);
        digitalWrite(c, LOW);
        digitalWrite(d, LOW);
        digitalWrite(e, HIGH);
        digitalWrite(f, LOW);
        digitalWrite(g, LOW);
      }
      break;
  }
}
