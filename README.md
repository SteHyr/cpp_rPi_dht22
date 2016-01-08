short c++ application to read raspberry Pi GPIO data from dht22 (temperatur&humidity sensor) 

compiler: g++ -std=c++0x rPi_dht.cpp jsoncpp.cpp -orPi_dht -L../wiringPI/wiringPi -L./json -lwiringPi
