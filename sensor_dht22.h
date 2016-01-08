// based on work from www.lolware.net & github.com/technion/lol_dht22

#ifndef JSON_LIB
#define JSON_LIB
#include "json/json.h"
#endif

#ifndef LOCKING_LIB
#define LOCKING_LIB
#include "locking.h"
#endif

#define MAXTIMINGS 85
static int DHTPIN = 7;
static int dht22_dat[5] = {0,0,0,0,0};
 
std::string int2string(const int& number)
{
	std::ostringstream oss;
	oss << number;
	return oss.str();
} 

class Sensor_dht22
{
	public:
		Sensor_dht22();
 		Json::Value getJson();		
		
	private:
		int lockfd;
		float temp;
		float humi;
		
		uint8_t sizecvt(const int read);
		int read_dht22_dat();
		int updateData();
		int updateTime();
		Json::Value createJsonMsg();
		
		time_t rawtime;
		struct tm * timeinfo;

};

Sensor_dht22::Sensor_dht22()
{
	temp = 0.0;
	humi = 0.0;
}; 

Json::Value Sensor_dht22::getJson() 
{
	updateData();
	updateTime();
	
	Json::Value jsonMsgLocal = createJsonMsg(); 

	return jsonMsgLocal;
};

Json::Value Sensor_dht22::createJsonMsg()
{
	Json::Value jsonMsgLocal;

    time_t ltime;
    struct tm *Tm;
 
    ltime=time(NULL);
    Tm=localtime(&ltime);
  
	std::string timeStr = 
		      int2string(Tm->tm_year+1900) + 
		" " + int2string(Tm->tm_mon+1) +
		" " + int2string(Tm->tm_mday)+
		" " + int2string(Tm->tm_hour)+
		":" + int2string(Tm->tm_min)+
		":" + int2string(Tm->tm_sec);


	jsonMsgLocal["temperature"] = temp;
	jsonMsgLocal["humidity"] = humi;
	jsonMsgLocal["timestamp"] = timeStr;

	return jsonMsgLocal;
};

int Sensor_dht22::updateTime()
{
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	return 1;
};

int Sensor_dht22::updateData()
{
	 
	lockfd = open_lockfile(LOCKFILE);

	if (wiringPiSetup () == -1)
		exit(EXIT_FAILURE) ;

	if (setuid(getuid()) < 0)
	{
		perror("Dropping privileges failed\n");
		exit(EXIT_FAILURE);
	}

	while (read_dht22_dat() == 0)
	{ 
		delay(100);
	}

	close_lockfile(lockfd);
	return 1;
};


uint8_t Sensor_dht22::sizecvt(const int read)
{
  /* digitalRead() and friends from wiringpi are defined as returning a value
  < 256. However, they are returned as int() types. This is a safety function */

	if (read > 255 || read < 0)
	{
		printf("Invalid data from wiringPi library\n");	
		exit(EXIT_FAILURE);
	}
	return (uint8_t)read;
};	

int Sensor_dht22::read_dht22_dat()
{
  uint8_t laststate = HIGH;
  uint8_t counter = 0;
  uint8_t j = 0, i;

  dht22_dat[0] = dht22_dat[1] = dht22_dat[2] = dht22_dat[3] = dht22_dat[4] = 0;

  // pull pin down for 18 milliseconds
  pinMode(DHTPIN, OUTPUT);
  digitalWrite(DHTPIN, HIGH);
  delay(10);
  digitalWrite(DHTPIN, LOW);
  delay(18);
  // then pull it up for 40 microseconds
  digitalWrite(DHTPIN, HIGH);
  delayMicroseconds(40); 
  // prepare to read the pin
  pinMode(DHTPIN, INPUT);

  // detect change and read data
  for ( i=0; i< MAXTIMINGS; i++) {
    counter = 0;
    while (sizecvt(digitalRead(DHTPIN)) == laststate) {
      counter++;
      delayMicroseconds(1);
      if (counter == 255) {
        break;
      }
    }

    laststate = sizecvt(digitalRead(DHTPIN));

    if (counter == 255) break;

    // ignore first 3 transitions
    if ((i >= 4) && (i%2 == 0)) {
      // shove each bit into the storage bytes
      dht22_dat[j/8] <<= 1;
      if (counter > 16)
        dht22_dat[j/8] |= 1;
      j++;
    }
  }

  // check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
  // print it out if data is good
  if ((j >= 40) && 
      (dht22_dat[4] == ((dht22_dat[0] + dht22_dat[1] + dht22_dat[2] + dht22_dat[3]) & 0xFF)) ) {
        float t, h;
        h = (float)dht22_dat[0] * 256 + (float)dht22_dat[1];
        h /= 10;
        t = (float)(dht22_dat[2] & 0x7F)* 256 + (float)dht22_dat[3];
        t /= 10.0;
        if ((dht22_dat[2] & 0x80) != 0)  t *= -1;

		temp = t;
		humi = h;

		return 1;
  } 
  return 0; 
}; 
