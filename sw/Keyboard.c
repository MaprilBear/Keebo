//*****************************************************************************
//
// usb_dev_keyboard.c - Main routines for the keyboard example.
//
// Copyright (c) 2011-2020 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 2.2.0.295 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "usblib/usblib.h"

#include "usblib/device/usbdevice.h"
#include "usblib/usbhid.h"
#include "usblib/device/usbdhid.h"
#include "usblib/device/usbdhidkeyb.h"
#include "./drivers/buttons.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"
#include "usb_keyb_structs.h"
#include "usblib/usbhid.h"

#include "drivers/rgb.h"
#include "inc/CortexM.h"
#include "inc/UART1.h"
#include "inc/UART.h"
#include "Switch_Matrix.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>USB HID Keyboard Device (usb_dev_keyboard)</h1>
//!
//! This example turns the EK-TM4C123GXL LaunchPad into a USB keyboard
//! supporting the Human Interface Device class.  When either the SW1/SW2
//! push button is pressed, a sequence of key presses is simulated to type a
//! string.  Care should be taken to ensure that the active window can safely
//! receive the text; enter is not pressed at any point so no actions are
//! attempted by the host if a terminal window is used (for example).  The
//! status LED is used to indicate the current Caps Lock state and is updated
//! in response to any other keyboard attached to the same USB host system.
//!
//! The device implemented by this application also supports USB remote wakeup
//! allowing it to request the host to reactivate a suspended bus.  If the bus
//! is suspended (as indicated on the application display), pressing the
//! push button will request a remote wakeup assuming the host has not
//! specifically disabled such requests.
//
//*****************************************************************************

// connected to USB host
volatile bool g_bConnected = false;

// is the USB bus suspended
volatile bool g_bSuspended = false;

// Have we enabled bluetooth
volatile bool bluetoothOn = false;

//*****************************************************************************
//
// Add the supplied usage code to the list of keys currently in the pressed
// state.
//
// \param ui8UsageCode is the HID usage code of the newly pressed key.
//
// This function adds the supplied usage code to the global list of keys which
// are currently pressed (assuming it is not already noted as pressed and that
// there is space in the list to hold the new information).  The return code
// indicates success if the list did not overflow and failure if the list
// already contains as many pressed keys as can be reported.
//
// \return Returns \b true if the usage code was successfully added to the
// list or \b false if there was insufficient space to hold the new key
// press (in which case the caller should report a roll over error to the
// host).
//
//*****************************************************************************
static bool AddKeyToPressedList(tHIDKeyboardInstance *psInst, uint8_t ui8UsageCode) {
  uint32_t ui32Loop;
  bool bRetcode;

  //
  // Assume all is well until we determine otherwise.
  //
  bRetcode = true;

  //
  // Look through the list of existing pressed keys to see if the new one
  // is already there.
  //
  for (ui32Loop = 0; ui32Loop < (uint32_t)psInst->ui8KeyCount; ui32Loop++)
  {
    //
    // Is this key already included in the list of keys in the pressed
    // state?
    //
    if (ui8UsageCode == psInst->pui8KeysPressed[ui32Loop])
    {
      //
      // Yes - drop out.
      //
      break;
    }
  }

  //
  // If we exited the loop at the end of the existing key presses, this
  // key does not exist already so add it if space exists.
  //
  if (ui32Loop >= psInst->ui8KeyCount)
  {
    if (psInst->ui8KeyCount < KEYB_MAX_CHARS_PER_REPORT)
    {
      //
      // We have room so store the new key press in the list.
      //
      psInst->pui8KeysPressed[psInst->ui8KeyCount] = ui8UsageCode;
      psInst->ui8KeyCount++;
      bRetcode = true;
    }
    else
    {
      //
      // We have no room for the new key - declare a rollover error.
      //
      bRetcode = false;
    }
  }

  return (bRetcode);
}

//*****************************************************************************
//
// Remove the supplied usage code from the list of keys currently in the
// pressed state.
//
// \param ui8UsageCode is the HID usage code of the newly released key.
//
// This function removes the supplied usage code from the global list of keys
// which are currently pressed.  The return code indicates whether the key was
// found in the list.  On exit, the list has been cleaned up to ensure
// that all key presses are contiguous starting at the first entry.
//
// \return Returns \b true if the usage code was found and removed from the
// list or \b false if the code was not found.  The caller need not pass a new
// report to the host if \b false is returned since the key list has not
// changed.
//
//*****************************************************************************
static bool RemoveKeyFromPressedList(tHIDKeyboardInstance *psInst, uint8_t ui8UsageCode) {
  uint32_t ui32Loop;
  uint32_t ui32Pos;

  //
  // Keep the compiler happy by setting ui32Pos to something.
  //
  ui32Pos = 0;

  //
  // Find the usage code in the current list.
  //
  for (ui32Loop = 0; ui32Loop < KEYB_MAX_CHARS_PER_REPORT; ui32Loop++)
  {
    if (psInst->pui8KeysPressed[ui32Loop] == ui8UsageCode)
    {
      ui32Pos = ui32Loop;
      break;
    }
  }

  //
  // If we dropped out at the end of the loop, we could not find the code so
  // just return false.
  //
  if (ui32Loop == KEYB_MAX_CHARS_PER_REPORT)
  {
    return (false);
  }

  //
  // Now shuffle all the values to the right of the usage code we found
  // down one position to fill the gap left by removing it.
  //
  for (ui32Loop = (ui32Pos + 1); ui32Loop < KEYB_MAX_CHARS_PER_REPORT;
       ui32Loop++)
  {
    psInst->pui8KeysPressed[ui32Loop - 1] =
        psInst->pui8KeysPressed[ui32Loop];
  }

  //
  // Clear the last entry in the array and adjust the number of keys in the
  // array.
  //
  psInst->pui8KeysPressed[KEYB_MAX_CHARS_PER_REPORT - 1] =
      HID_KEYB_USAGE_RESERVED;
  psInst->ui8KeyCount--;

  //
  // Tell the caller we were successful.
  //
  return (true);
}

// Geneare the INPUT report based on the currently pressed keys
void _GenerateReport(void *pvKeyboardDevice, uint8_t report[8]) {
  tHIDKeyboardInstance *psInst;
  tUSBDHIDKeyboardDevice *psHIDKbDevice;
  tUSBDHIDDevice *psHIDDevice;

  psHIDKbDevice = (tUSBDHIDKeyboardDevice *)pvKeyboardDevice;

  //
  // Get a pointer to the HID device data.
  //
  psHIDDevice = &psHIDKbDevice->sPrivateData.sHIDDevice;

  //
  // Get a pointer to our instance data
  //
  psInst = &psHIDKbDevice->sPrivateData;

  uint32_t ui32Loop, ui32Count;

  //
  // Assume all is well until we determine otherwise.
  //
  bool bRetcode = true;

  for (int i = 0; i < 8; i++){
    report[i] = psInst->pui8Report[i];
  }

  // Build LED state changes
  report[2] = psInst->ui8LEDStates;

  //
  // Build the report from the current list of keys.  If we added a key
  // and got a bad return code indicating a roll over error, we need to
  // send a roll over report
  //
  for (ui32Loop = 0; ui32Loop < KEYB_MAX_CHARS_PER_REPORT; ui32Loop++)
  {
    report[2 + ui32Loop] = (bRetcode ? psInst->pui8KeysPressed[ui32Loop] : HID_KEYB_USAGE_ROLLOVER);
  }
}


// Send the provided INPUT report over USB
uint32_t _SendKeyReport(void *pvKeyboardDevice, uint8_t report[8]) {
  tHIDKeyboardInstance *psInst;
  tUSBDHIDKeyboardDevice *psHIDKbDevice;
  tUSBDHIDDevice *psHIDDevice;

  uint32_t ui32Loop, ui32Count;

  psHIDKbDevice = (tUSBDHIDKeyboardDevice *)pvKeyboardDevice;

  //
  // Get a pointer to the HID device data.
  //
  psHIDDevice = &psHIDKbDevice->sPrivateData.sHIDDevice;

  //
  // Get a pointer to our instance data
  //
  psInst = &psHIDKbDevice->sPrivateData;

  for (int i = 0; i < 8; i++)
  {
    psInst->pui8Report[i] = report[i];
  }

  //
  // Only send a report if the transmitter is currently free.
  //

  if (USBDHIDTxPacketAvailable((void *)psHIDDevice))
  {
    //
    // Send the report to the host.
    //
    psInst->eKeyboardState = HID_KEYBOARD_STATE_SEND;
    ui32Count = USBDHIDReportWrite((void *)psHIDDevice,
                                   psInst->pui8Report, KEYB_IN_REPORT_SIZE,
                                   true);

    //
    // Did we schedule a packet for transmission correctly?
    //
    if (!ui32Count)
    {
      //
      // No - report the error to the caller.
      //
      return (KEYB_ERR_TX_ERROR);
    }
  }
  else
  {
    //
    // We can't send the report immediately so mark the instance so that
    // it is sent next time the transmitter is free.
    //
    psInst->bChangeMade = true;
  }
}

// Toggle bluetooth, duh :P
void ToggleBluetooth(){
  if (bluetoothOn){
    // disable the flag and tell the bluetooth chip to shutdown (active low)
    bluetoothOn = false;
    GPIO_PORTD_DATA_R &= ~0x1;
  } else {
    // enable the flag and wakeup the bluetooth chip
    bluetoothOn = true;
    GPIO_PORTD_DATA_R |= 0x1;
  }
}

// last report so we only tell the bluetooth chip when the currently pressed keys changes
uint8_t lastReport[8];

// Send the provided report to the bluetooth chip, but only if something has changed
void _SendKeyReportBluetooth(void *pvKeyboardDevice, uint8_t report[8]) {
  tHIDKeyboardInstance *psInst;
  tUSBDHIDKeyboardDevice *psHIDKbDevice;
  tUSBDHIDDevice *psHIDDevice;

  uint32_t ui32Loop, ui32Count;

  //
  // Assume all is well until we determine otherwise.
  //
  bool bRetcode = true;

  psHIDKbDevice = (tUSBDHIDKeyboardDevice *)pvKeyboardDevice;

  //
  // Get a pointer to the HID device data.
  //
  psHIDDevice = &psHIDKbDevice->sPrivateData.sHIDDevice;

  //
  // Get a pointer to our instance data
  //
  psInst = &psHIDKbDevice->sPrivateData;

  // Check if our report is identical to the last
  bool identical = true;
  for (int i = 0; i < 8; i++)
  {
    if (report[i] != lastReport[i])
    {
      identical = false;
      break;
    }
  }

  if (!identical)
  {
    // Send over UART
    for (int i = 0; i < 8; i++)
    {
      if (UART1_OutChar(report[i]) != 0){
        ToggleBluetooth();
        Clock_Delay1ms(1000);
        ToggleBluetooth();
        break;
      }
    }
  }
}

bool isUniqueReport(uint8_t report1[8], uint8_t report2[8]){
  for (int i = 0; i < 8; i++)
  {
    if (report1[i] != report2[i])
    {
      return true;
      break;
    }
  }
  return false;
}
 
// Public function called by Switch_Matrix.c at the end of it's poll
// Sends the key report either by USB or Bluetooth
void SendKeyReport() {
  uint8_t report[8];

  _GenerateReport((void *)&g_sKeyboardDevice, report);

  if (isUniqueReport(report, lastReport)){
    if (bluetoothOn) {
      _SendKeyReportBluetooth((void *)&g_sKeyboardDevice, report);
    } else {
      _SendKeyReport((void *)&g_sKeyboardDevice, report);
    }
    for (int i = 0; i < 8; i++) {
      lastReport[i] = report[i];
    }
  }
}



// Add or remove the keycode from our list
uint32_t KeyStateChange(void *pvKeyboardDevice, uint8_t ui8Modifiers, uint8_t ui8UsageCode, bool bPress) {
  bool bRetcode;
  uint32_t ui32Loop, ui32Count;
  tHIDKeyboardInstance *psInst;
  tUSBDHIDKeyboardDevice *psHIDKbDevice;
  tUSBDHIDDevice *psHIDDevice;

  psHIDKbDevice = (tUSBDHIDKeyboardDevice *)pvKeyboardDevice;

  //
  // Get a pointer to the HID device data.
  //
  psHIDDevice = &psHIDKbDevice->sPrivateData.sHIDDevice;

  //
  // Assume all is well until we determine otherwise.
  //
  bRetcode = true;

  //
  // Get a pointer to our instance data
  //
  psInst = &psHIDKbDevice->sPrivateData;

  //
  // Update the global keyboard report with the information passed.
  //
  psInst->pui8Report[0] = ui8Modifiers;
  psInst->pui8Report[1] = 0;

  //
  // Were we passed a usage code for a new key press or release or was
  // this call just telling us about a modifier change?
  //
  if (ui8UsageCode != HID_KEYB_USAGE_RESERVED)
  {
    //
    // Has a key been pressed or released?
    //
    if (bPress)
    {
      //
      // A key has been pressed - add it to the list if there is space an
      // and the key is not already in the list.
      //
      bRetcode = AddKeyToPressedList(psInst, ui8UsageCode);
    }
    else
    {
      //
      // A key has been released - remove it from the list.
      //
      bRetcode = RemoveKeyFromPressedList(psInst, ui8UsageCode);

      //
      // The return code here indicates whether the key was found.  If it
      // wasn't, the list has not changes so merely exit at this point
      // without sending anything to the host.
      //
      if (!bRetcode)
      {
        return (KEYB_ERR_NOT_FOUND);
      }
    }
  }

  //
  // If we are not configured, return an error here before trying to send
  // anything.
  //
  if (!psInst->ui8USBConfigured)
  {
    return (KEYB_ERR_NOT_CONFIGURED);
  }

  //
  // If we get this far, the key information was sent successfully.  Are
  // too many keys currently pressed, though?
  //
  return (bRetcode ? KEYB_SUCCESS : KEYB_ERR_TOO_MANY_KEYS);
}

void LockKeyToggle(void *pvKeyboardDevice, uint8_t code) {
  tUSBDHIDKeyboardDevice *psHIDKbDevice = (tUSBDHIDKeyboardDevice *)pvKeyboardDevice;
  tHIDKeyboardInstance *psInst = &psHIDKbDevice->sPrivateData;

  psInst->ui8LEDStates ^= code;
}

//*****************************************************************************
//
// Global system tick counter holds elapsed time since the application started
// expressed in 100ths of a second.
//
//*****************************************************************************
volatile uint32_t g_ui32SysTickCount;

//*****************************************************************************
//
// The number of system ticks to wait for each USB packet to be sent before
// we assume the host has disconnected.  The value 50 equates to half a second.
//
//*****************************************************************************
#define MAX_SEND_DELAY 50

// Did the host update an LED? (Caps, Scroll, and Num locks)
volatile bool g_bDisplayUpdateRequired;

//*****************************************************************************
//
// This enumeration holds the various states that the keyboard can be in during
// normal operation.
//
//*****************************************************************************
volatile enum {
  //
  // Unconfigured.
  //
  STATE_UNCONFIGURED,

  //
  // No keys to send and not waiting on data.
  //
  STATE_IDLE,

  //
  // Waiting on data to be sent out.
  //
  STATE_SENDING
} g_eKeyboardState = STATE_UNCONFIGURED;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void __error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
// Handles asynchronous events from the HID keyboard driver.
//
// \param pvCBData is the event callback pointer provided during
// USBDHIDKeyboardInit().  This is a pointer to our keyboard device structure
// (&g_sKeyboardDevice).
// \param ui32Event identifies the event we are being called back for.
// \param ui32MsgData is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the HID keyboard driver to inform the application
// of particular asynchronous events related to operation of the keyboard HID
// device.
//
// \return Returns 0 in all cases.
//
//*****************************************************************************
uint32_t
KeyboardHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgData,
                void *pvMsgData)
{
  switch (ui32Event)
  {
  //
  // The host has connected to us and configured the device.
  //
  case USB_EVENT_CONNECTED:
  {
    g_bConnected = true;
    g_bSuspended = false;
    break;
  }

  //
  // The host has disconnected from us.
  //
  case USB_EVENT_DISCONNECTED:
  {
    g_bConnected = false;
    break;
  }

  //
  // We receive this event every time the host acknowledges transmission
  // of a report.  It is used here purely as a way of determining whether
  // the host is still talking to us or not.
  //
  case USB_EVENT_TX_COMPLETE:
  {
    //
    // Enter the idle state since we finished sending something.
    //
    g_eKeyboardState = STATE_IDLE;
    break;
  }

  //
  // This event indicates that the host has suspended the USB bus.
  //
  case USB_EVENT_SUSPEND:
  {
    g_bSuspended = true;
    break;
  }

  //
  // This event signals that the host has resumed signalling on the bus.
  //
  case USB_EVENT_RESUME:
  {
    g_bSuspended = false;
    break;
  }

  //
  // This event indicates that the host has sent us an Output or
  // Feature report and that the report is now in the buffer we provided
  // on the previous USBD_HID_EVENT_GET_REPORT_BUFFER callback.
  //
  case USBD_HID_KEYB_EVENT_SET_LEDS:
  {
    //
    // Set the LED to match the current state of the caps lock LED.
    //
    GPIO_PORTD_DATA_R = (GPIO_PORTD_DATA_R & ~0x2) + (ui32MsgData & HID_KEYB_CAPS_LOCK ? 0 : 0x2);

    break;
  }

  //
  // We ignore all other events.
  //
  default:
  {
    break;
  }
  }

  return (0);
}

//***************************************************************************
//
// Wait for a period of time for the state to become idle.
//
// \param ui32TimeoutTick is the number of system ticks to wait before
// declaring a timeout and returning \b false.
//
// This function polls the current keyboard state for ui32TimeoutTicks system
// ticks waiting for it to become idle.  If the state becomes idle, the
// function returns true.  If it ui32TimeoutTicks occur prior to the state
// becoming idle, false is returned to indicate a timeout.
//
// \return Returns \b true on success or \b false on timeout.
//
//***************************************************************************
bool WaitForSendIdle(uint_fast32_t ui32TimeoutTicks)
{
  uint32_t ui32Start;
  uint32_t ui32Now;
  uint32_t ui32Elapsed;

  ui32Start = g_ui32SysTickCount;
  ui32Elapsed = 0;

  while (ui32Elapsed < ui32TimeoutTicks)
  {
    //
    // Is the keyboard is idle, return immediately.
    //
    if (g_eKeyboardState == STATE_IDLE)
    {
      return (true);
    }

    //
    // Determine how much time has elapsed since we started waiting.  This
    // should be safe across a wrap of g_ui32SysTickCount.
    //
    ui32Now = g_ui32SysTickCount;
    ui32Elapsed = ((ui32Start < ui32Now) ? (ui32Now - ui32Start) : (((uint32_t)0xFFFFFFFF - ui32Start) + ui32Now + 1));
  }

  //
  // If we get here, we timed out so return a bad return code to let the
  // caller know.
  //
  return (false);
}

// need to keep a modifier list

uint8_t modifierFlags = 0;

void App_Keyboard_KeyPress(uint8_t c)
{

  g_eKeyboardState = STATE_SENDING;
  switch (c)
  {
  case LEFT_SHIFT:
    modifierFlags |= HID_KEYB_LEFT_SHIFT;
    break;
  case RIGHT_SHIFT:
    modifierFlags |= HID_KEYB_RIGHT_SHIFT;
    break;
  case LEFT_CTRL:
    modifierFlags |= HID_KEYB_LEFT_CTRL;
    break;
  case RIGHT_CTRL:
    modifierFlags |= HID_KEYB_RIGHT_CTRL;
    break;
  case LEFT_ALT:
    modifierFlags |= HID_KEYB_LEFT_ALT;
    break;
  case RIGHT_ALT:
    modifierFlags |= HID_KEYB_RIGHT_ALT;
    break;
  case LEFT_GUI:
    modifierFlags |= HID_KEYB_LEFT_GUI;
    break;
  case RIGHT_GUI:
    modifierFlags |= HID_KEYB_RIGHT_GUI;
    break;
  case CAPS_LOCK:
    LockKeyToggle((void *)&g_sKeyboardDevice, HID_KEYB_CAPS_LOCK);
    break;
  case BLUETOOTH:
    ToggleBluetooth();
    break;
  case APP_A:
  case APP_B:
    return;
  default:
    KeyStateChange((void *)&g_sKeyboardDevice, modifierFlags, c, true);
    return;
  }

  switch (c)
  {
  case LEFT_SHIFT:
  case RIGHT_SHIFT:
  case LEFT_CTRL:
  case RIGHT_CTRL:
  case LEFT_ALT:
  case RIGHT_ALT:
  case LEFT_GUI:
  case RIGHT_GUI:
    KeyStateChange((void *)&g_sKeyboardDevice, modifierFlags, HID_KEYB_USAGE_RESERVED, true);
    break; 
  }
}

void App_Keyboard_KeyRelease(uint8_t c)
{

  g_eKeyboardState = STATE_SENDING;
  switch (c)
  {
  case LEFT_SHIFT:
    modifierFlags &= ~HID_KEYB_LEFT_SHIFT;
    break;
  case RIGHT_SHIFT:
    modifierFlags &= ~HID_KEYB_RIGHT_SHIFT;
    break;
  case LEFT_CTRL:
    modifierFlags &= ~HID_KEYB_LEFT_CTRL;
    break;
  case RIGHT_CTRL:
    modifierFlags &= ~HID_KEYB_RIGHT_CTRL;
    break;
  case LEFT_ALT:
    modifierFlags &= ~HID_KEYB_LEFT_ALT;
    break;
  case RIGHT_ALT:
    modifierFlags &= ~HID_KEYB_RIGHT_ALT;
    break;
  case LEFT_GUI:
    modifierFlags &= ~HID_KEYB_LEFT_GUI;
    break;
  case RIGHT_GUI:
    modifierFlags &= ~HID_KEYB_RIGHT_GUI;
    break;
  case APP_A:
  case APP_B:
    return;
  default:
    KeyStateChange((void *)&g_sKeyboardDevice,
                   modifierFlags,
                   c,
                   false);
    return;
  }

  switch (c)
  {
  case LEFT_SHIFT:
  case RIGHT_SHIFT:
  case LEFT_CTRL:
  case RIGHT_CTRL:
  case LEFT_ALT:
  case RIGHT_ALT:
  case LEFT_GUI:
  case RIGHT_GUI:
    KeyStateChange((void *)&g_sKeyboardDevice,
                   modifierFlags,
                   HID_KEYB_USAGE_RESERVED,
                   false);
    break;
  }
  //
  // Wait until the key release message has been sent.
  //
}

void Bluetooth_Init(){

  // Setup UART with BGM220P
  UART1_Init();

  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3;
  while (!(SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R3)){}

  GPIO_PORTD_DIR_R |= 0x1;
  GPIO_PORTD_DEN_R |= 0x1;
  GPIO_PORTD_DATA_R &= ~0x1;
}

void App_Keyboard_Tick(){ 
  // NOT IMPLEMENTED
  return; 
}

void App_Keyboard_Load(void) {

  // Initialize and turn on capslock LED (PD1, active low)
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3;
  while (!(SYSCTL_PRGPIO_R & SYSCTL_RCGCGPIO_R3))
  {
  }

  GPIO_PORTD_DIR_R |= 0x2;
  GPIO_PORTD_PUR_R |= 0x2;
  GPIO_PORTD_DEN_R |= 0x2;

  GPIO_PORTD_DATA_R &= ~0x2;
  
  uint_fast32_t ui32LastTickCount;
  bool bLastSuspend;

  //
  // Configure the required pins for USB operation.
  //
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  MAP_GPIOPinTypeUSBAnalog(GPIO_PORTD_BASE, GPIO_PIN_4 | GPIO_PIN_5);

  //
  // Erratum workaround for silicon revision A1.  VBUS must have pull-down.
  //
  if (CLASS_IS_TM4C123 && REVISION_IS_A1)
  {
    HWREG(GPIO_PORTB_BASE + GPIO_O_PDR) |= GPIO_PIN_1;
  }

  //
  // Not configured initially.
  //
  g_bConnected = false;
  g_bSuspended = false;
  bLastSuspend = false;

  //
  // Initialize the USB stack for device mode.  We do not operate in USB
  // device mode with active monitoring of VBUS and therefore, we will
  // specify eUSBModeForceDevice as the operating mode instead of
  // eUSBModeDevice.  To use eUSBModeDevice, the EK-TM4C123GXL LaunchPad
  // must have the R28 and R29 populated with zero ohm resistors.
  //
  USBStackModeSet(0, eUSBModeForceDevice, 0);

  //
  // Pass our device information to the USB HID device class driver,
  // initialize the USB controller, and connect the device to the bus.
  //
  USBDHIDKeyboardInit(0, &g_sKeyboardDevice);

  Switch_Init();

  Bluetooth_Init();
}

void App_Keyboard_Unload(){
  // NOT IMPLEMENTED
  return;
}