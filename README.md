# Micro Shell library for small embedded systems with basic VT100 support

## Description

MicroSH is platform independent command line shell for embedded systems, based on my [remaster of microRL library](https://github.com/dimmykar/microrl-remaster.git) by Helius. The library provides complete command interpretator for microRL command line to quickly and efficiently deploy the CLI on the target embedded device.

## Features

  - Platform independent command shell written in C language (C99)
  - Flexible library configuration
      * Turn on/off feature for add functional/decrease memory via `microsh_config.h` and `microsh_user_config.h` config files
  - No dynamic allocation
      * Maximum number of commands is assigned in configuration file
  - Console sessions feature (optional)
      * Use a shell in multi-user mode with a different set of commands 
  - Permissive Apache 2.0 license

## Getting started

### Install

This repository uses [Git Submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules) to bring in dependent components.

  - Open console and navigate to path in the system to clone repository to. Use command `cd <path_to_lib>`
  - Clone repository<br>
    To clone this repository using HTTPS:
    ```
    git clone https://github.com/dimmykar/microsh.git --recurse-submodules
    ```
    Using SSH:
    ```
    git clone git@github.com:dimmykar/microsh.git --recurse-submodules
    ```
    If you have downloaded repository without using the `--recurse-submodules` argument, you need to run:
    ```
    git submodule update --init --recursive
    ```
  - Checkout to `master` branch for _latest stable_ version or stay on `develop` branch for _latest development_ version

### Include library to the project

When the library is downloaded successfully, the next step is to add the library to the project, by means of source files to compiler inputs and header files in search path

  - Copy `microsh` folder to your project, it contains library files
  - Add `microsh/src/include` folder to include path of your toolchain. This is where C/C++ compiler can find the files during compilation process. Usually using `-I` flag
  - Add source files from `microsh/src/` folder to toolchain build. These files are built by C/C++ compiler
  - Copy `microsh/src/include/microsh/microsh_user_config_template.h` to project folder and rename it to `microsh_user_config.h`
  -  Additionally, the microSH library requires the microRL library, whose files follow the same `src/` and `src/include` pattern as the microSH library; its install instructions can be found [here](https://github.com/dimmykar/microrl-remaster#install). Note that you need also to configure microRL library using `microrl_user_config.h` file and include it to your project together with `microsh_user_config.h`.

### Library configuration

The list of configurations are available in the `microsh_config.h` file with default values. If any config value needs to be changed, specify values for these configuration values as follows:
  - Define in `microsh_user_config.h` used by microSH<br>
    __OR__
  - Pass as compile time preprocessor macros

User configurations file `microsh_user_config.h` is used to overwrite default configuration values defined in `microsh_config.h` file without overwrite the file.

If you prefer to avoid using user configurations file, application must define a global symbol `MICROSH_IGNORE_USER_CONFIGS` and `MICRORL_IGNORE_USER_CONFIGS`, visible across entire application. This can be achieved with `-D` compiler option.

## Minimal example

See examples in `examples` folder for minimal library usage.



Author: Dmitry Karasev aka dimmykar (karasevsdmitry@yandex.ru)<br>
15.01.2022
