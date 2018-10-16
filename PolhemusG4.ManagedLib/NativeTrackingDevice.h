#pragma once

#pragma unmanaged
#include <windows.h>
#include "tchar.h"
#include "PDI.h"
#include <string>
#include <vector>

using namespace std;

constexpr auto BUFFER_SIZE = 223200; // 15 seconds of 1 hub, 3 sensors at 120 hz = 15*124*120

enum NativePositionUnits {
	CM, FOOT, INCH, METER, MAX
};

enum NativeOrientationUnits {
	EULER_ANGLES, EULER_RADIAN, QUATERNION
};

struct NativePNOFrame {
	UINT32 hubID;
	UINT32 sensorID;
	UINT32 frameNumber;
	UINT32 DigIO;

	NativePositionUnits positionUnits;

	NativeOrientationUnits orientationUnits;

	float x;
	float y;
	float z;

	float r1;
	float r2;
	float r3;
	float r4;
};

struct NativeFrameOfReference {
	float x;
	float y;
	float z;

	float r1;
	float r2;
	float r3;
	float r4;

	bool valid;
};

class NativeTrackingDevice
{
public:
	NativeTrackingDevice();
	~NativeTrackingDevice();

	CPDIg4 tracker;

	bool isConnected;

private:
	byte g_pMotionBuf[BUFFER_SIZE];

	intptr_t systemID;

	intptr_t maxHubCount;

	int m_nHubIDs[G4_MAX_HUB_COUNT];

	UINT64 m_stamap[G4_STAMAP_BLOCK_COUNT];

	int lastHostFrameCount = 0;

	bool continousPollingRunning = false;

	NativeOrientationUnits orientationUnits;

	NativePositionUnits positionUnits;

	TCHAR sourceCalibrationFilePath[MAX_PATH + 1];

public:
	/*
		Initialize the tracker before connecting to a device.

		Params:
			string calibrationFilePath: Full path to a source calibration file
	*/
	bool Initialize(string calibrationFilePath);

	/*
		Connect to a G4 hub. Returns false if not successfull.
	*/
	bool Connect();

	/*
		Disconnect from a G4 hub. If false is returned, query the last device response for more information.
	*/
	bool Disconnect();

	/*
		Returns information about the connected hub. Internally calls the WhoAmI function.
	*/
	string GetTrackerInformation();

	string GetLastResultString();

	string GetSourceCalibrationFilePath();

	// =================================
	// Setting Position and Orientation

	bool SetPNOPositionUnits(NativePositionUnits unit);

	NativePositionUnits GetPNOPositionUnits();

	bool ResetPNOPositionUnits();

	bool SetPNOOrientationUnits(NativeOrientationUnits units);

	NativeOrientationUnits GetPNOOrientationUnits();

	bool ResetPNOOrientationUnits();

	void SetCommandPositionUnits(NativePositionUnits units);

	void SetCommandOrientationUnits(NativeOrientationUnits units);

	bool SetFrameOfReference(NativeFrameOfReference frameOfReference);

	NativeFrameOfReference GetFrameOfReference();

	bool ResetFrameOfReference();

	// END: Setting Position and Orientation

	// =================================
	// Reading Frames

	std::vector<NativePNOFrame*>* ReadSinglePNOFrame();

	/*
		New Frames are only reported when the position and orientation changes exceed the chosen increments

		Params:
			int hubID
			int sensorID
			float positionIncrement: 0 disables increment, -1 sets auto-increment
			float orientationIncrement: 0 disables increment, -1 sets auto-increment
			bool readSensorsAsBitmap: inteprete sensorID as a bitmap (for multi configuration)
	*/
	bool SetIncrement(int hubID, int sensorID, float positionIncrement, float orientationIncrement, bool readSensorsAsBitmap);

	/*
		Read the increment values for a specific hub and sensor
	*/
	bool GetIncrement(int hubID, int sensorID, float& positionIncrement, float& orientationIncrement);

	// END: Reading Frames


	//void GetStationMap();

	//void GetActiveHubs();

	//void GetSystemSensorMap();




private:
	std::vector<NativePNOFrame*>* ParseG4NativeFrame(PBYTE buffer, DWORD size);
};

