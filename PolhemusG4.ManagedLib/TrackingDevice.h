#pragma once

#pragma unmanaged
#include "NativeTrackingDevice.h"

#pragma managed
#include "msclr\marshal_cppstd.h"
#include "PNOFrame.h"
#include "PDIvec3.h"
#include "FrameOfReference.h"
#include "PositionUnits.h"
#include "OrientationUnits.h"

using System::String;

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;
using namespace msclr::interop;

namespace PolhemusG4 {
	namespace Managed {

		public ref class TrackingDevice
		{
		public:
			NativeTrackingDevice *tracker;

		public:
			TrackingDevice() { 
				tracker = new NativeTrackingDevice();
			}

			/*
				Initialize the tracker before connecting to a device.

				Params:
					string calibrationFilePath: Full path to a source calibration file
			*/
			bool Initialize(String^ calibrationFilePath) {
				std::string unmanagedFilePath = marshal_as<std::string>(calibrationFilePath);
				return tracker->Initialize(unmanagedFilePath);
			}

			/*
				Connect to a G4 hub. Returns false if not successfull.
			*/
			bool Connect() {
				return tracker->Connect();
			}


			/*
				Disconnect from a G4 hub. If false is returned, query the last device response for more information.
			*/
			bool Disconnect() {
				return tracker->Disconnect();
			}

			String^ GetTrackerInformation() {
				std::string info = tracker->GetTrackerInformation();
				return marshal_as<String^>(info);
			}

			/*
				Return the last result string from the tracking device. This string can for example contain error messages
			*/
			String^ GetLastResultString() {
				return gcnew String(tracker->GetLastResultString().c_str());
			}

			cli::array<Int32>^ GetActiveHubs() {
				int hubIDs[G4_MAX_HUB_COUNT];
				int hubCount = tracker->GetActiveHubs(hubIDs);

				cli::array<Int32>^ managedHubIDs = gcnew cli::array<Int32>(hubCount);
				for (size_t i = 0; i < hubCount; i++)
				{
					managedHubIDs[i] = hubIDs[i];
				}

				return managedHubIDs;
			}

			int GetActiveSensorCount() {
				return tracker->GetActiveSensorCount();
			}

			UINT32 GetHubSensorMap(int hub) {
				return tracker->GetHubSensorMap(hub);
			}

#pragma region Position and Orientation

			bool SetPNOPositionUnits(PositionUnits unit) {
				return tracker->SetPNOPositionUnits((NativePositionUnits)unit);
			}

			PositionUnits GetPNOPositionUnits() {
				return (PositionUnits)tracker->GetPNOPositionUnits();
			}

			bool ResetPNOPositionUnits() {
				return tracker->ResetPNOPositionUnits();
			}

			bool SetPNOOrientationUnits(OrientationUnits unit) {
				return tracker->SetPNOOrientationUnits((NativeOrientationUnits)unit);
			}

			NativeOrientationUnits GetPNOOrientationUnits() {
				return (NativeOrientationUnits)tracker->GetPNOOrientationUnits();
			}

			bool ResetPNOOrientationUnits() {
				return tracker->ResetPNOOrientationUnits();
			}

			void SetCommandPositionUnits(PositionUnits unit) {
				tracker->SetCommandPositionUnits((NativePositionUnits)unit);
			}

			void SetCommandOrientationUnits(OrientationUnits unit) {
				tracker->SetCommandOrientationUnits((NativeOrientationUnits)unit);
			}

			bool SetFrameOfReference(FrameOfReference frameOfReference) {  
				NativeFrameOfReference nativeFOR;

				nativeFOR.x = frameOfReference.x;
				nativeFOR.y = frameOfReference.y;
				nativeFOR.z = frameOfReference.z;
				nativeFOR.r1 = frameOfReference.r1;
				nativeFOR.r2 = frameOfReference.r2;
				nativeFOR.r3 = frameOfReference.r3;
				nativeFOR.r4 = frameOfReference.r4;

				return tracker->SetFrameOfReference(nativeFOR);
			}

			FrameOfReference GetFrameOfReference() {
				NativeFrameOfReference nativeFOR = tracker->GetFrameOfReference();

				FrameOfReference frameOfReference = FrameOfReference();

				if (nativeFOR.valid) {
					frameOfReference.x = nativeFOR.x;
					frameOfReference.y = nativeFOR.y;
					frameOfReference.z = nativeFOR.z;
					frameOfReference.r1 = nativeFOR.r1;
					frameOfReference.r2 = nativeFOR.r2;
					frameOfReference.r3 = nativeFOR.r3;
					frameOfReference.r4 = nativeFOR.r4;
					frameOfReference.valid = true;
				}
				return frameOfReference;
			}

			bool SetTipOffset(int hub, int sensor, float x, float y, float z) {
				return tracker->SetTipOffset(hub, sensor, x, y, z);
			}

			PDIvec3 GetTipOffset(int hub, int sensor) {
				NativePDIvec3 nativeTipOffset = tracker->GetTipOffset(hub, sensor);

				PDIvec3 tipOffset = PDIvec3();
				tipOffset.x = nativeTipOffset.x;
				tipOffset.y = nativeTipOffset.y;
				tipOffset.z = nativeTipOffset.z;

				return tipOffset;
			}

			bool ResetTipOffset(int hub, int sensor) {
				return tracker->ResetTipOffset(hub, sensor);
			}

#pragma endregion

#pragma region Reading Frames

			List<PNOFrame>^ ReadSinglePNOFrame() {
				vector<NativePNOFrame*>* frames = tracker->ReadSinglePNOFrame();

				List<PNOFrame>^ managedFrames = gcnew List<PNOFrame>();

				vector<NativePNOFrame*>::iterator itr;
				for (itr = frames->begin(); itr != frames->end(); itr++)
				{
					PNOFrame managedFrame = ConvertToManagedFrame(*itr);
					managedFrames->Add(managedFrame);
				}

				return managedFrames;
			}

			bool SetIncrement(int hubID, int sensorID, float positionIncrement, float orientationIncrement) {
				return tracker->SetIncrement(hubID, sensorID, positionIncrement, orientationIncrement, false);
			}

			bool GetIncrement(int hubID, int sensorID, [Out] float^% positionIncrement, [Out] float^% orientationIncrement) {
				// TODO test this! 
				
				float posInc;
				float oriInc;
				bool result = tracker->GetIncrement(hubID, sensorID, posInc, oriInc);

				positionIncrement = posInc;
				orientationIncrement = oriInc;

				return result;
			}

#pragma endregion

		private:
			PNOFrame ConvertToManagedFrame(NativePNOFrame* nativeFrame) {
				PNOFrame managedFrame = PNOFrame();

				managedFrame.hubID = nativeFrame->hubID;
				managedFrame.sensorID = nativeFrame->sensorID;
				managedFrame.DigIO = nativeFrame->DigIO;
				managedFrame.frameNumber = nativeFrame->frameNumber;
				managedFrame.x = nativeFrame->x;
				managedFrame.y = nativeFrame->y;
				managedFrame.z = nativeFrame->z;
				managedFrame.r1 = nativeFrame->r1;
				managedFrame.r2 = nativeFrame->r2;
				managedFrame.r3 = nativeFrame->r3;

				return managedFrame;
			}
		};
	}
}
