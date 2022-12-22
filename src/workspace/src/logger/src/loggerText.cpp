#include "loggerText.h"

LoggerText::LoggerText(std::string & logFolder, std::string separator):LoggerBase(logFolder), separator(separator){

}

LoggerText::~LoggerText(){
	finalize();
}

/*
 * Creates and opens the logger files with a proper timestamp and headers into a temporary location
 */
void LoggerText::init(){
	//Make sure environment is sane and that logging is enabled
	if(bootstrappedGnssTime && loggerEnabled){
		fileRotationMutex.lock();

		//Make sure the files are not already opened...
		if(!gnssOutputFile && !imuOutputFile && !sonarOutputFile && !lidarOutputFile){

			std::string dateString = TimeUtils::getStringDate();

			//Open GNSS file
			gnssFileName = dateString + "_gnss.txt";

			std::string gnssFilePath = tmpLoggingFolder + "/"  + gnssFileName;

			gnssOutputFile= fopen(gnssFilePath.c_str(),"a");

			if(!gnssOutputFile){
				fileRotationMutex.unlock();
				throw std::invalid_argument(std::string("Couldn't open GNSS log file ") + gnssFileName);
			}

			//Open IMU file
			imuFileName = dateString + "_imu.txt";

			std::string imuFilePath = tmpLoggingFolder + "/" + imuFileName;

			imuOutputFile = fopen(imuFilePath.c_str(),"a");

    		if(!imuOutputFile){
			fileRotationMutex.unlock();
	        	throw std::invalid_argument(std::string("Couldn't open IMU log file ") + imuFileName);
    		}

			//Open sonar file
			sonarFileName = dateString + "_sonar.txt";

			std::string sonarFilePath = tmpLoggingFolder + "/" + sonarFileName;

			sonarOutputFile = fopen(sonarFilePath.c_str(),"a");

	        if(!sonarOutputFile){
			fileRotationMutex.unlock();
			throw std::invalid_argument(std::string("Couldn't open sonar log file ") + sonarFileName);
	        }
			
			//Open lidar file
			lidarFileName = dateString + "_lidar.txt";

			std::string lidarFilePath = tmpLoggingFolder + "/" + lidarFileName;

			lidarOutputFile = fopen(lidarFilePath.c_str(),"a");

	        if(!lidarOutputFile){
			fileRotationMutex.unlock();
			throw std::invalid_argument(std::string("Couldn't open lidar log file ") + lidarFileName);
	        }
	        
	        
			
			fprintf(gnssOutputFile,"Timestamp%sLongitude%sLatitude%sEllipsoidalHeight%sStatus%sService\n",
									separator.c_str(),separator.c_str(),separator.c_str(),separator.c_str(),separator.c_str());
			fprintf(imuOutputFile,"Timestamp%sHeading%sPitch%sRoll\n",separator.c_str(),separator.c_str(),separator.c_str());
			fprintf(sonarOutputFile,"Timestamp%sDepth\n",separator.c_str());
			fprintf(lidarOutputFile,"Timestamp%sPoints\n",separator.c_str());
		}

		lastRotationTime = ros::Time::now();

		fileRotationMutex.unlock();
	}
}

/*
 * Closes the logging files and moves them to the web server's record directory to be accessible to download
 */

void LoggerText::finalize(){
	fileRotationMutex.lock();

        if(gnssOutputFile){
		//close
		fclose(gnssOutputFile);
		gnssOutputFile = NULL;

		//move
		std::string oldPath = tmpLoggingFolder + "/"  + gnssFileName;
		std::string newPath = outputFolder + "/" + gnssFileName;
		rename(oldPath.c_str(),newPath.c_str());
	}

        if(imuOutputFile){
		//close
		fclose(imuOutputFile);
		imuOutputFile = NULL;

		//move
		std::string oldPath = tmpLoggingFolder + "/"  + imuFileName;
		std::string newPath = outputFolder + "/" + imuFileName;
		rename(oldPath.c_str(),newPath.c_str());
	}

        if(sonarOutputFile){
		//close
		fclose(sonarOutputFile);
		sonarOutputFile = NULL;

		//move
		std::string oldPath = tmpLoggingFolder + "/"  + sonarFileName;
		std::string newPath = outputFolder + "/" + sonarFileName;
		rename(oldPath.c_str(),newPath.c_str());
	}
	
	if(lidarOutputFile){
		//close
		fclose(lidarOutputFile);
		lidarOutputFile = NULL;

		//move
		std::string oldPath = tmpLoggingFolder + "/"  + lidarFileName;
		std::string newPath = outputFolder + "/" + lidarFileName;
		rename(oldPath.c_str(),newPath.c_str());
	}
	
	fileRotationMutex.unlock();
}

/* Rotates logs based on time */
void LoggerText::rotate(){
	if(bootstrappedGnssTime && loggerEnabled){

		ros::Time currentTime = ros::Time::now();

		if(currentTime.toSec() - lastRotationTime.toSec() > logRotationIntervalSeconds){
			ROS_INFO("Rotating logs");
			//close (if need be), then reopen files.
			finalize();
			init();
		}
	}
}

void LoggerText::gnssCallback(const sensor_msgs::NavSatFix& gnss){
	if(!bootstrappedGnssTime && gnss.status.status >= 0){
		bootstrappedGnssTime = true;
	}

	if(bootstrappedGnssTime && loggerEnabled){

		if(!gnssOutputFile){
			init();
		}
		else{
			//We will clock the log rotation check on the GNSS data since it's usually the slowest at 1Hz, which is plenty
			//TODO: we could throttle this if CPU usage becomes an issue
			rotate();
		}

		uint64_t timestamp = TimeUtils::buildTimeStamp(gnss.header.stamp.sec, gnss.header.stamp.nsec);

		if(timestamp > lastGnssTimestamp){
			fprintf(gnssOutputFile,"%s%s%.10f%s%.10f%s%.3f%s%d%s%d\n",
				TimeUtils::getTimestampString(gnss.header.stamp.sec, gnss.header.stamp.nsec).c_str(),
				separator.c_str(),
				gnss.longitude,
				separator.c_str(),
				gnss.latitude,
				separator.c_str(),
				gnss.altitude,
				separator.c_str(),
				gnss.status.status,
				separator.c_str(),
				gnss.status.service
			);

			lastGnssTimestamp = timestamp; //XXX unused lastGnssTimestamp variable
		}
	}
}

void LoggerText::imuCallback(const sensor_msgs::Imu& imu){
	if(bootstrappedGnssTime && loggerEnabled){
		double heading = 0;
		double pitch   = 0;
		double roll    = 0;

		uint64_t timestamp = TimeUtils::buildTimeStamp(imu.header.stamp.sec, imu.header.stamp.nsec);

		if(timestamp > lastImuTimestamp){
			
			imuTransform(imu, roll, pitch, heading);
			
			fprintf(imuOutputFile,"%s%s%.3f%s%.3f%s%.3f\n",TimeUtils::getTimestampString(imu.header.stamp.sec, imu.header.stamp.nsec).c_str(),separator.c_str(),heading,separator.c_str(),pitch,separator.c_str(),roll);

			lastImuTimestamp = timestamp;
		}
	}
}

void LoggerText::sonarCallback(const geometry_msgs::PointStamped& sonar){
	if(bootstrappedGnssTime && loggerEnabled){
		uint64_t timestamp = TimeUtils::buildTimeStamp(sonar.header.stamp.sec, sonar.header.stamp.nsec);

		if(timestamp > lastSonarTimestamp){
			fprintf(sonarOutputFile,"%s%s%.3f\n",TimeUtils::getTimestampString(sonar.header.stamp.sec, sonar.header.stamp.nsec).c_str(),separator.c_str(),sonar.point.z);
			lastSonarTimestamp = timestamp;
		}
	}
}

void LoggerText::lidarCallBack(const sensor_msgs::PointCloud2& lidar){
	//ROS_INFO_STREAM("lidar callback : " << lidar.data.size() << "\n" );
	
	sensor_msgs::PointCloud lidarXYZ;
	sensor_msgs::convertPointCloud2ToPointCloud(lidar, lidarXYZ);
	
	std::vector<geometry_msgs::Point32> points = lidarXYZ.points;
	
	
	if(bootstrappedGnssTime && loggerEnabled){
		uint64_t timestamp = TimeUtils::buildTimeStamp(lidar.header.stamp.sec, lidar.header.stamp.nsec);
		
		if(timestamp > lastLidarTimestamp){
			fprintf(lidarOutputFile,"%s%s",TimeUtils::getTimestampString(lidar.header.stamp.sec, lidar.header.stamp.nsec).c_str(), separator.c_str());
			lastLidarTimestamp = timestamp;
			for(auto const& point : points){
    			fprintf(lidarOutputFile,"%f %f %f%s", point.x, point.y, point.z, separator.c_str());
    		}
    		fprintf(lidarOutputFile,"\n");
		}
	}
	
}
