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

# 待修改相关代码，此处不符合Modbus标准，CRC计算应该包含设备地址
pdu = b'\x04\x00\x01\x00\x02'
checksum = calc.checksum(pdu).to_bytes(2, 'little')
s.write(b'\x02' + pdu + checksum)

print(s.read(10))
