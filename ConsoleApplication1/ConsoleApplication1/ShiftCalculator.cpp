#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <map>
#include <list>

#define RPM_STEP 50

bool parseEngine(std::string file, std::map<int, double> &torqueCurve, int &redline){
	std::regex torqRegex("RPMTorque=\\(\\s(\\d+)\\.\\d+,\\s?-\\d+\\.\\d+,\\s(-?\\d+\\.\\d+)\\)");
	std::regex redlineRegex("LifetimeEngineRPM=\\((\\d+)\\.\\d+,\\s\\d+\\.\\d+\\)");
	std::smatch matches;

	std::map<int, double> tempTorqueCurve;

	std::ifstream infile(file);
	std::string line;
	while(std::getline(infile, line)){
		if(std::regex_search(line, matches, torqRegex)){
			int rpm;
			std::istringstream(matches[1]) >> rpm;
			double torque = stod(matches[2]);
			tempTorqueCurve[rpm] = torque;
		}else if(std::regex_search(line, matches, redlineRegex)){
			std::istringstream(matches[1]) >> redline;
		}
	}

	//Now we extrapolate the torque curve with 50 rpm intervals
	int lastRPM = -1;
	double lastTorque = 0.0;
	for(std::map<int, double>::iterator it=tempTorqueCurve.begin(); it!=tempTorqueCurve.end(); ++it){
		if(-1 == lastRPM){
			lastRPM = it->first;
			lastTorque = it->second;
			torqueCurve[it->first] = it->second;
			continue;
		}
		int steps = (it->first - lastRPM)/RPM_STEP;
		int remainder = (it->first - lastRPM)%RPM_STEP;
		double torqueStep = (it->second - lastTorque)/steps;

		int curRPM = lastRPM+RPM_STEP;
		for(int i=1; i<=steps; i++){
			torqueCurve[curRPM] = lastTorque+torqueStep*i;
			curRPM += RPM_STEP;		
		}

		lastRPM = curRPM;
		lastTorque = lastTorque+torqueStep*steps;
	}
	return true;	
}

bool parseGearbox(std::string grbFileName, std::string hdcFileName, std::vector<double> &gearRatios, int &gearCount){
	std::regex finalDrive("\\[FINAL_DRIVE\\]");
	std::regex ratioRegex("[R|r]atio=\\((\\d+),\\s?(\\d+)\\)");

	std::regex forwardGearsRegex("ForwardGears=(\\d+)");
	std::regex gearRegex("Gear(\\d+)Setting=(\\d+)");
	std::regex reverseRegex("ReverseSetting=(\\d+)");

	std::smatch matches;

	std::list<double> ratios;

	std::ifstream infile(grbFileName);
	std::string line;
	while(std::getline(infile, line)){
		if(std::regex_search(line, matches, finalDrive)){
			break;
		}else if(std::regex_search(line, matches, ratioRegex)){
			
			ratios.push_back(stod(matches[2])/stod(matches[1]));
		}
	}
	if(ratios.empty()){
		return false;
	}

	infile =  std::ifstream(hdcFileName);
	while(std::getline(infile, line)){
		if(std::regex_search(line, matches, forwardGearsRegex)){
			std::istringstream(matches[1]) >> gearCount;
		}else if(std::regex_search(line, matches, gearRegex)){
			int gear;
			std::istringstream(matches[1]) >> gear;
			int ratioNum;
			std::istringstream(matches[2]) >> ratioNum;
			
			std::list<double>::iterator it = ratios.begin();
			std::advance(it, ratioNum);
			gearRatios[gear] = *it;
		}else if(std::regex_search(line, matches, reverseRegex)){
			int ratioNum;
			std::istringstream(matches[2]) >> ratioNum;
			
			std::list<double>::iterator it = ratios.begin();
			std::advance(it, ratioNum);
			
			gearRatios[0] = *it; 
		}
	}	
}

int findClosestRPM(int rpm, std::map<int, double> &torqueCurve){
	int lowerBound = 0;
	int higherBound = (torqueCurve.end())->first;
	for(std::map<int, double>::iterator it = torqueCurve.begin(); it != torqueCurve.end(); ++it){
		if(rpm >= it->first){
			lowerBound = it->first;
		}else if(rpm < it->first){
			higherBound = it->first;
			break;
		}
	}
	return (rpm - lowerBound < higherBound - rpm)? lowerBound : higherBound;
}

int calculateOptimalShiftPoint(int curGear, int nextGear, std::map<int, double> &torqueCurve, int redline, std::vector<double> &gearRatios){
	double curGearRatio = gearRatios[curGear];
	double nextGearRatio = gearRatios[nextGear];

	double deltaTorquemin=999;
	int optimalRPM=redline;
	int maxRPM = findClosestRPM(redline, torqueCurve);
	for (int x=maxRPM; x>=torqueCurve[0]; x-=RPM_STEP) {
		double torquegearL = torqueCurve[x] * curGearRatio;
		double rpmH = x * nextGearRatio / curGearRatio;
		rpmH = findClosestRPM(rpmH, torqueCurve);
		double torquegearH = torqueCurve[rpmH] * nextGearRatio;
		double deltaTorque = torquegearL - torquegearH;
		if(deltaTorque < deltaTorquemin) {
			deltaTorquemin = deltaTorque;
			optimalRPM = x;
		}
	}

	return optimalRPM;
}

int main(int argc, char** argv){
	std::map<int, double> torqueCurve;
	int redline;
	std::vector<double> gearRatios(10); //No car should have more than 9 gears
	int gearCount;


	parseEngine(argv[1], torqueCurve, redline);
	/*for(std::map<int, double>::iterator it=torqueCurve.begin(); it!=torqueCurve.end(); ++it){
		std::cout << "RPM: "<< it->first << " Torque: " << it->second << std::endl;
	}*/
	std::cout << "Redline: " << redline << std::endl;
	
	parseGearbox(argv[2], argv[3], gearRatios, gearCount);
	/*std::cout << "Reverse ratio: " << gearRatios[0] << std::endl;
	for(int i=1; i<=gearCount; i++){
		std::cout << "Gear " << i << " ratio: " << gearRatios[i] << std::endl;
	}*/

	for(int i=1; i<gearCount; i++){
		std::cout << i << " --> " << i+1 << " : @" << calculateOptimalShiftPoint(i, i+1, torqueCurve, redline, gearRatios) << "RPM" << std::endl;
	}
	return 0;
}
