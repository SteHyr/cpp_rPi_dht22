#include <iostream>

#include <wiringPi.h>
#include <stdio.h> 
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include <string> 
#include <time.h>  
 
#ifndef JSON_LIB 
#define JSON_LIB
	#include "json/json.h"
#endif

#ifndef SENSOR_DHT22
#define SENSOR_DHT22
	#include "sensor_dht22.h"
#endif
 
int main() {

	Sensor_dht22 myDht22;
	Json::StyledWriter styledWriter;
    while(true)
    {
		std::cout << styledWriter.write(myDht22.getJson());
		delay(1000); // wait to refresh
    }
	return 1;
} 
