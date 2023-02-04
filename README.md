# NOT READY 2/3/2023
# Renaming Files and adapting code

# OSRS AUTO ALCHING TOOL 


Tool for human-like autoclicking, specifically for Old School Runescape


This is a simple Flipper Zero application to send left-clicks as a USB HID device.

Forked USB HID Autofire tool https://github.com/pbek/usb_hid_autofire

Adapted code and added capabilities for human-like randomized autoclicking.



## Building

```shell
cd applications_user
git clone https://github.com/DiehlJ7/osrs_auto_alch.git

cd ..

# Build the application
./fbt fap_usb_hid_autofire

# Build and launch the application
./fbt launch_app APPSRC=usb_hid_autofire
```

# Future
1. User Friendly UI
2. Scalable scales
3. OSRS Theme
4. Clean Up Code
5. Add Capabilities for mouse movement patterns/scripting
6. Add scripts for other simple skills
