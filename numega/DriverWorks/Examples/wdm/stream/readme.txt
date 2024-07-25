DriverWorks stream minidriver sample

To test the stream sample, follow these steps:

1. First build the driver, cstream.sys
2. Install the driver through the Add New Hardware Wizard:
   - Select the device type: "Sound, Video, and Game controllers"
   - Select "Have disk" and specify the directory where the INF and driver reside
3. Restart the computer.
4. Run the Filter Graph Editor utility (GRAPHEDT) that comes with the DirectX Media SDK
5. Insert a video capture element into the graph.
6. Insert a video rendering element into the graph, and connect its input to the
   output of the video capture

This should give you an "active movie" showing color bars.

