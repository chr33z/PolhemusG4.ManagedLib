#include "NativeTrackingDevice.h"

NativeTrackingDevice::NativeTrackingDevice()
{
}

NativeTrackingDevice::~NativeTrackingDevice()
{
}

bool NativeTrackingDevice::Initialize(string calibrationFilePath)
{
	tracker.Trace(TRUE, 9);
	tracker.SetPnoBuffer(g_pMotionBuf, BUFFER_SIZE);

	memset(&m_nHubIDs[0], 0, sizeof(INT)*G4_MAX_HUB_COUNT);
	memset(&m_stamap[0], 0, sizeof(UINT64)*G4_STAMAP_BLOCK_COUNT);

	const char* file = calibrationFilePath.c_str();
	_tcsncpy_s(sourceCalibrationFilePath, _countof(sourceCalibrationFilePath), file, _tcslen(file));

	return true;
}

bool NativeTrackingDevice::Connect()
{
	if (!tracker.CnxReady()) {
		if (tracker.ConnectG4(sourceCalibrationFilePath))
		{
			isConnected = true;
			return true;
		}
		else {
			isConnected = false;
			return false;
		}
	}
	else {
		// already connected
		isConnected = true;
		return true;
	}
}

bool NativeTrackingDevice::Disconnect()
{
	if (tracker.CnxReady()) {
		if (tracker.Disconnect())
		{
			isConnected = false;
			return true;
		}
		else {
			isConnected = false;
			return false;
		}
	}
	else {
		// not connected
		isConnected = false;
		return true;
	}
}

string NativeTrackingDevice::GetTrackerInformation()
{
	if (tracker.CnxReady()) {
		LPCTSTR	szWho;
		tracker.WhoAmIG4Sys(systemID, maxHubCount, szWho);
		return string(szWho);
	}
	else {
		return "";
	}

}

bool NativeTrackingDevice::SetPNOPositionUnits(NativePositionUnits unit)
{
	if (tracker.CnxReady()) {
		return tracker.SetPNOPosUnits((ePDIposUnits)unit);
	}
	return false;
}

NativePositionUnits NativeTrackingDevice::GetPNOPositionUnits()
{
	if (tracker.CnxReady()) {
		ePDIposUnits units;
		if (tracker.GetPNOPosUnits(units)) {
			return (NativePositionUnits)units;
		}
	}
	return NativePositionUnits::MAX;
}

bool NativeTrackingDevice::ResetPNOPositionUnits()
{
	if (tracker.CnxReady()) {
		return tracker.ResetPNOPosUnits();
	}
	else {
		return false;
	}
}

bool NativeTrackingDevice::SetPNOOrientationUnits(NativeOrientationUnits units)
{
	if (tracker.CnxReady()) {
		return tracker.SetPNOOriUnits((ePDIoriUnits)units);
	}
	return false;
}

NativeOrientationUnits NativeTrackingDevice::GetPNOOrientationUnits()
{
	if (tracker.CnxReady()) {
		ePDIoriUnits units;
		if (tracker.GetPNOOriUnits(units)) {
			return (NativeOrientationUnits)units;
		}
	}
}

bool NativeTrackingDevice::ResetPNOOrientationUnits()
{
	if (tracker.CnxReady()) {
		return tracker.ResetPNOOriUnits();
	}
	else {
		return false;
	}
}

string NativeTrackingDevice::GetLastResultString()
{
	return string(tracker.GetLastResultStr());
}

string NativeTrackingDevice::GetSourceCalibrationFilePath()
{
	return sourceCalibrationFilePath;
}

int NativeTrackingDevice::GetActiveHubs(int* hubIDs)
{
	int hubCount;
	tracker.GetActiveHubs(hubCount, hubIDs);
	return hubCount;
}

int NativeTrackingDevice::GetActiveSensorCount()
{
	int count;
	tracker.GetActiveSensorCount(count);
	return count;
}

UINT32 NativeTrackingDevice::GetHubSensorMap(int hub)
{
	DWORD map;
	tracker.GetHubSensorMap(hub, map);

	return map;
}

NativeFrameOfReference NativeTrackingDevice::GetFrameOfReference()
{
	NativeFrameOfReference frameOfReference;

	if (tracker.CnxReady()) {
		PDI7vec vec7;
		tracker.GetFrameOfRef(vec7);

		frameOfReference.x = vec7[0];
		frameOfReference.y = vec7[1];
		frameOfReference.z = vec7[2];
		frameOfReference.r1 = vec7[3];
		frameOfReference.r2 = vec7[4];
		frameOfReference.r3 = vec7[5];
		frameOfReference.r4 = vec7[6];

		frameOfReference.valid = true;
	}

	return frameOfReference;
}

bool NativeTrackingDevice::SetFrameOfReference(NativeFrameOfReference frameOfReference)
{
	if (tracker.CnxReady()) {
		PDI7vec v;

		v[0] = frameOfReference.x;
		v[1] = frameOfReference.y;
		v[2] = frameOfReference.z;
		v[3] = frameOfReference.r1;
		v[4] = frameOfReference.r2;
		v[5] = frameOfReference.r3;
		v[6] = frameOfReference.r4;

		return tracker.SetFrameOfRef(v);
	}
	return false;
}

bool NativeTrackingDevice::ResetFrameOfReference()
{
	if (tracker.CnxReady()) {
		return tracker.ResetFrameOfRef();
	}
	return false;
}

void NativeTrackingDevice::SetCommandPositionUnits(NativePositionUnits units)
{
	if (tracker.CnxReady()) {
		tracker.SetCmdPosUnits((ePDIposUnits)units);
	}
}

void NativeTrackingDevice::SetCommandOrientationUnits(NativeOrientationUnits units)
{
	if (tracker.CnxReady()) {
		tracker.SetCmdOriUnits((ePDIoriUnits)units);
	}
}

bool NativeTrackingDevice::SetTipOffset(int hub, int sensor, float x, float y, float z)
{
	if (tracker.CnxReady()) {
		PDIpos position;
		position[0] = x;
		position[1] = y;
		position[2] = z;

		return tracker.SetSTipOffset(hub, sensor, position);
	}
	return false;
}

NativePDIvec3 NativeTrackingDevice::GetTipOffset(int hub, int sensor)
{
	PDI3vec v;
	tracker.GetSTipOffset(hub, sensor, v);

	NativePDIvec3 tipOffset;
	tipOffset.x = v[0];
	tipOffset.y = v[1];
	tipOffset.z = v[2];

	return tipOffset;
}

bool NativeTrackingDevice::ResetTipOffset(int hub, int sensor)
{
	return tracker.ResetSTipOffset(hub, sensor, false);
}

std::vector<NativePNOFrame*>* NativeTrackingDevice::ReadSinglePNOFrame()
{
	PBYTE pBuf;
	DWORD dwSize;

	if (tracker.CnxReady()) {
		if (tracker.ReadSinglePnoBufG4(pBuf, dwSize))
		{
			return ParseG4NativeFrame(pBuf, dwSize);
		}
	}

	// in case of an error return nothing
	return new vector<NativePNOFrame*>();;
}

// Private Methods

bool NativeTrackingDevice::SetIncrement(int hubID, int sensorID, float positionIncrement, float orientationIncrement, bool readSensorsAsBitmap)
{
	if (tracker.CnxReady()) {
		return tracker.SetSIncrement(hubID, sensorID, positionIncrement, orientationIncrement, readSensorsAsBitmap);
	}
	return false;
}

bool NativeTrackingDevice::GetIncrement(int hubID, int sensorID, float & positionIncrement, float & orientationIncrement)
{
	if (tracker.CnxReady()) {
		return tracker.GetSIncrement(hubID, sensorID, positionIncrement, orientationIncrement);
	}
	return false;
}

vector<NativePNOFrame*>* NativeTrackingDevice::ParseG4NativeFrame(PBYTE buffer, DWORD size)
{
	std::vector<NativePNOFrame*>* frames = new vector<NativePNOFrame*>();

	if ((!buffer) || (!size))
	{
	}
	else
	{
		DWORD i = 0;
		LPG4_HUBDATA	pHubFrame;

		while (i < size)
		{
			pHubFrame = (LPG4_HUBDATA)(&buffer[i]);

			i += sizeof(G4_HUBDATA);

			UINT	nHubID = pHubFrame->nHubID;
			UINT	nFrameNum = pHubFrame->nFrameCount;
			UINT	nSensorMap = pHubFrame->dwSensorMap;
			UINT	nDigIO = pHubFrame->dwDigIO;

			UINT	nSensMask = 1;

			for (int j = 0; j < G4_MAX_SENSORS_PER_HUB; j++)
			{
				if (((nSensMask << j) & nSensorMap) != 0)
				{
					NativePNOFrame* frame = new NativePNOFrame;

					G4_SENSORDATA * pSD = &(pHubFrame->sd[j]);

					frame->hubID = pHubFrame->nHubID;
					frame->sensorID = pSD->nSnsID;
					frame->frameNumber = pHubFrame->nFrameCount;
					frame->DigIO = pHubFrame->dwDigIO;

					frame->x = pSD->pos[0];
					frame->y = pSD->pos[1];
					frame->z = pSD->pos[2];
					frame->r1 = pSD->ori[0];
					frame->r2 = pSD->ori[1];
					frame->r3 = pSD->ori[2];

					if (orientationUnits == NativeOrientationUnits::QUATERNION)
					{
						frame->r4 = pSD->ori[3];
					}

					frames->push_back(frame);
				}
			}

		} // end while dwsize
	}

	return frames;
}

