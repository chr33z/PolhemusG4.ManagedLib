#pragma once
#include "PositionUnits.h"
#include "OrientationUnits.h"

namespace PolhemusG4 {
	namespace Managed {
		/*
			Managed PNOFrame Struct:

			When reading euler angles the values r1, r2, r3 refer to azimuth, elevation and roll of the sensor.
			When reading quaternions r1, r2, r3, r4 refer to the components of a quaternion
		*/
		public value class PNOFrame
		{
		public:
			int hubID;
			int sensorID;
			int frameNumber;
			int DigIO;

			PositionUnits positionUnits;

			OrientationUnits orientationUnits;

			float x;
			float y;
			float z;

			float r1;
			float r2;
			float r3;
			float r4;
		};
	}
}

