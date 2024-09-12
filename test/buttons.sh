#!/bin/bash

#Controllers and Sensors / Test Input Devices / Test Controller Buttons

send_command() {
    echo -ne "$1" > /dev/hidg0
}

# Press Y
send_command '\x01\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1
send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1

# Press B
send_command '\x02\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1
send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1

# Press A
send_command '\x04\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1
send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1

# Press X
send_command '\x08\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1
send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1

# Press L
send_command '\x10\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1
send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1

# Press R
send_command '\x20\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1
send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1

# Press Zl
send_command '\x40\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1
send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1

# Press Zr
send_command '\x80\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1
send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1

# Press -
send_command '\x00\x01\x00\x00\x00\x00\x00\x00'
sleep 0.1
send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1

# Press +
send_command '\x00\x02\x00\x00\x00\x00\x00\x00'
sleep 0.1
send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1

# Press L Stick
send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1
send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1

# Press R Stick
send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1
send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
sleep 0.1

# Press Power Button
#send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
#sleep 0.1
#send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
#sleep 0.1

# Press Capture Button
#send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
#sleep 0.1
#send_command '\x00\x00\x00\x00\x00\x00\x00\x00'
#sleep 0.1