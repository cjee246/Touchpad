/*!
	Touchpad_Snowflake.h - Library for interpreting touchpad inputs
	Created by Calvin Jee, April 14, 2019
	For Rita - happy birthday, my love
		
	Uses 2 Adafruit_MPR121 capacitive touch sensors
	See sensor layout in folder
*/

#ifndef Touchpad_Snowflake_h
#define Touchpad_Snowflake_h

#include "Arduino.h"
#include "Adafruit_MPR121.h"

class Touchpad_Snowflake {
	public:
		Touchpad_Snowflake();
		uint8_t getPosition(char axis);
		void initialize(char *map);
	private:
		const uint8_t MIN = 0;
		const uint8_t MAX = 255; 
		const double _PI = 3.14159;
		uint8_t _ic2addr1;
		uint8_t _ic2addr2;
		uint8_t sensorXY1[12][2];
		uint8_t sensorXY2[12][2];
		uint16_t sensorActive1;
		uint16_t sensorActive2;
		Adafruit_MPR121 cap1;
		Adafruit_MPR121 cap2;
		void mapping_default();
		void mapping_inner();
		void buffer();
		uint8_t sensor_rad2XY(double degrees, double len, char axis);
};

#endif