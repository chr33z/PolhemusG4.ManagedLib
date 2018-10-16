#pragma once

namespace PolhemusG4 {
	namespace Managed {
		/*
			Managed FrameOfReference Struct
		*/
		public value class FrameOfReference
		{
		public:
			float x;
			float y;
			float z;

			float r1;
			float r2;
			float r3;
			float r4;

			bool valid;
		};
	}
}