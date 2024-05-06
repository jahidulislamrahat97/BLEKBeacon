/*
 * BLEKBeacon.cpp
 *
 *  Created on: May 06, 2024
 *      Author: Jahidul Islam Rahat <jahidulislamrahat97@gmail.com>
 * 
 *  This file implements the BLEKBeacon class.
 *  BLEKBeacon is a class for processing BLE beacon data.
 *  It extracts various sensor data from the KBeacon like KBPro_XXXX.
 */
#ifndef _BLEKBEACON_H_
#define _BLEKBEACON_H_
#include "BLEUUID.h"
#include <Arduino.h>

#define KBEACON_FRAME_TYPE 0x21
#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00)>>8) + (((x)&0xFF)<<8))
#define ENDIAN_CHANGE_U32(x) ((((x)&0xFF000000)>>24) + (((x)&0x00FF0000)>>8)) + ((((x)&0xFF00)<<8) + (((x)&0xFF)<<24))
#define KBEACON_TEMP_U16_TO_FLOAT(tempU16)   (((int16_t)ENDIAN_CHANGE_U16(tempU16)) / 256.0f)
#define KBEACON_TEMP_FLOAT_TO_U16(tempFloat) (ENDIAN_CHANGE_U16(((int)((tempFloat) * 256))))


class BLEKBeacon
{
private:
    struct KBeaconData
    {
        bool voltage_available;
        bool temperature_available;
        bool humidity_available;
        bool accelerometer_available;
        bool cutoff_indication;
        bool pir_available;
        bool light_available;
        bool voc_available;
        bool co2_available;
        int index;
        uint16_t volt;
        uint16_t temp;
        uint16_t hum;
        int16_t acc_x;
        int16_t acc_y;
        int16_t acc_z;
    }__attribute__((packed)) kb;
    
public:
    BLEKBeacon();
    void setData(uint8_t *payload);
    uint16_t getVoltage();
    float getTemperature();
    float getHumidity();
    int16_t getAccelerometer_X();
    int16_t getAccelerometer_Y();
    int16_t getAccelerometer_Z();
};



#endif