/*
 * Copyright (c) 2014-2016 IBM Corporation.
 * All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of the <organization> nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "lmic.h"
#include "debug.h"
#include "bme280Sensor.h"
#include "CayenneLPP.h"

// sensor functions
extern void initsensor(void);
extern u2_t readsensor(void);

struct bme280_data comp_data;
float celsius;
float pressure;
float humidity;

//////////////////////////////////////////////////
// CONFIGURATION (FOR APPLICATION CALLBACKS BELOW)
//////////////////////////////////////////////////

// application router ID (LSBF)
static const u1_t APPEUI[8]  = { 0x43, 0x4B, 0x00, 0xF0, 0x7E, 0xD5, 0xB3, 0x70 };

// unique device ID (LSBF)
static const u1_t DEVEUI[8]  = { 0xDE, 0xF0, 0x6A, 0x7B, 0xA6, 0x6F, 0x3D, 0x00 };

// device-specific AES key (derived from device EUI)
static const u1_t DEVKEY[16] = { 0x7B, 0xDC, 0xB6, 0xA6, 0x6A, 0x51, 0xC5, 0x76, 0xF0, 0x1C, 0x74, 0x50, 0x21, 0x68, 0x58, 0x00 };

//////////////////////////////////////////////////
// APPLICATION CALLBACKS
//////////////////////////////////////////////////

// provide application router ID (8 bytes, LSBF)
void os_getArtEui(u1_t *buf)
{
    memcpy(buf, APPEUI, 8);
}

// provide device ID (8 bytes, LSBF)
void os_getDevEui(u1_t *buf)
{
    memcpy(buf, DEVEUI, 8);
}

// provide device key (16 bytes)
void os_getDevKey(u1_t *buf)
{
    memcpy(buf, DEVKEY, 16);
}

//////////////////////////////////////////////////
// MAIN - INITIALIZATION AND STARTUP
//////////////////////////////////////////////////

// initial job
static void initfunc(osjob_t *j)
{
    // intialize sensor hardware
    initsensor();
    // reset MAC state
    LMIC_reset();

    LMIC_selectSubBand(1);
    // start joining
    LMIC_startJoining();
    // init done - onEvent() callback will be invoked...
}

// application entry point
int main()
{

    initCayenne(51);

    osjob_t initjob;

    // initialize runtime env
    os_init();
    // initialize debug library
    debug_init();
    // setup initial job
    os_setCallback(&initjob, initfunc);
    // execute scheduled jobs and events
    os_runloop();
    // (not reached)
    return 0;
}

//////////////////////////////////////////////////
// UTILITY JOB
//////////////////////////////////////////////////

static osjob_t reportjob;

// report sensor value every minute
static void reportfunc(osjob_t *j)
{
    getWeatherData(&comp_data);
    celsius = comp_data.temperature;
    pressure = comp_data.pressure / 100.00;
    humidity = comp_data.humidity;

    resetCayenne();
    addTemperature(1, celsius);
    addBarometricPressure(2, pressure);(2, pressure);
    addRelativeHumidity(3, humidity);
    LMIC_setTxData2(1, getBuffer(), getSizeCayenne(), 0); // (port 1, 2 bytes, unconfirmed)
    printf("%0.2f, %0.2f, %0.2f, size(%d)\r\n", celsius, pressure, humidity, getSizeCayenne());


    // // read sensor
    // u2_t val = readsensor();
    // debug_val("val = ", val);
    // // prepare and schedule data for transmission
    // LMIC.frame[0] = val >> 8;
    // LMIC.frame[1] = val;
    // LMIC_setTxData2(1, LMIC.frame, 2, 0); // (port 1, 2 bytes, unconfirmed)

    
    // reschedule job in 60 seconds
    os_setTimedCallback(j, os_getTime() + sec2osticks(5), reportfunc);
}

//////////////////////////////////////////////////
// LMIC EVENT CALLBACK
//////////////////////////////////////////////////

void onEvent(ev_t ev)
{
    debug_event(ev);

    switch (ev)
    {

    // network joined, session established
    case EV_JOINED:
        // switch on LED
        debug_led(1);
        // kick-off periodic sensor job
        reportfunc(&reportjob);
        break;
    }
}
