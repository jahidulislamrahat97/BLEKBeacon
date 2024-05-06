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
#include <BLEEddystoneTLM.h>
#include <BLEBeacon.h>
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
        const uint8_t serviceData_TLM_KBeacon[3] = {0x16, 0xAA, 0xFE}; //it has Eddystone TLM & KBeacon BLE UUID

        while (payload < payloadEnd)
        {
            if (payload[1] == serviceData_TLM_KBeacon[0] && payload[2] == serviceData_TLM_KBeacon[1] && payload[3] == serviceData_TLM_KBeacon[2])
            {
                payload += 4;
                break;
            }
            payload += *payload + 1;
        }

        if (payload < payloadEnd)
        {
            if (payload[0] == EDDYSTONE_TLM_FRAME_TYPE)
            {
                Serial.println("\n\n*************************************** TLM Found");
                BLEEddystoneTLM eddystoneTLM;
                eddystoneTLM.setData(std::string((char *)payload, 14));

                Serial.printf("TLM battery voltage: %dmV\n", eddystoneTLM.getVolt());
                Serial.printf("TLM temperature: %.2f°C\n", eddystoneTLM.getTemp());
                Serial.printf("TLM advertise count: %d\n", eddystoneTLM.getCount());
                Serial.printf("TLM time since last reboot: %ds\n", eddystoneTLM.getTime());
            }
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

        if (advertisedDevice.haveManufacturerData() == true)
        {
            std::string strManufacturerData = advertisedDevice.getManufacturerData();
            uint8_t cManufacturerData[100];
            strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);

            if (strManufacturerData.length() == 25 && cManufacturerData[0] == 0x4C && cManufacturerData[1] == 0x00)
            {
                Serial.println("\n\n*************************************** iBeacon Found");
                BLEBeacon iBeacon = BLEBeacon();
                iBeacon.setData(strManufacturerData);

                Serial.print("iBeacon Device address:");
                Serial.println(advertisedDevice.getAddress().toString().c_str()); // Print MAC address
                Serial.print("iBeacon Proximity UUID:");
                Serial.println(iBeacon.getProximityUUID().toString().c_str());
                Serial.print("iBeacon Device Major:");
                Serial.println(ENDIAN_CHANGE_U16(iBeacon.getMajor()));
                Serial.print("iBeacon Device Minor:");
                Serial.println(ENDIAN_CHANGE_U16(iBeacon.getMinor()));
                Serial.print("iBeacon Device Measured power (RSSI at a 1m distance):");
                Serial.println(iBeacon.getSignalPower());
            }
            else
            {
                // Serial.println("Found another manufacturers beacon!");
                // Serial.printf("strManufacturerData: %d ", strManufacturerData.length());
                // for (int i = 0; i < strManufacturerData.length(); i++)
                // {
                //     Serial.printf("[%X]", cManufacturerData[i]);
                // }
                // Serial.printf("\n");
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
