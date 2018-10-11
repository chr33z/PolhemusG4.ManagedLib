using System;
using System.Threading;
using PolhemusG4.Managed;

namespace PolhemusG4.ManagedLib.CSharpTest
{
    class Program
    {
        static void Main(string[] args)
        {
            TrackingDevice trackingDevice = new TrackingDevice();

            Console.WriteLine("Initialize Tracking Device...");
            trackingDevice.Initialize("C:\\default.g4c");

            if(!trackingDevice.Connect())
            {
                Console.WriteLine("Initialize Tracking Device...failed");
                Console.ReadKey();
                return;
            }

            Console.WriteLine("G4 Device Information:");
            Console.WriteLine(trackingDevice.GetTrackerInformation());

            int count = 1000;
            while(count-- > 0)
            {
                var frames = trackingDevice.ReadSinglePNOFrame();

                foreach(var frame in frames)
                {
                    Console.WriteLine(
                        $"[{frame.frameNumber} {frame.hubID} {frame.sensorID} | Position: {frame.x} {frame.y} {frame.z} | Rotation: {frame.r1} {frame.r2} {frame.r3}]");
                }

                Thread.Sleep(10);
            }

            trackingDevice.Disconnect();

            Console.ReadKey();
        }
    }
}
