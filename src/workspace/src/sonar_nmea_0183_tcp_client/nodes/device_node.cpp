#include "ros/ros.h"
#include "sonar_nmea_0183_tcp_client/sonar_nmea_0183_tcp_client.h"

int main(int argc,char** argv){
	ros::init(argc, argv, "nmea_device");

	std::string device;

	//if no params present. use default values of 127.0.0.1:5000
	if(!ros::param::get("device", device)){
		device = "/dev/sonar";
	}

	//TODO: get useDepth/usePOsition/useAttitude from parameters
	DeviceNmeaClient nmea(device,true,false,false);
	nmea.run();
}
