/***************************************************************************//**
 * @file
 * @brief Core application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "em_common.h"
#include "em_gpio.h"
#include "em_emu.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "app.h"
#include "app_log.h"
#include "app_assert.h"
#include "gatt_db.h"
#include "sl_simple_button_instances.h"
#include "app_iostream_usart.h"
#include "sl_simple_led_instances.h"
#include "uartdrv.h"

#define KEY_ARRAY_SIZE         25
#define MODIFIER_INDEX         0
#define DATA_INDEX             2

#define CAPSLOCK_KEY_OFF       0x00
#define CAPSLOCK_KEY_ON        0x02

#define EM4WU_PIN           BSP_GPIO_PC0_PIN
#define EM4WU_PORT          BSP_GPIO_PC0_PORT
#define EM4WU_EM4WUEN_NUM   (9)                       // PD2 is EM4WUEN pin 9
#define EM4WU_EM4WUEN_MASK  (1 << EM4WU_EM4WUEN_NUM)

// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;
static uint8_t notification_enabled = 0;

static uint8_t input_report_data[] = { 0, 0, 0, 0, 0, 0, 0, 0 };


void callback(UARTDRV_Handle_t handle,
              Ecode_t transferStatus,
              uint8_t *data,
              UARTDRV_Count_t transferCount)
{
  for (int i = 0; i < 8; i++){
      input_report_data[i] = data[i];
  }

  sl_bt_external_signal(1);
  return;
}

// turn on, immediately shutoff (EM4) and then wait for the wakeup and then broadcast


/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////

  // Initialize our UART connection with the TM4C
  app_iostream_usart_init();

  sl_simple_led_init_instances();

  GPIO_EM4EnablePinWakeup(GPIO_IEN_EM4WUIEN6, GPIO_IEN_EM4WUIEN6); // PC0

  sl_led_turn_on(&sl_led_status_led);
}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////

  if (GPIO_PinInGet(gpioPortC, 0) == 0){
        sl_led_turn_off(&sl_led_status_led);
        GPIO_IntClear(GPIO_IntGet());
        EMU_EnterEM4();
  }

  app_iostream_usart_process_action();

  uint8_t uartInput[8];

  if (notification_enabled == 1){
      UARTDRV_Receive(sl_uartdrv_get_default(), uartInput, 8, callback);
  }

}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;

  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      // Create an advertising set.
      sc = sl_bt_advertiser_create_set(&advertising_set_handle);
      app_assert_status(sc);

      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert_status(sc);

      // Set advertising interval to 100ms.
      sc = sl_bt_advertiser_set_timing(
        advertising_set_handle,
        160, // min. adv. interval (milliseconds * 1.6)
        160, // max. adv. interval (milliseconds * 1.6)
        0,   // adv. duration
        0);  // max. num. adv. events
      app_assert_status(sc);


      printf("boot event - starting advertising\r\n");

      sc = sl_bt_sm_configure(0, sm_io_capability_noinputnooutput);
      app_assert_status(sc);
      sc = sl_bt_sm_set_bondable_mode(1);
      app_assert_status(sc);

      // Start advertising and enable connections.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);
      break;

    // -------------------------------
    // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id:
      printf("connection opened\r\n");
      sc =
        sl_bt_sm_increase_security(evt->data.evt_connection_opened.connection);
      app_assert_status(sc);
      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      printf("connection closed, reason: 0x%2.2x\r\n",
              evt->data.evt_connection_closed.reason);
      notification_enabled = 0;
      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert_status(sc);

      // Restart advertising after client has disconnected.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);
      break;

    case sl_bt_evt_sm_bonded_id:
      printf("successful bonding\r\n");
      break;

    case sl_bt_evt_sm_bonding_failed_id:
      printf("bonding failed, reason 0x%2X\r\n",
              evt->data.evt_sm_bonding_failed.reason);

      /* Previous bond is broken, delete it and close connection,
       *  host must retry at least once */
      sc = sl_bt_sm_delete_bondings();
      app_assert_status(sc);
      sc = sl_bt_connection_close(evt->data.evt_sm_bonding_failed.connection);
      app_assert_status(sc);
      break;

    case sl_bt_evt_gatt_server_characteristic_status_id:
      if (evt->data.evt_gatt_server_characteristic_status.characteristic
          == gattdb_report) {
        // client characteristic configuration changed by remote GATT client
        if (evt->data.evt_gatt_server_characteristic_status.status_flags
            == gatt_server_client_config) {
          if (evt->data.evt_gatt_server_characteristic_status.
              client_config_flags == gatt_notification) {
            notification_enabled = 1;
          } else {
            notification_enabled = 0;
          }
        }
      }
      break;
    case  sl_bt_evt_system_external_signal_id:
      if (notification_enabled == 1) {

        sc = sl_bt_gatt_server_notify_all(gattdb_report,
                                          sizeof(input_report_data),
                                          input_report_data);
        app_assert_status(sc);

        printf("Key report was sent\r\n");
      }
      break;

    // -------------------------------
    // Default event handler.
    default:
      break;
  }
}
