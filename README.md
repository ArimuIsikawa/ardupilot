# ArduTailSitter

ArduTailSitter is a two-motor tailsitter project based on ArduPlane from
[ArduPilot](https://github.com/ArduPilot/ardupilot), with elevon control,
vertical takeoff and landing, and automatic transitions to fixed-wing flight.

It retains Plane and QuadPlane tests, shared libraries, supported hardware
abstraction layers, simulators, build tools, bootloaders, IO firmware and
AP_Periph CAN firmware used with aircraft. Shared libraries and simulators
retain their upstream vehicle definitions and dependencies. Git submodules
are preserved unchanged.

## Build and test

From the repository root, with submodules initialized:

```sh
./waf configure --board sitl
./waf tailsitter
Tools/autotest/autotest.py test.Plane
Tools/autotest/autotest.py test.QuadPlane
```

To build for hardware, replace `sitl` with the board name. List supported
boards with `./waf list_boards`.

The executable is `build/<board>/bin/ardutailsitter`. ChibiOS firmware files
use the same name, including `ardutailsitter.apj` and `ardutailsitter.bin`.
For Cube Orange Plus:

```sh
./waf configure --board CubeOrangePlus
./waf tailsitter
```

Run the two-motor SITL model with
`Tools/autotest/sim_vehicle.py -v ArduTailSitter -f plane-tailsitter-dual -L Dalby`.
The `ArduPlane/` source directory and Plane/QuadPlane test suites retain their
names. `./waf plane` and `sim_vehicle.py -v ArduPlane` remain compatible aliases.

## Documentation and contributions

- [Plane documentation](https://ardupilot.org/plane/)
- [Developer documentation](https://ardupilot.org/dev/)
- [Tailsitter AUTO mission and SITL examples](docs/tailsitter-auto.md)
- [Contribution guidelines](.github/CONTRIBUTING.md)
- [Code of conduct](CODE_OF_CONDUCT.md)
- [Upstream contributors](https://github.com/ArduPilot/ardupilot/graphs/contributors)

The tailsitter model, examples, tests, and build rename were prepared with AI assistance.

## License

GNU General Public License, version 3. See [COPYING.txt](COPYING.txt).
