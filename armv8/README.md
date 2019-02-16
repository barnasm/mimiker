# Connection JLink to RPi
---

| JLink pin | JTAG function | RPi function | RPi pin |
| --------- | ------------- | -------- | ------- |
| 1 (right top)| Reference voltage | 3.3V | 17 |
| 3 | reset the target signal | GPIO22 | 15 |
| 5 | JTAG data input of the target | GPIO26 | 37 |
| 7 | mode set input of the target | GPIO27 | 13 |
| 9 | clock signal to target | GPIO25 | 22 |
| 11 | Return test clock signal from the target. Some targets must synchronize the JTAG inputs to internal clocks | GPIO23 | 16 |
| 13 | JTAG data output from the target | GPIO24 | 18 |
| 4 | GND | GND | 20 |

![](https://i.imgur.com/DOTWFmn.png)

## CONFIG FILES
---
### Raspberry Pi3 boot configuration
#### config.txt [[1]](https://elinux.org/RPiconfig) [[2]](https://www.raspberrypi.org/documentation/configuration/config-txt/)

* `kernel_old = 1` - load the kernel to the memory address 0x0
* `arm_64bit = 1` (or `arm_control=0x200`) - make the ARM start in 64-mode.
* `disable_commandline_tags = 1` - stop start.elf from filling in ATAGS (memory from 0x100) before launching the kernel (What are Atags? [[1]](https://jsandler18.github.io/extra/atags.html)[[2]](https://stackoverflow.com/questions/24448680/what-are-atags-for-a-device-that-uses-linux-kernel) )
* `enable_jtag_gpio=1` - selects Alt4 for GPIOs 22-27 - turn on jtag debugging

### OpenOCD configuration for aarch64: 
#### jlink.cfg
#### rpi3_64.cfg

### Boot files
#### kernel.img
#### start.elf
#### bootcode.bin

## OpenOCD
---
Run: `openocd -f jlink.cfg -f rpi3_64.cfg`.
Connect: `telnet localhost 4444` 
Basic commands:
* `reset init`: prepare CPU for debugging
* `targets`: list cores
* `targets CORE`: select core
* `halt`: stop execution
* `resume`: resume execution
* `reg`: show registers
* `reg REG VALUE`: set value of register
* `step`: single-step
* `bp ADDR SIZE [hw]`: set a breakpoint at ADDR (you must supply the opcode size)
* `rbp ADDR`: remove a breakpoint 
* `load_image FILENAME ADDR [TYPE]`: copy program to RAM at given address
* `poll`: show debugging state

## GDB 
---
Prepare GDB to remote debug ARMv8 device. All OpenOCD commands can be executed within GDB using `monitor` or in short `mon` e.g.: `mon targets`.
Basic configuration needed to start debugging baremetal program on Raspberry Pi3:
* `set architecture aarch64`: set target architecture
* `file rpi3-jtag.elf`: load program which is running on RPi and read its debug symbols
* `target extended-remote localhost:3333`: set connection to OpenOCD to debug specific device core. Notice that you do not have to use `target CORE` because connection to a given port define which core is debugging.
* `monitor reset init`: send command to the remote target
* `monitor load_image kernel.img 0x0 bin`: copy `kernel.img` and put it at `0x0` device address
* `monitor reg pc 0x0`: set program counter to `0x0` (same as `set $pc=0x0`)


