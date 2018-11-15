using System;
using System.Linq;
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

            Thread.Sleep(2000);

            var sensorCount = trackingDevice.GetActiveSensorCount();

            var hubIDs = trackingDevice.GetActiveHubs();

            if (hubIDs.Length > 0)
            {
                var sensorMap = trackingDevice.GetHubSensorMap(hubIDs[0]);
            }
            hubIDs.ToList().ForEach(hub => Console.WriteLine("active hub: " + hub));

            Console.WriteLine("G4 Device Information:");
            Console.WriteLine(trackingDevice.GetTrackerInformation());

            int count = 100;
            while(count-- > 0)
            {
                var frames = trackingDevice.ReadSinglePNOFrame();

                foreach(var frame in frames)
                {
                    Console.WriteLine(
                        $"[{frame.frameNumber} {frame.hubID} {frame.sensorID} | Position: {frame.x} {frame.y} {frame.z} | Rotation: {frame.r1} {frame.r2} {frame.r3}]");
                }

                Thread.Sleep(25);
            }


            //Tip Offset
            trackingDevice.SetTipOffset(0, 1, 0, 0, 10);
            var message = trackingDevice.GetLastResultString();
            Console.WriteLine(message);

            count = 100;
            while (count-- > 0)
            {
                var frames = trackingDevice.ReadSinglePNOFrame();

                foreach (var frame in frames)
                {
                    Console.WriteLine(
                        $"[{frame.frameNumber} {frame.hubID} {frame.sensorID} | Position: {frame.x} {frame.y} {frame.z} | Rotation: {frame.r1} {frame.r2} {frame.r3}]");
                }

                Thread.Sleep(25);
            }

            trackingDevice.Disconnect();

            Console.ReadKey();
        }
    }
}
