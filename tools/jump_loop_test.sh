#!/bin/bash
for i in {1..100}
do
    sudo /home/xrj/elec/solarnode_firmware/tools/entry_bootloader.py
    sudo /home/xrj/elec/xx32_bootloader/devices/PY32F0/tools/go_program.py 0x08000000
done
