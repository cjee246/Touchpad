/*!
	Touchpad_Snowflake.h - Library for interpreting touchpad inputs
	Created by Calvin Jee, April 14, 2019
	For Rita - happy birthday, my love
	
	Uses 2 Adafruit_MPR121 capacitive touch sensors
	See sensor layout in folder
*/

#include "Touchpad_Snowflake.h"

/*!
 *  @brief      Default constructor
 */
Touchpad_Snowflake::Touchpad_Snowflake () {}

/*!
 *  @brief      Define Adafruit MPR121 sensors for touchpad.
 *              Recommended addresses are:
 *					1: 0x5A (ADDR pin not connected)
 *					2: 0x5B (ADDR pin connected to 3vo)
 *  @param      ic2addr1
 *              the i2c address for the sensor 1 (see mapping) 
 *  @param      ic2addr2
 *              the i2c address for the sensor 2 (see mapping) 
 */
void Touchpad_Snowflake::initialize (uint8_t ic2addr1, uint8_t ic2addr2) {
		
	// declare sensors
	// Serial.println("Initializing MPR121 sensors..."); 
	_ic2addr1 = ic2addr1;
	_ic2addr2 = ic2addr2;
	cap1 = Adafruit_MPR121();
	cap2 = Adafruit_MPR121();
	
	// iniitialize sensors
	if (!cap1.begin(_ic2addr1)) 
	{
		// Serial.println("MPR121 sensor #1 not found.");
		while (1);
	}
	if (!cap2.begin(_ic2addr2)) 
	{
		// Serial.println("MPR121 sensor #2 not found.");
		while (1);
	}
	// Serial.println("Both MPR121 sensors found!");
	
	// define mapping of sensors
	mapping_default();
}

/*!
 *  @brief      Pre-define x and y values of sensors based on location
 *              (see sensor layout in folder).
 */
void Touchpad_Snowflake::mapping_default() {
	
	// define direction limits
	uint8_t LEFT1 = 4;
	uint8_t TOP1 = 11;
	uint8_t RIGHT2 = 5;
	uint8_t BOT2 = 11;
	
	// definitions for sensor positions {angle in degrees, unit length}
	// 0 degrees is downwards
	double sensorRad1[12][2] = {
		{-105, 1},			// 0 - bot-left dot 
		{-135, 1},			// 1 - outer bot-left arrow
		{-135, 0.5},    	// 2 - inner bot-left arrow
		{-165, 1},    		// 3 - left-bot dot
		{-180, 1},    		// 4 - outer left arrow
		{-180, 0.5},   		// 5 - inner left arrow
		{165, 1},    		// 6 - left-top dot
		{135, 1},    		// 7 - outer top-left arrow
		{135, 0.5},    		// 8 - inner top-left arrow
		{105, 1},    		// 9 - top-left dot
		{90, 0.5},    		// 10 - inner top arrow
		{90, 1}    			// 11 - outer top arrow
	};
	double sensorRad2[12][2] = {
		{75, 1},			// 0 - top-right dot 
		{45, 0.5},			// 1 - inner top-left arrow
		{45, 1},    		// 2 - outer top-left arrow
		{15, 1},    		// 3 - right-top dot
		{0, 0.5},    		// 4 - inner right arrow
		{0, 1},   			// 5 - outer right arrow
		{-15, 1},    		// 6 - right-bot dot
		{-45, 0.5},    		// 7 - inner bot-right arrow
		{-45, 1},    		// 8 - outer bot-right arrow
		{-75, 1},    		// 9 - bot-right dot
		{-90, 0.5},    		// 10 - inner bot arrow
		{-90, 1}    		// 11 - outer bot arrow
	};
	
	// convert to x and y coordinates on 8-bit scale
	for (int i=0; i<12; i++) {
		sensorXY1[i][0] = sensor_rad2XY(sensorRad1[i][0], sensorRad1[i][1], 'x');
		sensorXY1[i][1] = -sensor_rad2XY(sensorRad1[i][0], sensorRad1[i][1], 'y');
	}
	for (int i=0; i<12; i++) {
		sensorXY2[i][0] = sensor_rad2XY(sensorRad2[i][0], sensorRad2[i][1], 'x');
		sensorXY2[i][1] = -sensor_rad2XY(sensorRad2[i][0], sensorRad2[i][1], 'y');
	}
	
	// buffer for max coordinates
	sensorXY1[LEFT1][0] = MIN; sensorXY1[LEFT1][1] = MAX/2;
	sensorXY1[TOP1][0] = MAX/2; sensorXY1[TOP1][1] = MIN;
	sensorXY2[RIGHT2][0] = MAX; sensorXY2[RIGHT2][1] = MAX/2;
	sensorXY2[BOT2][0] = MAX/2; sensorXY2[BOT2][1] = MAX;
	
	return;
}

/*!
 *  @brief      Get average x or y position from touched sensors.
 *  @param      axis
 *              specify either 'x' or 'y' axis
 *  @returns    the average position as 8-bit unsigned int
 */
uint8_t Touchpad_Snowflake::getPosition(char axis) {

	// determine axis requested
	int j;
	switch (axis) {
		case 'x':
			j = 0;
			break;
		case 'y':
			j = 1;
			break;
	}
	
	// extract currently touched sensors
	uint16_t touch1 = cap1.touched();
	uint16_t touch2 = cap2.touched();	
	
	// determine average XY value of touched sensors
	int touchTotal = 0;
	int touchCount = 0;
	uint8_t touchAvg = 0;
	for (uint8_t i=0; i<12; i++)  {
		if (touch1 & bit(i))
		{
			touchTotal = touchTotal + sensorXY1[i][j];
			touchCount++;
		}
		if (touch2 & bit(i))
		{
			touchTotal = touchTotal + sensorXY2[i][j];
			touchCount++;
		}
	}
	
	// soft reset - Left, right, top, and bottom arrows upon release
	if ((touch1 & bit(4)) && (touch2 & bit(5)) && (touch1 & bit(11)) && (touch2 & bit(11)))
	{
		while ((touch1 & bit(4)) || (touch2 & bit(5)) || (touch1 & bit(11)) || (touch2 & bit(11)))
		{
			touch1 = cap1.touched();
			touch2 = cap2.touched();
		}
		delay(200);
		cap1.begin(_ic2addr1);
		cap2.begin(_ic2addr2);
	}
	
	// return average
	if (!touchCount) {
		touchAvg = MAX/2;
	}
	else {
		touchAvg = touchTotal / touchCount;	
	}
	return touchAvg;
	
}

/*!
 *	@brief		Prevent far proximity touches from influencing direction
 *				------- futurework...
 */
void Touchpad_Snowflake::buffer () {}

/*!
 *  @brief      Convert sensor locations from cylindrical to cartesian coordinates.
 *  @param      degrees
 *              angle position of sensor in degrees
 *  @param      len
 *              vector length of sensor (from 0 to 1)
 *  @param      axis
 *              specify either 'x' or 'y' axis
 *  @returns    the x or y location of the sensor
 */
uint8_t Touchpad_Snowflake::sensor_rad2XY(double degrees, double len, char axis) {
	
	// convert angle to radians
	double rad = degrees * (2 * _PI / 360);
	
	// determine x or y component of unit vector from angle
	double unitXY = 0;
	switch (axis) {
		case 'x':
			unitXY = cos(rad);
			break;
		case 'y':
			unitXY = sin(rad);
			break;
	}
	
	// calculate x or y position as 8-bit
	uint8_t pt = unitXY * len * (MAX+1)/2 + MAX/2;
	return pt;
}



	