//https://github.com/BoschSensortec/BME280_driver

#include "bme280Sensor.h"


struct bme280_dev dev;
typedef enum { false, true } bool;
bool bme280_initialised = false;
int fd = 0;

void user_delay_ms(uint32_t period)
{
	usleep(period * 1000);
}

int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
	// http://wiringpi.com/reference/i2c-library/
	int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */

	reg_data[0] = wiringPiI2CReadReg8(fd, reg_addr);
	if (reg_data[0] < 0)
	{
		return reg_data[0];
	}

	for (int index = 1; index < len; index++)
	{
		reg_data[index] = wiringPiI2CRead(fd);
		if (reg_data[index] < 0)
		{
			return reg_data[index];
		}
	}

	return rslt;
}

int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
	// http://wiringpi.com/reference/i2c-library/
	int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */

	rslt = wiringPiI2CWriteReg8(fd, reg_addr, reg_data[0]);

	for (int index = 1; index < len; index++)
	{
		rslt = wiringPiI2CWrite(fd, reg_data[index]);
	}

	return rslt;
}

int8_t init_bme280_normal_mode()
{
	int8_t rslt;
	uint8_t settings_sel;

	if (bme280_initialised)
	{
		return 0;
	}
	bme280_initialised = true;


	dev.dev_id = BME280_I2C_ADDR_PRIM;
	dev.intf = BME280_I2C_INTF;
	dev.read = user_i2c_read;
	dev.write = user_i2c_write;
	dev.delay_ms = user_delay_ms;

	fd = wiringPiI2CSetup(dev.dev_id);

	rslt = bme280_init(&dev);


	/* Recommended mode of operation: Indoor navigation */
	dev.settings.osr_h = BME280_OVERSAMPLING_1X;
	dev.settings.osr_p = BME280_OVERSAMPLING_16X;
	dev.settings.osr_t = BME280_OVERSAMPLING_2X;
	dev.settings.filter = BME280_FILTER_COEFF_16;
	dev.settings.standby_time = BME280_STANDBY_TIME_62_5_MS;

	settings_sel = BME280_OSR_PRESS_SEL;
	settings_sel |= BME280_OSR_TEMP_SEL;
	settings_sel |= BME280_OSR_HUM_SEL;
	settings_sel |= BME280_STANDBY_SEL;
	settings_sel |= BME280_FILTER_SEL;
	rslt = bme280_set_sensor_settings(settings_sel, &dev);
	rslt = bme280_set_sensor_mode(BME280_NORMAL_MODE, &dev);

	dev.delay_ms(100);

	return rslt;
}

int8_t getWeatherData(struct bme280_data *comp_data)
{
	int8_t rslt;

	rslt = init_bme280_normal_mode(dev);

	rslt = bme280_get_sensor_data(BME280_ALL, comp_data, &dev);

	return rslt;
}