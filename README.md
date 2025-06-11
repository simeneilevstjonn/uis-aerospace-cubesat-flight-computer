# UiS Aerospace CubeSat Flight Computer

## Installation

Clone the repository recursively
```sh
git clone --recursive git@github.com:simeneilevstjonn/uis-aerospace-cubesat-flight-computer.git
```

### Build C++ application
Install toolchain:
```sh
sudo apt-get install cmake ninja-build linux-headers-generic libi2c-dev
```

First, create a build directory:
```sh
mkdir build
cd build
```
Then, generate build files:
```sh
cmake ../src -G Ninja
```
Finally build:
```sh
ninja
```

### Service
```sh
sudo cp cubesat.service /etc/systemd/system
sudo systemctl enable cubesat
sudo systemctl daemon-reload
sudo systemctl start cubesat
```

### Device tree
The device tree tells the kernel what sensors are attached. Unfortunately, the stock Raspberry Pi OS only includes drivers for the ADT7410 and the ChipCap2, but all I2C sensors are defined.

Build the device tree using
```sh
dtc -@ -I dts -O dtb -o cubesat.dtbo cubesat.dts
```

Deploy using
```sh
sudo cp cubesat.dts /boot/firmware/overlays
```

The device tree must be enabled in `/boot/firmware/config.txt` by adding
```
dtoverlay=cubesat
```
also ensure that
```
dtparam=i2c_arm=on
```

is not commented out.

Add the following to `/etc/modules`
```
i2c-dev

# Adapter drivers
i2c_bcm2835
# Chip drivers
adt7410
```

Then reboot.

Temperature should now be available through the sysfs interface at `/sys/bus/i2c/devices/1-0048/hwmon/hwmon1/temp1_input`

