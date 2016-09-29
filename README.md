# apu2-led
kernel driver for support PCEngine APU2 LEDs

import from https://daduke.org/linux/apu2/

PC Engines apu2
Hardware related stuff on the PC Engines apu2 board
apu2 front LEDs
kernel module to access the 3 front LEDs on the apu2 board and a small daemon to use LEDs 2 and 3 as network up/downstream indicators. Download is here, just run
# make
LED 1 as a load indicator:
# modprobe ledtrig_default_on; modprobe ledtrig_heartbeat
# echo heartbeat > /sys/class/leds/apu2:1/trigger
and run apuled like this:
# apu2led eth0 -c nrt -f
