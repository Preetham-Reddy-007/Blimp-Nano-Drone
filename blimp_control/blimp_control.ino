/*
  LED Control

  This example scans for Bluetooth® Low Energy peripherals until one with the advertised service
  "19b10000-e8f2-537e-4f6c-d104768a1214" UUID is found. Once discovered and connected,
  it will remotely control the Bluetooth® Low Energy peripheral's LED, when the button is pressed or released.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.
  - Button with pull-up resistor connected to pin 2.

  You can use it with another board that is compatible with this library and the
  Peripherals -> LED example.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>
#include <Scheduler.h>
#include "Adafruit_VL53L0X.h"
#define tof_en
#ifdef tof_en
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
#endif
//#define debug_en
#define front_left  3
#define front_right 5
#define rear_left   4
#define rear_right  2
#define RED 22
#define BLUE 24
#define GREEN 23

BLEService                    blimpService        ("19B0"); // Bluetooth® Low Energy LED Service
BLEUnsignedIntCharacteristic  dirCharacteristic   ("19B1", BLEWrite);
BLEUnsignedIntCharacteristic  levelCharacteristic ("19B2", BLEWrite);
BLECharacteristic             meshCharacteristic  ("19B3", BLEWrite, 33);

BLEDevice central;
uint8_t mesh_data[33];
int mesh_flag = 0;
int mm_size = 0;
unsigned int level = 0;

void setup() {
#ifdef debug_en
  Serial.begin(9600);
  while (!Serial);
#endif
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(front_left, OUTPUT);
  pinMode(front_right, OUTPUT);
  pinMode(rear_left, OUTPUT);
  pinMode(rear_right, OUTPUT);
  analogWrite(front_left, 0);
  analogWrite(front_right, 0);
  analogWrite(rear_left, 0);
  analogWrite(rear_right, 0);
#ifdef debug_en
  Serial.println("Adafruit VL53L0X test.");
#endif
#ifdef tof_en
  if (!lox.begin()) {
#ifdef debug_en
    Serial.println(F("Failed to boot VL53L0X"));
#endif
    while (1);
  }
#endif
#ifdef debug_en
  Serial.println(F("VL53L0X API Continuous Ranging example\n\n"));
#endif
  // start continuous ranging
#ifdef tof_en
  lox.startRangeContinuous();
#endif
  // initialize the Bluetooth® Low Energy hardware
  BLE.begin();
#ifdef debug_en
  Serial.println("Bluetooth® Low Energy Central - Blimp control");
#endif
  BLE.setLocalName("NVV Blimp");

  BLE.setAdvertisedService(blimpService);

  blimpService.addCharacteristic(dirCharacteristic);
  blimpService.addCharacteristic(levelCharacteristic);
  blimpService.addCharacteristic(meshCharacteristic);
  // add service
  BLE.addService(blimpService);
  // add the characteristic to the service

  // set the initial value for the characeristic:
  dirCharacteristic.writeValue(0);

  levelCharacteristic.writeValue(0);
  // start advertising
  BLE.setConnectionInterval(20, 20);
  BLE.setAdvertisingInterval(160);
  BLE.advertise();
  indicateLed(RED);
}

void loop()
{
  unsigned long val = 0;
  int temp_level = 0;
  // if a central is connected to peripheral:
  central = BLE.central();
  if (central) {

#ifdef debug_en
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
#endif
    indicateLed(GREEN);
    // while the central is still connected to peripheral:
    while (central.connected())
    {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      if (dirCharacteristic.written()) {
        val = dirCharacteristic.value();
        analogWrite(front_left, (uint8_t)(val & 0x00ff) >> 1);
        analogWrite(front_right, (uint8_t)((val & 0xff00) >> 9));

      }
      if (levelCharacteristic.written()) {
        level = levelCharacteristic.value();
      
#ifdef debug_en
        Serial.println("Level");
        Serial.println(level);
#endif
      }
      if (meshCharacteristic.written()) 
        indicateLed(BLUE);
#ifdef tof_en
      if (lox.isRangeComplete())
      {
        //Serial.println("Completed Ranging");
        temp_level = lox.readRange() / 10;
        if (temp_level < (level & 0x00ff))
        {
          analogWrite(rear_left,  (uint8_t)((level & 0xff00) >> 8));
          analogWrite(rear_right, (uint8_t)((level & 0xff00) >> 8));
        }
        else if (temp_level > (level & 0x00ff) + 5)
        {
          analogWrite(rear_left,  0 );
          analogWrite(rear_right, 0 );
        }
#ifdef debug_en
        Serial.println(temp_level);
#endif
      }

#endif
    }
    central = BLE.central();
    yield();
  }
  // when the central disconnects, print it out:
#ifdef debug_en
  Serial.print(F("Disconnected from central: "));
  Serial.println(central.address());
#endif
  indicateLed(RED);
  analogWrite(rear_left,  0 );
  analogWrite(rear_right, 0 );
  analogWrite(front_left,  0 );
  analogWrite(front_right, 0 );
}


void indicateLed(int colour)
{
  digitalWrite(RED, HIGH);
  digitalWrite(BLUE, HIGH);
  digitalWrite(GREEN, HIGH);
  digitalWrite(colour, LOW);
}
