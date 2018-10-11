#pragma once
#include "PositionUnits.h"
#include "OrientationUnits.h"

namespace PolhemusG4 {
	namespace Managed {
		/*
			Managed PNOFrame Struct
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

