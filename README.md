# Magic Radio
This crude radio transmitter uses a `100Mhz` crystal oscillator to transmit music in a form of Nokia ringtones (`RTTTL` sheets).

![Schematic](https://raw.githubusercontent.com/akoskovacs/MagicRadio/master/MagicRadio2.png)
The hardware is based around an `AVR ATtiny 2313` microcontroller. The chip unfortunately, doesn't include `USB` functionality,
so the `V-USB` library has to be used. This transforms the radio to a USB device capable of receiving USB commands.

![GUI](https://raw.githubusercontent.com/akoskovacs/MagicRadio/master/screenshot.png)

Qt5 Graphical User Interface for RTTTL playing in its natural habitat.
