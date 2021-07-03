/**
 * Copyright (c) 2017 - 2020, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "nrf.h"
#include "nrf_drv_usbd.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "nrf_drv_power.h"

#include "app_timer.h"
#include "app_usbd.h"
#include "app_usbd_core.h"
#include "app_usbd_hid_mouse.h"
#include "app_usbd_hid_kbd.h"
#include "app_usbd_dummy.h"
#include "app_error.h"
#include "bsp.h"

#include "bsp_cli.h"
#include "nrf_cli.h"
#include "nrf_cli_uart.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"




//esb
#include "nrf_esb.h"
#include "nrf_esb_error_codes.h"
#include "app_usbd_hid_kbd_internal.h"
#include "nrf_gpiote.h"
#include "nrf_drv_gpiote.h"
#include "user/kbd.h"
#include "SEGGER_RTT.h"

#include "nrfx_timer.h"
#include "nrf_drv_spi.h"
#include "user/ws2812.h"

#include "user/low_energy_timer.h"
#include "nrf_gzll.h"
#include "nrf_gzll_error.h"




bool m_usbOnline=false;

#define PIPE_NUMBER             0   /**< Pipe 0 is used in this example. */
#define TX_PAYLOAD_LENGTH       1   /**< 1-byte payload length is used when transmitting. */
#define MAX_TX_ATTEMPTS         100 /**< Maximum number of transmission attempts */

static uint8_t                  m_data_payload[TX_PAYLOAD_LENGTH];                /**< Payload to send to Host. */
static uint8_t                  m_ack_payload[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH]; /**< Placeholder for received ACK payloads from Host. */

/**
 * @brief Enable USB power detection
 */
#ifndef USBD_POWER_DETECTION
#define USBD_POWER_DETECTION true
#endif

/**
 * @brief Enable HID mouse class
 */
#define CONFIG_HAS_MOUSE    1

/**
 * @brief Enable HID keyboard class
 */
#define CONFIG_HAS_KBD      1

/**
 * @brief Mouse button count
 */
#define CONFIG_MOUSE_BUTTON_COUNT 2

/**
 * @brief Mouse speed (value sent via HID when board button is pressed).
 */
#define CONFIG_MOUSE_MOVE_STEP (3)

/**
 * @brief Mouse move repeat time in milliseconds
 */
#define CONFIG_MOUSE_MOVE_TIME_MS (5)

/**
 * @brief Letter to be sent on LETTER button
 *
 * @sa BTN_KBD_LETTER
 */
#define CONFIG_KBD_LETTER APP_USBD_HID_KBD_G

/**
 * @brief Propagate SET_PROTOCOL command to other HID instance
 */
#define PROPAGATE_PROTOCOL  0


#define LED_CAPSLOCK       (BSP_BOARD_LED_0) /**< CAPSLOCK */
#define LED_NUMLOCK        (BSP_BOARD_LED_1) /**< NUMLOCK */
#define LED_HID_REP        (BSP_BOARD_LED_2) /**< Changes its state if any HID report was received or transmitted */
#define LED_USB_START      (BSP_BOARD_LED_3) /**< The USBD library has been started and the bus is not in SUSPEND state */

#define BTN_MOUSE_X_POS    0
#define BTN_MOUSE_LEFT     1
#define BTN_KBD_SHIFT      2
#define BTN_KBD_LETTER     3

/**
 * @brief Additional key release events
 *
 * This example needs to process release events of used buttons
 */
enum {
    BSP_USER_EVENT_RELEASE_0 = BSP_EVENT_KEY_LAST + 1, /**< Button 0 released */
    BSP_USER_EVENT_RELEASE_1,                          /**< Button 1 released */
    BSP_USER_EVENT_RELEASE_2,                          /**< Button 2 released */
    BSP_USER_EVENT_RELEASE_3,                          /**< Button 3 released */
    BSP_USER_EVENT_RELEASE_4,                          /**< Button 4 released */
    BSP_USER_EVENT_RELEASE_5,                          /**< Button 5 released */
    BSP_USER_EVENT_RELEASE_6,                          /**< Button 6 released */
    BSP_USER_EVENT_RELEASE_7,                          /**< Button 7 released */
};

/**
 * @brief USB composite interfaces
 */
#define APP_USBD_INTERFACE_MOUSE 0
#define APP_USBD_INTERFACE_KBD   1

/**
 * @brief User event handler, HID mouse
 */
static void hid_mouse_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                      app_usbd_hid_user_event_t event);

/**
 * @brief User event handler, HID keyboard
 */
static void hid_kbd_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                    app_usbd_hid_user_event_t event);

/*lint -save -e26 -e64 -e123 -e505 -e651*/

/**
 * @brief Global HID mouse instance
 */
APP_USBD_HID_MOUSE_GLOBAL_DEF(m_app_hid_mouse,
                              APP_USBD_INTERFACE_MOUSE,
                              NRF_DRV_USBD_EPIN1,
                              CONFIG_MOUSE_BUTTON_COUNT,
                              hid_mouse_user_ev_handler,
                              APP_USBD_HID_SUBCLASS_BOOT
);

APP_USBD_DUMMY_GLOBAL_DEF(m_app_mouse_dummy, APP_USBD_INTERFACE_MOUSE);

/**
 * @brief Global HID keyboard instance
 */
APP_USBD_HID_KBD_GLOBAL_DEF(m_app_hid_kbd,
                            APP_USBD_INTERFACE_KBD,
                            NRF_DRV_USBD_EPIN2,
                            hid_kbd_user_ev_handler,
                            APP_USBD_HID_SUBCLASS_BOOT
);
APP_USBD_DUMMY_GLOBAL_DEF(m_app_kbd_dummy, APP_USBD_INTERFACE_KBD);

/*lint -restore*/

/**
 * @brief Timer to repeat mouse move
 */
APP_TIMER_DEF(m_mouse_move_timer);


static void kbd_status(void)
{
    if(app_usbd_hid_kbd_led_state_get(&m_app_hid_kbd, APP_USBD_HID_KBD_LED_NUM_LOCK))
    {
        
    }
    else
    {
       
    }

    if(app_usbd_hid_kbd_led_state_get(&m_app_hid_kbd, APP_USBD_HID_KBD_LED_CAPS_LOCK))
    {
        
    }
    else
    {
       
    }
}

/**
 * @brief Class specific event handler.
 *
 * @param p_inst    Class instance.
 * @param event     Class specific event.
 * */
static void hid_mouse_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                      app_usbd_hid_user_event_t event)
{
    UNUSED_PARAMETER(p_inst);
    switch (event) {
        case APP_USBD_HID_USER_EVT_OUT_REPORT_READY:
            /* No output report defined for HID mouse.*/
            ASSERT(0);
            break;
        case APP_USBD_HID_USER_EVT_IN_REPORT_DONE:
           
            break;
        case APP_USBD_HID_USER_EVT_SET_BOOT_PROTO:
            UNUSED_RETURN_VALUE(hid_mouse_clear_buffer(p_inst));
#if PROPAGATE_PROTOCOL
            hid_kbd_on_set_protocol(&m_app_hid_kbd, APP_USBD_HID_USER_EVT_SET_BOOT_PROTO);
#endif
            break;
        case APP_USBD_HID_USER_EVT_SET_REPORT_PROTO:
            UNUSED_RETURN_VALUE(hid_mouse_clear_buffer(p_inst));
#if PROPAGATE_PROTOCOL
            hid_kbd_on_set_protocol(&m_app_hid_kbd, APP_USBD_HID_USER_EVT_SET_REPORT_PROTO);
#endif
            break;
        default:
            break;
    }
}

/**
 * @brief Class specific event handler.
 *
 * @param p_inst    Class instance.
 * @param event     Class specific event.
 * */
static void hid_kbd_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                    app_usbd_hid_user_event_t event)
{
    UNUSED_PARAMETER(p_inst);
    switch (event) {
        case APP_USBD_HID_USER_EVT_OUT_REPORT_READY:
            /* Only one output report IS defined for HID keyboard class. Update LEDs state. */
           
            kbd_status();
            break;
        case APP_USBD_HID_USER_EVT_IN_REPORT_DONE:
            
            break;
        case APP_USBD_HID_USER_EVT_SET_BOOT_PROTO:
            UNUSED_RETURN_VALUE(hid_kbd_clear_buffer(p_inst));
#if PROPAGATE_PROTOCOL
            hid_mouse_on_set_protocol(&m_app_hid_mouse, APP_USBD_HID_USER_EVT_SET_BOOT_PROTO);
#endif
            break;
        case APP_USBD_HID_USER_EVT_SET_REPORT_PROTO:
            UNUSED_RETURN_VALUE(hid_kbd_clear_buffer(p_inst));
#if PROPAGATE_PROTOCOL
            hid_mouse_on_set_protocol(&m_app_hid_mouse, APP_USBD_HID_USER_EVT_SET_REPORT_PROTO);
#endif
            break;
        default:
            break;
    }
}


/**
 * @brief USBD library specific event handler.
 *
 * @param event     USBD library event.
 * */
static void usbd_user_ev_handler(app_usbd_event_type_t event)
{
    switch (event)
    {
        case APP_USBD_EVT_DRV_SOF:
            break;
        case APP_USBD_EVT_DRV_SUSPEND:
            app_usbd_suspend_req(); // Allow the library to put the peripheral into sleep mode
           
            break;
        case APP_USBD_EVT_DRV_RESUME:
            
            //kbd_status(); /* Restore LED state - during SUSPEND all LEDS are turned off */
            break;
        case APP_USBD_EVT_STARTED:
            
            break;
        case APP_USBD_EVT_STOPPED:
            app_usbd_disable();
            
            break;
        case APP_USBD_EVT_POWER_DETECTED:
            NRF_LOG_INFO("USB power detected");

            if (!nrf_drv_usbd_is_enabled())
            {
                app_usbd_enable();
            }
            break;
        case APP_USBD_EVT_POWER_REMOVED:
            NRF_LOG_INFO("USB power removed");
            app_usbd_stop();
            //
            m_kbdContext.m_connMode=WIRELESS_MODE;
            //m_usbOnline=false;
             //kbd_usb_disconnect();
            break;
        case APP_USBD_EVT_POWER_READY:
            NRF_LOG_INFO("USB ready");
            app_usbd_start();
            m_kbdContext.m_connMode=WIRE_MODE;
            //
            //m_usbOnline=true;
            //kbd_usb_connect();
            break;
        default:
            break;
    }
}


static void mouse_move_timer_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);
    UNUSED_RETURN_VALUE(app_usbd_hid_mouse_x_move(&m_app_hid_mouse, CONFIG_MOUSE_MOVE_STEP));
}





nrf_esb_payload_t rx_payload;


void clocks_start( void )
{
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;

    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}


/*
void nrf_esb_event_handler(nrf_esb_evt_t const * p_event)
{
    switch (p_event->evt_id)
    { 
        case NRF_ESB_EVENT_TX_SUCCESS:
            NRF_LOG_DEBUG("TX SUCCESS EVENT");
            break;
        case NRF_ESB_EVENT_TX_FAILED:
           
             NRF_LOG_DEBUG("TX FAILED EVENT");
            (void) nrf_esb_flush_tx();
            (void) nrf_esb_start_tx();
            break;
        case NRF_ESB_EVENT_RX_RECEIVED:
            NRF_LOG_DEBUG("RX RECEIVED EVENT");
            if (nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS)
            {
                //send_hid_data(&m_app_hid_kbd,rx_payload.data);
                // Set LEDs identical to the ones on the PTX.
                //nrf_gpio_pin_write(LED_1, !(rx_payload.data[1]%8>0 && rx_payload.data[1]%8<=4));
                //nrf_gpio_pin_write(LED_2, !(rx_payload.data[1]%8>1 && rx_payload.data[1]%8<=5));
                //nrf_gpio_pin_write(LED_3, !(rx_payload.data[1]%8>2 && rx_payload.data[1]%8<=6));
                //nrf_gpio_pin_write(LED_4, !(rx_payload.data[1]%8>3));

                //NRF_LOG_DEBUG("Receiving packet: %02x", rx_payload.data[1]);
            }
            break;
    }
}

uint32_t esb_init( void )
{
    uint32_t err_code;
    uint8_t base_addr_0[4] = {0x43,0x10,0x10,0x01};
    uint8_t base_addr_1[4] = {0x43,0x10,0x10,0x01};
    uint8_t addr_prefix[8] = {0x34, 0x2c, 0x2c, 0x2c, 0x2c, 0x2c, 0x2c, 0x2c };
   
    nrf_esb_config_t nrf_esb_config         = NRF_ESB_LEGACY_CONFIG;
    nrf_esb_config.payload_length           = 8;
    nrf_esb_config.protocol                 = NRF_ESB_PROTOCOL_ESB_DPL;
    nrf_esb_config.bitrate                  = NRF_ESB_BITRATE_2MBPS;
    nrf_esb_config.mode                     = NRF_ESB_MODE_PTX;
    nrf_esb_config.event_handler            = nrf_esb_event_handler;
    nrf_esb_config.selective_auto_ack       = true;
    nrf_esb_config.crc                      =NRF_ESB_CRC_8BIT;
    nrf_esb_config.tx_output_power          =NRF_ESB_TX_POWER_0DBM;
    nrf_esb_config.retransmit_count         =15;
    nrf_esb_config.retransmit_delay         =400;
    
     err_code= nrf_esb_set_rf_channel(60);
     VERIFY_SUCCESS(err_code);
    
   
    VERIFY_SUCCESS(err_code);
    err_code = nrf_esb_init(&nrf_esb_config);
    VERIFY_SUCCESS(err_code);

    err_code = nrf_esb_set_base_address_0(base_addr_0);
    VERIFY_SUCCESS(err_code);

    err_code = nrf_esb_set_base_address_1(base_addr_1);
    VERIFY_SUCCESS(err_code);

    err_code = nrf_esb_set_prefixes(addr_prefix, 8);
    VERIFY_SUCCESS(err_code);

    err_code= nrf_esb_set_address_length(5);

    

    return err_code;
}

*/

/**
 * @brief Gazelle callback.
 * @warning Required for successful Gazell initialization.
 */
void nrf_gzll_host_rx_data_ready(uint32_t pipe, nrf_gzll_host_rx_info_t rx_info)
{
}


/**
 * @brief Gazelle callback.
 * @warning Required for successful Gazell initialization.
 */
void nrf_gzll_disabled()
{
}


/*****************************************************************************/
/** @name Gazell callback function definitions  */
/*****************************************************************************/
/**
 * @brief TX success callback.
 *
 * @details If an ACK was received, another packet is sent.
 */
void  nrf_gzll_device_tx_success(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{
    bool     result_value         = false;
    uint32_t m_ack_payload_length = NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH;

    if (tx_info.payload_received_in_ack)
    {
        
    }

  

}


/**
 * @brief TX failed callback.
 *
 * @details If the transmission failed, send a new packet.
 *
 * @warning This callback does not occur by default since NRF_GZLL_DEFAULT_MAX_TX_ATTEMPTS
 * is 0 (inifinite retransmits).
 */
void nrf_gzll_device_tx_failed(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{
    NRF_LOG_ERROR("Gazell transmission failed");

    // Load data into TX queue.
   

    /*bool result_value = nrf_gzll_add_packet_to_tx_fifo(pipe, m_data_payload, TX_PAYLOAD_LENGTH);
    if (!result_value)
    {
        NRF_LOG_ERROR("TX fifo error ");
    }*/
}

#define TEST_USB



APP_TIMER_DEF(m_keyboard_scan_timer);
static void keyboard_scan_timer_handler(void * p_context)
{
     kbd_loop();
     if(kbd_status_is_changed())
     {
          kbd_update_hid_data();                
          kbd_send_hid_report();
            
     }
    
}


/**
 * Function for configuring UICR_REGOUT0 register
 * to set GPIO output voltage to 3.0V.
 */
static void gpio_output_voltage_setup(void)
{
    // Configure UICR_REGOUT0 register only if it is set to default value.
    if ((NRF_UICR->REGOUT0 & UICR_REGOUT0_VOUT_Msk) ==
        (UICR_REGOUT0_VOUT_DEFAULT << UICR_REGOUT0_VOUT_Pos))
    {
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
 
        NRF_UICR->REGOUT0 = (NRF_UICR->REGOUT0 & ~((uint32_t)UICR_REGOUT0_VOUT_Msk)) |
                            (UICR_REGOUT0_VOUT_2V7 << UICR_REGOUT0_VOUT_Pos);
 
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
 
        // System reset is needed to update UICR registers.
        NVIC_SystemReset();
    }
}





int main(void)
{

   gpio_output_voltage_setup();
    
    


    ret_code_t ret;
    static const app_usbd_config_t usbd_config = {
        .ev_state_proc = usbd_user_ev_handler,
    };
//////////////////
    // Initialize Gazell.
    bool result_value = nrf_gzll_init(NRF_GZLL_MODE_DEVICE);
    GAZELLE_ERROR_CODE_CHECK(result_value);

    // Attempt sending every packet up to MAX_TX_ATTEMPTS times.
    nrf_gzll_set_max_tx_attempts(MAX_TX_ATTEMPTS);
    
    nrf_gzll_set_base_address_0(0x01020304);
    nrf_gzll_set_base_address_1(0x05060709);

    result_value = nrf_gzll_enable();
    GAZELLE_ERROR_CODE_CHECK(result_value);
//////////////
    ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);

    ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret);

    nrf_drv_clock_lfclk_request(NULL);
    while(!nrf_drv_clock_lfclk_is_running())
    {
        /* Just waiting */
    }

    ret = app_timer_init();
    APP_ERROR_CHECK(ret);


    //ret = app_timer_create(&m_keyboard_scan_timer, APP_TIMER_MODE_REPEATED, keyboard_scan_timer_handler);
    //APP_ERROR_CHECK(ret);

    //app_timer_start(m_keyboard_scan_timer, APP_TIMER_TICKS(CONFIG_MOUSE_MOVE_TIME_MS), NULL);

    


    ret = app_usbd_init(&usbd_config);
    APP_ERROR_CHECK(ret);

    app_usbd_class_inst_t const * class_inst_mouse;
#if CONFIG_HAS_MOUSE
    class_inst_mouse = app_usbd_hid_mouse_class_inst_get(&m_app_hid_mouse);
#else
    class_inst_mouse = app_usbd_dummy_class_inst_get(&m_app_mouse_dummy);
#endif
    ret = app_usbd_class_append(class_inst_mouse);
    APP_ERROR_CHECK(ret);

    app_usbd_class_inst_t const * class_inst_kbd;
#if CONFIG_HAS_KBD
    class_inst_kbd = app_usbd_hid_kbd_class_inst_get(&m_app_hid_kbd);
#else
    class_inst_kbd = app_usbd_dummy_class_inst_get(&m_app_kbd_dummy);
#endif
    ret = app_usbd_class_append(class_inst_kbd);
    APP_ERROR_CHECK(ret);

    NRF_LOG_INFO("USBD HID composite example started.");
    
    if (USBD_POWER_DETECTION)
    {
        ret = app_usbd_power_events_enable();
        APP_ERROR_CHECK(ret);
    }
    else
    {
        NRF_LOG_INFO("No USB power detection enabled\r\nStarting USB now");

        app_usbd_enable();
        app_usbd_start();
    }

    clocks_start();
  

#ifdef TEST_USB
    kbd_init();
    m_kbdContext.m_usbHandle=&m_app_hid_kbd;
    ws2812_init();
#endif

    while (true)
    {
         while (app_usbd_event_queue_process());
#ifdef TEST_USB
         kbd_loop();
         if(kbd_status_is_changed())
         {
              kbd_update_hid_data();                
              kbd_send_hid_report();
                
         }
         
#endif
    }
}
