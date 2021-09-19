#include<fstream>
#include<math.h>
#include<time.h>

const double _DegToRad = M_PI/180;
const double _RadToDeg = 180/M_PI;

class NOWs {
	public:
		NOWs() {
			initialize();
		}
		NOWs(double latitude, double longitude) {
			initialize();
			Latitude  = latitude;
			Longitude = longitude;
		}

		double Latitude = 35.6809591;   // 東京駅の緯度(デフォルト)
		double Longitude = 139.7673068; // 東京駅の経度(デフォルト)
		double JST()  { return nJST;  } // 日本標準時[h]
		double UTC()  { return nUTC;  } // 協定世界時[h]
		double JD()   { return nJD;   } // JSTでのユリウス日[day]
		double MJD()  { return nMJD;  } // JSTでの修正ユリウス日[day]
		double UJD()  { return nUJD;  } // UTCでのユリウス日[day]
		double UMJD() { return nUMJD; } // UTCでの修正ユリウス日[day]
		double GST()  { return nGST;  } // グリニッジ標準時[h]
		double LST()  { return nLST;  } // 地方恒星時[h]

	private:
		int Year;
		int Month;
		int Day;
		int Hour;
		int Minute;
		int Second;
		double nJST;
		double nUTC;
		double nJD;
		double nMJD;
		double nUJD;
		double nUMJD;
		double nGST;
		double nLST;

		void initialize() {
			getTime();
			nJST  = JapanStandardTime();
			nUTC  = CoordinatedUniversalTime();
			nJD   = JulianDay("JST");
			nMJD  = nJD - 2400000.5;
			nUJD  = JulianDay("UTC");
			nUMJD = nUJD - 2400000.5;
			nGST  = GreenwichSiderealTime();
			nLST  = LocalSiderealTime();
		}

		// 現在の日付と時刻を取得
		void getTime() {
			time_t currentTime = time(NULL);
			struct tm *timeType = localtime(&currentTime);
			Year   = timeType-> tm_year + 1900;
			Month  = timeType-> tm_mon + 1;
			Day    = timeType-> tm_mday;
			Hour   = timeType-> tm_hour;
			Minute = timeType-> tm_min;
			Second = timeType-> tm_sec;
		}

		// JST [h]
		double JapanStandardTime() {
			return Hour + Minute/60.0 + Second/3600.0;
		}

		// UTC [h]
		double CoordinatedUniversalTime() {
			double UTC_Box = nJST - 9.0;
			if(UTC_Box < 0) UTC_Box += 24.0;
			return UTC_Box;
		}

		// ユリウス日 [day]
		double JulianDay(std::string which) {
			int mYear = Year, mMonth = Month;
			double JD_makingBox = 0;
			if(Month <= 2) {
				mYear -= 1;
				mMonth += 12;
			}
			JD_makingBox += (int)(365.25*mYear);
			JD_makingBox += (int)(mYear/400.0) - (int)(mYear/100.0);
			JD_makingBox += (int)(30.59*(mMonth-2.0)) + Day + 1721088.5;
			JD_makingBox += Hour/24.0 + Minute/1440.0 + Second/86400.0;

			if(which == "UTC") JD_makingBox -= 9.0 / 24.0;
			return JD_makingBox;
		}

		// グリニッジ標準時 [h]
		double GreenwichSiderealTime() {
			double D = nUMJD - 40000.0;
			double temp = 0.67239 + (1.00273781 * D);
			double GST_Box = 24.0 * (temp - (int)temp);
			return GST_Box;
		}

		// 地方恒星時 [h]
		double LocalSiderealTime() {
			double _longitude = Longitude/15.0;
			double LST_Box = nGST + _longitude;
			if(LST_Box < 0) LST_Box += 24.0;
			else if(LST_Box >= 24) LST_Box -= 24.0;
			return LST_Box;
		}
};

// 時角 [degree]
double HourAngle(NOWs thisTime, double RA) {
	double H = thisTime.LST()*15 - RA;
	return H >= 0 ? H : H + 360;
}

// 赤道座標 Equ(RA,Dec) -> 地平座標 Hor(A,h)
// 0 <= A <= 360, -90 <= h <= 90
void ConvEquToHor(NOWs thisTime, double* Equ, double* Hor) {
	double Dec = Equ[1]                      *_DegToRad;
	double Lat = thisTime.Latitude           *_DegToRad;
	double H   = HourAngle(thisTime, Equ[0]) *_DegToRad;

	double sinh = sin(Dec)*sin(Lat) + cos(Dec)*cos(Lat)*cos(H);
	double cosh = sqrt(1 - sinh*sinh);

	double sinA = -cos(Dec)*sin(H) / cosh;
	double cosA = (cos(Lat)*sin(Dec) - sin(Lat)*cos(Dec)*cos(H)) / cosh;

	Hor[0] = atan2(sinA, cosA)*_RadToDeg + 180.0;
	Hor[1] = atan2(sinh, cosh)*_RadToDeg;
}

// 地平座標 Hor(A,h) -> 赤道座標 Equ(RA,Dec)
// 0 <= RA <= 360, -90 <= Dec <= 90
void ConvHorToEqu(NOWs thisTime, double* Hor, double* Equ) {
	double A   = Hor[0]            *_DegToRad;
	double h   = Hor[1]            *_DegToRad;
	double Lat = thisTime.Latitude *_DegToRad;

	double sinDec = sin(h)*sin(Lat) - cos(h)*cos(Lat)*cos(A);
	double cosDec = sqrt(1 - sinDec*sinDec);

	double sinH = -cos(h)*sin(A) / cosDec;
	double cosH = -(sin(h)*cos(Lat) + cos(h)*sin(Lat)*cos(A)) / cosDec;
	double RA = thisTime.LST()*15 - (atan2(sinH, cosH)*_RadToDeg + 180.0);

	Equ[0] = RA >= 0 ? RA : RA + 360;
	Equ[1] = atan2(sinDec, cosDec)*_RadToDeg;
}