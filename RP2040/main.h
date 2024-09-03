#ifndef MAIN_H
#define MAIN_H

#include "pico/stdlib.h"
#include "hardware/usb.h"
#include "hardware/usb/hid.h"
#include "pico/cyw43_arch.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/apps/httpd.h"
#include "tusb.h"

#define BUTTON_A  (1 << 0)
#define BUTTON_B  (1 << 1)
#define BUTTON_X  (1 << 2)
#define BUTTON_Y  (1 << 3)
#define BUTTON_L1 (1 << 4)
#define BUTTON_R1 (1 << 5)
#define BUTTON_L2 (1 << 6)
#define BUTTON_R2 (1 << 7)
#define BUTTON_SELECT  (1 << 8)
#define BUTTON_START   (1 << 9)
#define BUTTON_HOME    (1 << 10)
#define BUTTON_THUMBL  (1 << 11)
#define BUTTON_THUMBR  (1 << 12)
#define BUTTON_DPAD_UP    (1 << 13)
#define BUTTON_DPAD_DOWN  (1 << 14)
#define BUTTON_DPAD_LEFT  (1 << 15)
#define BUTTON_DPAD_RIGHT (1 << 16)

#define DIRECTION_MASK  0xF000
#define BUTTON_MASK 0x0FFF

void setup_wifi();
void handle_http_status();
void send_hid_report();

#endif