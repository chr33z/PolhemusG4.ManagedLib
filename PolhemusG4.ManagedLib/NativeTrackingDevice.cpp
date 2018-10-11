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
	_tcsncpy_s(sourceConfigurationFilePath, _countof(sourceConfigurationFilePath), file, _tcslen(file));

	return true;
}

bool NativeTrackingDevice::Connect()
{
	if (!tracker.CnxReady()) {
		if (tracker.ConnectG4(sourceConfigurationFilePath))
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

string NativeTrackingDevice::GetLastResultString()
{
	return string(tracker.GetLastResultStr());
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

