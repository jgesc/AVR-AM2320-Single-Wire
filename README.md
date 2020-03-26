# AVR-AM2320-Single-Wire
AM2320 temperature and humidity sensor interface for AVR ATtiny85 through Single Wire.

Uses PB3 by default.

Sensor values are stored in the byte array `msg` following the order they are sent by the sensor (Big Endian, that is, the most significant byte first):
* The first two bytes contain the humidity (in Big Endian)
* The second two bytes contain the temperature (also in Big Endian)
* The last byte contains the CRC

## Function overview
### init()
Called at the beginning of your program. It sets the CPU clock at 8MHz (required by the speed the sensor sends the data), sets the pull-up resistor on the PB3 pin an sets it into input mode, and enables edge interrupt for this pin.

### poll()
Wakes up the sensor and stores the answer into the array `tm`.

### parsetm()
Parses the timings stored in `tm` and stores the result in `msg`. It also checks the CRC.

Returns `1` if the CRC check is correct, else `0`.
