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

#include "BLEKBeacon.h"
#include <string.h>


BLEKBeacon::BLEKBeacon(){

}

/**
 * Process the payload data to extract sensor information.
 * 
 * @param payload Pointer to the payload data.
 */
void BLEKBeacon::setData(uint8_t *payload){
    unsigned short sensor_mask = payload[2] | (payload[1] << 8);
    kb.voltage_available = sensor_mask & (1 << 0);
    kb.temperature_available = sensor_mask & (1 << 1);
    kb.humidity_available = sensor_mask & (1 << 2);
    kb.accelerometer_available = sensor_mask & (1 << 3);
    kb.cutoff_indication = sensor_mask & (1 << 4);
    kb.pir_available = sensor_mask & (1 << 5);
    kb.light_available = sensor_mask & (1 << 6);
    kb.voc_available = sensor_mask & (1 << 7);
    kb.co2_available = sensor_mask & (1 << 9);
    kb.index = 0;

    if (kb.voltage_available)
        kb.volt = payload[3] | (payload[4] << 8);

    if (kb.temperature_available)
        kb.temp = (int)payload[5] + (int)(payload[6] << 8);
    
    if (kb.humidity_available)
    {
        kb.hum = (int)payload[7] + (int)(payload[8] << 8);
    }
    else
    {
        kb.index = 2;
        kb.hum = 0;
    }

    if (kb.accelerometer_available)
    {
        kb.acc_x = payload[10 - kb.index] | (payload[9 - kb.index] << 8);
        kb.acc_y = payload[12 - kb.index] | (payload[11 - kb.index] << 8);
        kb.acc_z = payload[14 - kb.index] | (payload[13 - kb.index] << 8);
    }
}

/**
 * Get the voltage value from the beacon data.
 * 
 * @return Battery voltage, unit is mV.
 */
uint16_t BLEKBeacon::getVoltage()
{
    return ENDIAN_CHANGE_U16(kb.volt);
}

/**
 * Get the temperature value from the beacon data.
 * 
 * @return Temperature value, unit is celsius.
 */
float BLEKBeacon::getTemperature()
{
    return KBEACON_TEMP_U16_TO_FLOAT(kb.temp);
}

/**
 * Get the humidity value from the beacon data.
 * 
 * @return Humidity value.
 */
float BLEKBeacon::getHumidity()
{
    return KBEACON_TEMP_U16_TO_FLOAT(kb.hum);
}

/**
 * Get the X-axis accelerometer value from the beacon data.
 * 
 * @return X-axis accelerometer value, unit is mg.
 */
int16_t BLEKBeacon::getAccelerometer_X()
{
    return kb.acc_x;
}

/**
 * Get the Y-axis accelerometer value from the beacon data.
 * 
 * @return Y-axis accelerometer value, unit is mg.
 */
int16_t BLEKBeacon::getAccelerometer_Y()
{
    return kb.acc_y;
}

/**
 * Get the Z-axis accelerometer value from the beacon data.
 * 
 * @return Z-axis accelerometer value, unit is mg.
 */
int16_t BLEKBeacon::getAccelerometer_Z()
{
    return kb.acc_z;
}
