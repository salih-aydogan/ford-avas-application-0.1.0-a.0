# ford-avas-application
Contains the build environment, source code and sw tests of the FORD AVAS project.

## Project Specifications:

**MCU:** FS32K116LAT0MLFT

**Toolchain:** gcc-arm-none-eabi v10.3.1

**SDK:** NXP S32SDK for S32K1xx RTM 4.0.3

This project is managed on [CodeBeamer](https://codebeamer.daiichi.com/project/39).

GCC is used as a compiler. 
[Toolchain Repo](https://github.com/DaiichiElectronic/ford-avas-toolchain) added this repo as subrepo.

You can clone this project directly using this command:

```
git clone --recursive https://github.com/DaiichiElectronic/ford-avas-application.git
```
or
```
git clone https://github.com/DaiichiElectronic/ford-avas-application.git
git submodule update --init --recursive
```
## Development
**C modules must be created by copying from `source/example/module.c-h` as a template**

## Building

### Tool Requirements
#### MSYS2
For Installation use this link and follow the installation steps: 
https://www.msys2.org/

Afterwards add this path (or if you used different path add it) to the Windows PATH environment variable;
```
C:\msys64\usr\bin
C:\msys64\mingw32\bin
```

Run build tasks have been created in Vscode.
Click **Terminal->Run Task...**

```
Build All
Rebuild All
Clean
Download Flash
```
* To use **Download Flash**, see [JLink Setup] topic.
* MinGW must be installed into MSYS2.

## Running Unit Tests

Compile tasks have been created in Vscode.
Click **Terminal->Run Task...**

```
Ceedling test all files(Unit)
Ceedling test current file(Unit)
Ceedling clobber(Unit)
```
You can also run **ceedling_run.bat** under `/test/unit/` folder to run tests and generate reports.

Required header files for mock generation of 3rd party libraries SHOULD be placed into `test/unit/test/support` directory.

## Running Integration Tests

Compile tasks have been created in Vscode.
Click **Terminal->Run Task...**

```
Ceedling test all files(Integration)
Ceedling test current file(Integration)
Ceedling clobber(Integration)
```

Run **ceedling_run.bat** under `/test/integration/` folder to run tests and generate reports.

Required header files for mock generation of 3rd party libraries SHOULD be placed into `test/integration/test/support` directory.

## Sending Reports to CodeBeamer

Run **trigger_TEST-TECHNIQUE_test_jenkins.bat** under `/test/TEST-TECHNIQUE/` folder to send reports. TEST-TECHNIQUE is one of unit, integration.


## Debug Requirements

*marus25.cortex-debug* Vscode Extension must be installed. Extension settings are made in vscode launch.

Click *Debug Microcontroller* in **Run->Start Debugging** 

*numaru.vscode-ceedling-test-adapter* Vscode Extension must be installed. Extension settings are made in vscode launch.

Click *Ceedling Test Explorer Debug* in **Run->Start Debugging** 

## Jlink Setup

J-Link selected as debug probes. Downloadfw scripts have been added accordingly and added **Debug Microcontroller** mode.

You can download the [J-Link driver](https://www.segger.com/downloads/jlink/JLink_Windows.exe)

You must add the JLink file path to the Windows PATH environment variable. The Jlink path going to be look like below path;
```
C:\Program Files(x86)\SEGGER\JLink_Vxxx
```

## Unit and Integration Test Setup

Unit test source files are located under the directory `test/unit/`.
Integration test source files are located under the directory `test/integration/`.

* Unit and Integration tests are done with **Ceedling**

You can find the necessary information for Ceedling installation from the link below.
[Ceedling Installation Documentation](https://static1.squarespace.com/static/549f45d6e4b037c1971053fd/t/5a5914d39140b75f8713debf/1515787476452/installation-guide-windows.pdf)

* for gcovr setup: `pip install gcovr` command should be run.

## Static Analysis

You can run the task below to perform static analysis.

```
QAC trigger
Check DCS Rules
```

### Dependencies
The dependencies required to run the tests are given below.
- Ruby
- GCC (MinGW for Windows and should be added in Windows Environment Path)
- gcovr

# License

Copyright (c) 2022 Daiichi Arge
