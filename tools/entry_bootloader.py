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


calc = crc.Calculator(crc.Crc16.MODBUS)
s = serial.Serial('/dev/ttyUSB0', 250000, timeout=0.1)

addr = b'\x02'
pdu = b'\x64EntryBootloader\0'
pack = addr + pdu
pack += calc.checksum(pack).to_bytes(2, 'little')
s.write(pack)

print(s.read(256))
