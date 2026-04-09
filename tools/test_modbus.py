#!/usr/bin/env python3
#!/usr/bin/env python3
########################################################################
# Copyright (C) 2026  Xu Ruijun
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
########################################################################
# 使用本程序需要py32_rs485_convert转换模块，进行UART与编码RS485转换
import serial
import crc
import struct
import time

class CustomError(Exception):
    pass

calc = crc.Calculator(crc.Crc16.MODBUS)
s = serial.Serial('/dev/ttyUSB0', 250000, timeout=0.01)

addr = b'\x02'
pdu = b'\x04\x00\x10\x00\x04'
pack = addr + pdu
pack += calc.checksum(pack).to_bytes(2, 'little')

def f():
    s.write(pack)

    pack_r = s.read(13)
    #print(pack_r)
    if len(pack_r) != 13:
        raise CustomError(f"len {len(pack_r)}")
    elif pack_r[:3] != b'\x02\x04\x08':
        raise CustomError(f'head {pack_r[:3]}')
    elif calc.checksum(pack[:-2]).to_bytes(2, 'little') != pack[-2:]:
        raise CustomError('CRC Error')
    else:
        data = pack_r[3:3+2*4]
        vbus,ibus,islr,vslr = struct.unpack('>HHHH', data)
        return vbus,ibus,islr,vslr

while True:
    try:
        print(f())
    except CustomError as e:
        s.read(1000)  # 清空缓冲区
        print(e)
