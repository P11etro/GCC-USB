# GCC-USB

GCCUSB is a firmware for the SparkFun Pro Micro RP2040, that can turn your Gamecube Controller into an USB-C controller.

## Table of Contents
* [Features](#features)
* [Installing the Firmware](#installing-the-firmware)
* [How to Build the Controller](#how-to-build-the-controller)
  *	[Requirements](#requirements)
  *	(Optional) [Left Z Button](#left-z-button)
  *	[Building](#building)
* [Usage](#usage)
* [Customization](#customization)
  * [Basic Modifications](#basic-modifications)
  * [Building the Firmware](#building-the-firmware)
* [Contributors](#contributors)
* [License](#license)

## Features

The controller has 4 different modes:

* DInput
* XInput
* Gamecube
* Keyboard

The first two modes make the controller work as a standard usb controller.

The Gamecube mode makes the controller work as a regular Gamecube Controller when plugged into a Gamecube, a Wii or an adapter.
It needs an USB-C to Gamecube cable (you can either buy it or make it yourself, learn more at the [How To Build the Controller](#how-to-build-the-controller) paragraph.

The Keyboard mode makes the controller work as a keyboard, so that it can be used with games that don’t support controllers.

The firmware is very easily customizable: it supports button remapping and additional buttons.
Learn more at the [Customization](#customization) paragraph.

## Installing the Firmware

I really recommend doing this step before everything, since it will be hard to access the board after soldering everything.
There are two ways to get the firmware: one is to just use the pre built one, the other is to build it yourself using Visual Studio Code and the PlatformIO extension.
Building it yourself allows you to customize it and change the pins to your liking.
You can learn how to build it yourself from the [Customization](#customization) paragraph.

After you get the firmware, plug the board in your computer while holding the “Bootsel” button. It should appear as an external storage device.
Drag and drop the firmware on the device: it should automatically disconnect.
If the status Led turned yellow it’s up and running.

## How to Build the Controller

### Requirements

- Gamecube Controller
- SparkFun Pro Micro RP2040
- Crane’s Model U board
- 1kΩ Resistor
- Soldering Equipment
- (Optional) Tactile Buttons. I suggest these, choose 6x6x5mm: https://it.aliexpress.com/item/32912259140.html.
- (Optional) Replacement Buttons (buy two sets if you want to add a second Z button).
- (Optional) USB-C to Gamecube Cable. You can also make it yourself from the one that you’ll desolder from the controller. If you choose to make it yourself, I suggest you follow this guide by 
Hand Held Legend: https://youtu.be/KawLnRKVswM.

### Left Z Button

Since this process requires trimming the motherboard, I advise to do this as the first thing (if you want a left Z button, otherwise go on).

I recommend you follow this guide, since it’s the one that I also followed: https://docs.google.com/document/d/16IPw5Dme_2h1pfnlSI5Uuv3VEL8JA3F4FGKHc8sD_Vg.

Stop following the guide when it comes to the wiring: I’ll explain how to do it later.

### Building

After disassembling the Controller, you will have to desolder the cable.

Then, it’s very important to make a hole in the shell for the USB C port to fit. You can use some files, and you should make something like this (yes, I know that mine is horrible):

<img width="400" alt="image" src="https://user-images.githubusercontent.com/128312132/234313287-81b267d3-4c84-40c9-b571-c70e17679ac7.png">

Now we have to solder everything like in this diagram (credits to [@Crane1195](https://github.com/Crane1195))

<img width="1000" alt="image" src="https://user-images.githubusercontent.com/128312132/234313427-39311c79-8826-4d0a-869f-885e049380d3.png">

 
Solder the blue, yellow and black lines how you normally would, using some soldering pins.

Then, solder the green and white lines using cables, and solder the yellow and black lines from the top of the soldering pins to the rp2040.

To solder the red lines, I advise to solder the one connected to pin 21 (GCC Input) from the back of the controller, and to solder the one connected to pin 2 (GC Console) from the Model U board.

Last, you will have to solder the resistor between the GCC Input pin (in the picture pin 21) and the V pin.
If you added a left Z button, you will have to solder it directly to the Pro Micro.
One pin has to be soldered to one of the GND pins, and the other one to one of the input pins (in the prebuilt firmware is set to be pin 6).

If you want, you can replace the Z button with a tactile one. It’s a drop-in replacement, so you just have to desolder the old one and solder the new one.

Now try to connect your controller and test it in DInput mode and, if you have the cable, in Gamecube mode.
I really recommend you test both, since Gamecube mode uses other pins.

If everything works fine, we can close it!

Since all of the wiring takes up a lot of space, I advise to remove the two brackets that cover the inside of the triggers.
 
  ![image](https://user-images.githubusercontent.com/128312132/234313937-7d4dec1d-76d6-4e75-a700-db75489f95e8.png)


It’s going to be tricky, but you will eventually manage to close it.

## Usage

The controller automatically understands when it’s connected to a Gamecube or an USB device.

If it’s connected to an USB device it will automatically enter DInput mode.

If it’s plugged in while holding the “X” button it will enter XInput mode.

Both modes should work with most emulators and games. There shouldn’t be that many differences, aside from the fact that XInput doesn’t work on Macs.
If one doesn’t work try the other one.

To enter Keyboard mode, plug in the controller while holding “Z”.

If you added a left Z button, it will work as an “R1” button in USB modes, and as another “Z” button in Gamecube mode.

All of the other buttons and sticks will work as you would expect.

If the controller is plugged in while holding “Start” it will enter bootsel mode, so that it’s possible to update the firmware without opening the controller.

## Customization

### Basic Modifications

The firmware is very easy to customize and tweak to your liking. 

To change one of the input pins, and increase or decrease the deadzone or sensitivity of the sticks, it’s as easy as changing one of these constants:
 
 <img width="264" alt="image" src="https://user-images.githubusercontent.com/128312132/234314900-0935f315-068e-44fe-b4e2-1eb460c38e88.png">


To change one of the buttons mapping in any of the modes, just change the gc_report buttons in any of the functions:
 
 <img width="359" alt="image" src="https://user-images.githubusercontent.com/128312132/234314928-0d9274af-8fb1-4509-85dd-74c389a380e3.png">


The same goes for keyboard mode:
 
 <img width="349" alt="image" src="https://user-images.githubusercontent.com/128312132/234314986-cf17974e-ed0a-48ac-a958-447929f1bc48.png">


The input mode gets chosen by a simple button press condition, and sets the “mode” variable to the corresponding number:

<img width="299" alt="image" src="https://user-images.githubusercontent.com/128312132/234315018-1d9670f9-d65c-443e-8366-9ce1542297b9.png">

<img width="142" alt="image" src="https://user-images.githubusercontent.com/128312132/234315039-2b3b976f-e9bb-4075-8c7b-be99ecde3875.png">

If you want to add an additional button, just solder the button to a pin (let's say pin 8), and assign it in all of the profiles:

<img width="530" alt="Schermata 2023-04-26 alle 12 15 56" src="https://user-images.githubusercontent.com/128312132/234545639-9ca55627-f288-477a-9b56-f05c9237421f.png">


<img width="944" alt="Schermata 2023-04-26 alle 12 13 57" src="https://user-images.githubusercontent.com/128312132/234545354-3211ece0-ddc0-40af-92c3-fa40e7d1e1e9.png">

 
If you want to add a mode or profile, just copy the base mode that you want to use (both in setup and loop), change the condition, the mode number and add the desired changes.

Example:

<img width="360" alt="image" src="https://user-images.githubusercontent.com/128312132/234315071-616127de-2cba-415e-b337-d583ae79e517.png">

### Building the Firmware

To build the customized firmware you will need Visual Studio Code and the [PlatformIO IDE](https://platformio.org/install/ide?install=vscode).

- Import the GCC-USB folder and open it with PlatformIO
- Wait for it to download all of the dependencies.
- Click Build (in the bottom left) and make sure everything compiles without errors.
- Connect the controller in bootsel mode.
- Drag and drop the file GCC-USB/.pio/build/promicro/firmware.uf2 onto the RPI-RP2 drive that comes up.

## Contributors
* **P11etro** - *Creator* - [@P11etro](https://github.com/P11etro)
* **Jonathan Haylett** - *Creator of the HayBox firmware and of many of the libraries used* - [@JonnyHaystack](https://github.com/JonnyHaystack)

## Acknowledgments
* [@JonnyHaystack](https://github.com/JonnyHaystack) - for helping me a lot from the very beginning of this project, and for the following libraries: TUGamepad, Adafruit_TinyUSB_XInput, joybus-pio.
* [@Crane1195](https://github.com/Crane1195) - for adding PlatformIO support for arduino-pico

## License
This project is licensed under the GNU GPL Version 3 - see the [LICENSE](LICENSE) file for details
