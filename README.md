# nrf52840-mdk-receiver

Description

Easy example for transmission a packet between two devices.

1. Install

https://github.com/mbedmicro/pyOCD

2. Install arm-none-eabi-gcc
```
apt-get install -y gcc-arm-none-eabi
```
3. Change HOME in Makefile
```
HOME := /root/iot/
```
4. Download the latest vesion of NRF SDK https://infocenter.nordicsemi.com/index.jsp?topic=%2Fstruct_sdk%2Fstruct%2Fsdk_nrf5_latest.html, unzip and rename nRF5_SDK_17

```
cd /root/iot/nrf52840-mdk-receiver/
mv nRF5_SDK_17.0.2_d674dde nRF5_SDK_17
SDK_ROOT := $(MDK_ROOT)/nRF5_SDK_17
```
5. Create symlinks 
```
cd /usr/bin/
for file in arm-none-eabi-*; do ln -s /usr/bin/$file /usr/local/gcc-arm-none-eabi-7-2018-q2-update/bin/$file; done
```
7. Make all and write code
```shell script
make all
sudo make flash
```
