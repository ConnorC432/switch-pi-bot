#!/bin/bash

#Connect the controller to the switch in the "Change Controller Grip/Order" menu

send_command() {
    echo -ne "$1" > /dev/hidg0
}

#Press L + R
send_command '\x00\x0C\x00\x00\x00\x00\x00\x00'
sleep 0.5
send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
sleep 1

#Press A
send_command '\x04\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1
send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1