# MicroSH library demo examples


## STM32 demo

To run the STM32 demo, import the `examples/stm32_example/STM32CubeIDE` project into the STM32CubeIDE workspace and build one of its Build Configurations. No changes are required to build the project.

If you have installed `binutils` and `gcc-arm-none-eabi`, then you can just run the command

```sh
$ make
```

in `examples/stm32_example` forder.

Connect USART to PC (usb-convertor work!) COM-port and open terminal like `minicom` with it COM-port

```sh
$ minicom -c on -D /dev/ttyUSB0 -b 115200
# -c on: turn on color in terminal
# -D /dev/ttyUSB0: your COM-port (virtual if usb-converter is used)
# -b 11520: USART baud rate
```
