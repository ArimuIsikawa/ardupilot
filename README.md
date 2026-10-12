# ArduPlane

This repository contains ArduPlane, the fixed-wing and QuadPlane firmware
from [ArduPilot](https://github.com/ArduPilot/ardupilot).

It retains Plane and QuadPlane tests, shared libraries, supported hardware
abstraction layers, simulators, build tools, bootloaders, IO firmware and
AP_Periph CAN firmware used with aircraft. Shared libraries and simulators
retain their upstream vehicle definitions and dependencies. Git submodules
are preserved unchanged.

## Build and test

From the repository root, with submodules initialized:

```sh
./waf configure --board sitl
./waf plane
Tools/autotest/autotest.py test.Plane
Tools/autotest/autotest.py test.QuadPlane
```

To build for hardware, replace `sitl` with the board name. List supported
boards with `./waf list_boards`.

## Documentation and contributions

- [Plane documentation](https://ardupilot.org/plane/)
- [Developer documentation](https://ardupilot.org/dev/)
- [Contribution guidelines](.github/CONTRIBUTING.md)
- [Code of conduct](CODE_OF_CONDUCT.md)
- [Upstream contributors](https://github.com/ArduPilot/ardupilot/graphs/contributors)

## License

GNU General Public License, version 3. See [COPYING.txt](COPYING.txt).
