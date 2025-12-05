#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// --- CONFIGURATION ---
#define LOCK_PIN 16 

// UUIDs for the Service and Characteristic
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;

// --- CALLBACKS ---
// This class handles events when a phone connects or disconnects
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Phone Connected!");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Phone Disconnected.");
      // Restart advertising so you can connect again later
      BLEDevice::startAdvertising(); 
    }
};

// This class handles what happens when you write data from the App
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String value = pCharacteristic->getValue();

      if (value.length() > 0) {
        String command = "";
        
        // Convert the received value to a String
        for (int i = 0; i < value.length(); i++) {
          command += value[i];
        }
        
        Serial.print("Received Command: ");
        Serial.println(command);

        // --- CONTROL LOGIC ---
        // Check if the command is "open" or "close"
        if (command == "open" || command == "1") {
          digitalWrite(LOCK_PIN, HIGH); // Turn Relay ON
          Serial.println("Door UNLOCKED");
        } 
        else if (command == "close" || command == "0") {
          digitalWrite(LOCK_PIN, LOW);  // Turn Relay OFF
          Serial.println("Door LOCKED");
        }
        else {
          Serial.println("Unknown command. Send 'open' or 'close'");
        }
      }
    }
};

void setup() {
  Serial.begin(115200);
  
  // 1. Setup the Door Lock Pin
  pinMode(LOCK_PIN, OUTPUT);
  digitalWrite(LOCK_PIN, LOW); // Start locked

  // 2. Initialize BLE
  BLEDevice::init("ESP32_Door_Lock");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // 3. Create Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // 4. Create Characteristic
  // We allow READ (to see status) and WRITE (to send commands)
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("Locked"); // Initial status text

  // 5. Start the Service
  pService->start();

  // 6. Start Advertising (So your phone can find it)
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  
  Serial.println("BLE Ready! Waiting for App connection...");
}

void loop() {
  // Nothing to do here, everything is event-driven in the callbacks!
  delay(2000);
}