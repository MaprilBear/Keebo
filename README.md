# Stoatboard


The Stoatboard is a USB and Bluetooth enabled wireless keyboard with an attached Color LCD

MCUs: TM4C123G and the BGM220P <br>
LCD: Adafruit 1.8" TFT LCD (Product ID 358) <br>

A bill of materials is included in this repository

## Credits
- April Douglas - Schematic and PCB Design, Firmware
- Matija Jankovic - Schematic and PCB Design, PCB Assembly
- Jenna May - Firmware
- Eric Wang - Case

## How to Use
- Fn+B - Toggle Bluetooth 
- Fn+Period - Cycle between apps 
- All keys are defined in Switch_Matrix.c

## Errata
1.  The bottom row of keys is unconnected to the main MCU
    -  Fix - Solder a wire from the bottom of the Left Arrow key diode to the TDO pin of the JTAG pinout. Holding down both shift keys will return TDO to it's JTAG confirguration and allow for flashing
2. There is crosstalk between the control, alt, shift, and windows keys in the bottom right. Win+RShift for example is detected as Ctrl+Win+RShift and Ctrl+Z is sometimes detected as Ctrl+Alt+Z
    - No Fix 
3. Bluetooth will sometimes "break" and cause the keyboard to become entirely unresponsive outside of the current app on the display
    -  Workaround - When using bluetooth make sure to toggle the blank screen "app" with Fn+Period, this will avoid the crash in most situations
    - Theorized Cause -  Crosstalk between LCD SPI traces and UART traces to the BGM220P
