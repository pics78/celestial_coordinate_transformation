#include<iostream>
#include<regex>
#include<unistd.h>
using namespace std;

#include "toolsForObs.hpp"

#define OPTION_EQU_TO_HOR 'h' // Equatorial coordinates -> Horizontal coordinates
#define OPTION_HOR_TO_EQU 'e' // Horizontal coordinates -> Equatorial coordinates

#define ERROR_MESSAGE1 "-Usage : %s [-h] or [-e] arg1 arg2\n"
#define ERROR_MESSAGE2 "-input mistake: 0 <= arg1 < 360\n"
#define ERROR_MESSAGE3 "-input mistake: -90 <= arg2 <= 90\n"
#define ERROR_MESSAGE4 "local_info.conf format is wrong.\n"

#define CONF_FILE_NAME "local_info.conf"

int main(int argc, char* argv[]) {
	int option = getopt(argc, argv, "he:");
	if (option != OPTION_EQU_TO_HOR && option != OPTION_HOR_TO_EQU) {
		printf(ERROR_MESSAGE1, argv[0]);
		return 1;
	}
	if (argc < 4) {
		printf(ERROR_MESSAGE1, argv[0]);
		return 1;
	}

	double x = atof(argv[2]);
	double y = atof(argv[3]);

	if (!(0.0 <= x && x < 360)) {
		printf(ERROR_MESSAGE2);
		return 1;
	} else if (!(-90.0 <= y && y <= 90.0)) {
		printf(ERROR_MESSAGE3);
		return 1;
	}

	double latitude  = -1;
	double longitude = -1;
	ifstream ifs;
	string buf;
	smatch results;
	ifs.open(CONF_FILE_NAME, ios::in);
	while (getline(ifs, buf)) {
		if (regex_search(buf, results, regex("^ *Latitude *= *([\\d\\.]+) *$"))) {
			latitude = stof(results[1].str());
		} else if (regex_search(buf, results, regex("^ *Longitude *= *([\\d\\.]+) *$"))) {
			longitude = stof(results[1].str());
		}
    }
	if (latitude == -1 || longitude == -1) {
		printf(ERROR_MESSAGE4);
		return 1;
	}

	NOWs now(latitude, longitude);
	double inpc[2] = {x, y};
	double outc[2] = {0, 0};

	if (option == OPTION_EQU_TO_HOR) {
		ConvEquToHor(now, inpc, outc);
		printf("A=%.3lf, h=%.3lf\n", outc[0], outc[1]);
	} else if (option == OPTION_HOR_TO_EQU) {
		ConvHorToEqu(now, inpc, outc);
		printf("RA=%.3lf, Dec=%.3lf\n", outc[0], outc[1]);
	}
	return 0;
}