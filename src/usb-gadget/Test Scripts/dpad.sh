#!/bin/bash

#Controllers and Sensors / Test Input Devices / Test Controller Buttons

send_command() {
    echo -ne "$1" > /dev/hidg0
}

# Press DPad Right
send_command '\x00\x00\x01\x00\x00\x00\x00\x00'
sleep 0.1

# Press DPad Down
send_command '\x00\x00\x03\x00\x00\x00\x00\x00'
sleep 0.1

# Press DPad Left
send_command '\x00\x00\x05\x00\x00\x00\x00\x00'
sleep 0.1

# Press DPad Up
send_command '\x00\x00\x07\x00\x00\x00\x00\x00'
sleep 0.1

# Release all DPad buttons
send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
