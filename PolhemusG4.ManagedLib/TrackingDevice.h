#pragma once

#pragma unmanaged
#include "NativeTrackingDevice.h"

#pragma managed
#include "msclr\marshal_cppstd.h"
#include "PNOFrame.h"
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

			bool SetPNOPositionUnits(PositionUnits unit) {
				return tracker->SetPNOPositionUnits((NativePositionUnits)unit);
			}

			PositionUnits GetPNOPositionUnits() {
				return (PositionUnits)tracker->GetPNOPositionUnits();
			}

			bool ResetPNOPositionUnits() {
				return tracker->ResetPNOPositionUnits();
			}

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
