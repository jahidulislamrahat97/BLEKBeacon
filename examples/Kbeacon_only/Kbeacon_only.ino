/*********************************************************************************
   If this code works, it was written by Jahidul Islam Rahat.
   If not, I don't know who wrote it.
   :) xD

   Author: Jahidul Islam Rahat.
   Date: 06 May 2024.
*********************************************************************************/

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEKBeacon.h>

int scanTime = 5; // In seconds
BLEScan *pBLEScan;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        uint8_t *payload = advertisedDevice.getPayload();
        const size_t payloadLen = advertisedDevice.getPayloadLength();
        uint8_t *payloadEnd = payload + payloadLen - 1;
        const uint8_t serviceData_KBeacon[3] = {0x16, 0xAA, 0xFE}; //it has KBeacon BLE UUID

        while (payload < payloadEnd)
        {
            if (payload[1] == serviceData_KBeacon[0] && payload[2] == serviceData_KBeacon[1] && payload[3] == serviceData_KBeacon[2])
            {
                payload += 4;
                break;
            }
            payload += *payload + 1;
        }

        if (payload < payloadEnd)
        {
            if (payload[0] == KBEACON_FRAME_TYPE)
            {
                Serial.println("\n\n*************************************** KBeacon Found");
                BLEKBeacon kBeacon;
                kBeacon.setData(payload);

                Serial.print("KBeacon name: ");
                Serial.println(advertisedDevice.getName().c_str());
                Serial.print("KBeacon address:");
                Serial.println(advertisedDevice.getAddress().toString().c_str());
                Serial.printf("KBeacon RSSI: %d \n", advertisedDevice.getRSSI());
                BLEUUID devUUID = advertisedDevice.getServiceUUID();
                Serial.print("KBeacon Service UUID: ");
                Serial.println(devUUID.toString().c_str());
                Serial.printf("KBeacon battery voltage: %dmV\n", kBeacon.getVoltage());
                Serial.printf("KBeacon temperature: %.2f°C\n", kBeacon.getTemperature());
                Serial.printf("KBeacon humidity: %.2f%\n", kBeacon.getHumidity());
                Serial.printf("KBeacon acc x:%d, y:%d, z:%d\n", kBeacon.getAccelerometer_X(),kBeacon.getAccelerometer_Y(),kBeacon.getAccelerometer_Z());
            }
        }

        
        
    }
};

void setup()
{
    Serial.begin(115200);
    Serial.println("Scanning...");

    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan(); // Create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true); // Active scan uses more power, but gets results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99); // Less or equal setInterval value
}

void loop()
{
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    Serial.print("Devices found: ");
    Serial.println(foundDevices.getCount());
    Serial.println("Scan done!");
    pBLEScan->clearResults();
    delay(2000);
}
