#ifndef BME280Sensor_H_
#define BME280Sensor_H_

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

#include "bme280.h"
#include <wiringPiI2C.h>
#include <stdio.h>
#include <unistd.h>


int8_t getWeatherData(struct bme280_data *comp_data);


#ifdef __cplusplus
}
#endif /* End of CPP guard */
#endif /* BME280Sensor_H_ */