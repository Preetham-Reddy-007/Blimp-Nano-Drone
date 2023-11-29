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

#define RED 22
#define BLUE 24
#define GREEN 23

BLEService blimpService("1828");
BLEUnsignedIntCharacteristic  dirCharacteristic   ("19B1", BLEWrite);
BLEUnsignedIntCharacteristic  levelCharacteristic ("19B2", BLEWrite);
BLECharacteristic             meshCharacteristic  ("2add", BLEWrite, 33);

uint8_t mesh_data[33];
int mesh_flag = 0;
int mm_size = 0;
unsigned int level = 0;

void setup() {
  // initialize the Bluetooth® Low Energy hardware
  BLE.begin();

  Serial.println("Bluetooth® Low Energy Central - LED control");
  BLE.setLocalName("NVV Blimp");

  BLE.setAdvertisedService(blimpService);

  blimpService.addCharacteristic(dirCharacteristic);
  blimpService.addCharacteristic(levelCharacteristic);
  blimpService.addCharacteristic(meshCharacteristic);
  BLE.addService(blimpService);
  dirCharacteristic.writeValue(0);
  levelCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();
  // start scanning for peripherals
  BLE.scanForUuid("1828");
  indicateLed(RED);
  Scheduler.startLoop(loop1);

}

void loop() {
  BLEDevice peripheral = BLE.available();
  if (peripheral) {
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();
    if (peripheral.localName() != "Mesh Proxy") {
      return;
    }
    BLE.stopScan();
    controlMesh(peripheral);
    BLE.scanForUuid("1828");
  }
  yield();
}

void loop1()
{
  BLEDevice central = BLE.central();
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
    while (central.connected()) {
      indicateLed(GREEN);
      if (meshCharacteristic.written()) {
          indicateLed(BLUE);
          delay(2000);
          mm_size = meshCharacteristic.valueLength();
          meshCharacteristic.readValue(mesh_data, 33);
          mesh_flag = 1;
        }
        yield();
      }
      Serial.print(F("Disconnected from central: "));
      Serial.println(central.address());
      indicateLed(RED);
    }
}

void controlMesh(BLEDevice peripheral) {
    Serial.println("Connecting ...");
    if (peripheral.connect()) {
      Serial.println("Connected");
    } else {
      Serial.println("Failed to connect!");
      return;
    }
    Serial.println("Discovering attributes ...");
    if (peripheral.discoverAttributes()) {
      Serial.println("Attributes discovered");
    } else {
      Serial.println("Attribute discovery failed!");
      peripheral.disconnect();
      return;
    }
    BLECharacteristic ProxyInCharacteristic = peripheral.characteristic("2add");

    if (!ProxyInCharacteristic) {
      Serial.println("Peripheral does not have Proxy In characteristic!");
      peripheral.disconnect();
      return;
    } else if (!ProxyInCharacteristic.canWrite()) {
      Serial.println("Peripheral does not have a writable Proxy In characteristic!");
      peripheral.disconnect();
      return;
    }

    while (peripheral.connected()) {
      
      if (mesh_flag != 0) {
          ProxyInCharacteristic.writeValue(mesh_data, mm_size, true);
          mesh_flag = 0;
          indicateLed(GREEN);
        }
      }
    Serial.println("Peripheral disconnected");
  }

void indicateLed(int colour)
  {
    digitalWrite(RED, HIGH);
    digitalWrite(BLUE, HIGH);
    digitalWrite(GREEN, HIGH);
    digitalWrite(colour, LOW);
  }
  
