//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
#include "EEPROM.h"
#include <Servo.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif


#define USE_SERIAL Serial

static const int servoPin = 13;
Servo servo1;
SSD1306Wire display(0x3c, 4, 15);
char message[100];
BluetoothSerial SerialBT;

void setup() {
    /*
  * SERVO
  */
  servo1.attach(servoPin);
  servo1.write(100);
  delay(100);
  servo1.write(95);
  /*
  * LCD
  */
  pinMode(16,OUTPUT); 
  digitalWrite(16,LOW); 
  delay(50); 
  digitalWrite(16,HIGH); 
  // Initialising the UI will init the display too.
  display.init();
  display.clear();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  Serial.begin(115200);
  SerialBT.begin("ESP32Serial"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
}

char * read_from_BT() {
  #define BUFFER_SIZE 100
  static char buffer[BUFFER_SIZE];
  bool go=true;
  int i=0;
  char c;
  while (go){
    if (SerialBT.available()){
      c=SerialBT.read();
      buffer[i++]=c;
      Serial.write(c);
    }
    if (c==0 || c=='\n') {
      return buffer;
    }
    delay(20);
  }
}
void loop() {
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    char * command=read_from_BT();
    display.clear();
    display.drawStringMaxWidth(0, 0, 128,command);
    display.display();
    if (strstr(command,"angle:")){
      int angle;
      angle=atoi(&command[6]);
      servo1.write(angle);
    }
    delay(1000);
  }
  delay(20);
}
