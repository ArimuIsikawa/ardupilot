# ArduPlane ROS 2 integration

The `ardupilot_sitl` package builds and installs the Plane SITL executable,
launch files and Plane/QuadPlane defaults. The `ardupilot_msgs` package
contains the shared DDS message and service definitions.

Build from a ROS 2 workspace containing this repository:

```sh
colcon build --packages-up-to ardupilot_sitl
source install/setup.bash
ros2 launch ardupilot_sitl sitl.launch.py command:=arduplane model:=plane
```

DDS is enabled by default in the SITL package. Transport settings are in
`ardupilot_sitl/config/default_params/dds_serial.parm` and `dds_udp.parm`.

See the [DDS documentation](https://ardupilot.org/dev/docs/ros2.html)
for installation and transport setup.
