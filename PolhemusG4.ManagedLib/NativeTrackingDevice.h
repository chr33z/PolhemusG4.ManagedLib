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

class NativeTrackingDevice
{
public:
	NativeTrackingDevice();
	~NativeTrackingDevice();

	CPDIg4 tracker;

	bool isConnected;

	TCHAR sourceConfigurationFilePath[MAX_PATH + 1];

	NativeOrientationUnits orientationUnits;

	NativePositionUnits positionUnits;

private:
	byte g_pMotionBuf[BUFFER_SIZE];

	intptr_t systemID;

	intptr_t maxHubCount;

	int m_nHubIDs[G4_MAX_HUB_COUNT];

	UINT64 m_stamap[G4_STAMAP_BLOCK_COUNT];

	int lastHostFrameCount = 0;

	bool continousPollingRunning = false;

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

	bool SetPNOPositionUnits(NativePositionUnits unit);

	NativePositionUnits GetPNOPositionUnits();

	bool ResetPNOPositionUnits();

	//void SetPNOOrientationUnits();

	//void GetPNOOrientationUnits();

	//void ResetPNOOrientationUnits();

	std::vector<NativePNOFrame*>* ReadSinglePNOFrame();

	//void GetStationMap();

	//void GetActiveHubs();

	//void GetSystemSensorMap();

	string GetLastResultString();

private:
	std::vector<NativePNOFrame*>* ParseG4NativeFrame(PBYTE buffer, DWORD size);
};

