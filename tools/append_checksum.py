#!/usr/bin/env python3
########################################################################
# Append checksum to program file
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
import sys
import crc
import math
import functools

crc_config = crc.Configuration(width=32,
                               polynomial=0x4c11db7,
                               init_value=0xffffffff,
                               final_xor_value=0,
                               reverse_input=False,
                               reverse_output=False)
calc_crc32 = crc.Calculator(crc_config)
C_sqrt2 = int(math.sqrt(2)*2**32)&0xffffffff
C_sqrt3 = int(math.sqrt(3)*2**32)&0xffffffff

# 经过修改的Fletcher算法
def calc_fletcher_32m(data):
    assert len(data) % 4 == 0
    a = C_sqrt2  # 初始化常数避免前导零碰撞和一些简单序列使a变0
    b = C_sqrt3
    for i in range(len(data)//4):
        a += int.from_bytes(data[i*4:(i+1)*4], 'little')
        a &= 0xffffffff
        b += a
        b &= 0xffffffff
    b_r16 = ((b&0xffff)<<16)|(b>>16)
    return (b_r16 + a)&0xffffffff

if __name__ == "__main__":
    filename = sys.argv[1]
    with open(filename, 'rb') as f:
        data = f.read()
    data_rev = bytes(functools.reduce(lambda a,b:a+b, zip(data[3::4],data[2::4],data[1::4],data[0::4])))
    crc32 = calc_crc32.checksum(data_rev)
    fletcher_32m = calc_fletcher_32m(data)
    print(f'{crc32:08x}, {fletcher_32m:08x}')
    c1 = int.to_bytes(crc32, 4, 'little')
    c2 = int.to_bytes(fletcher_32m, 4, 'little')
    with open(filename, 'ab') as f:
        f.write(c1)
        f.write(c2)
