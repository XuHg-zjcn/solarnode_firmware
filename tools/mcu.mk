#############################################################################
# MCU操作
# Copyright (C) 2022-2024  Xu Ruijun
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#############################################################################

stm32flash_flash: $(TARGET).hex
	@$(STM32FLASH) -w "$<" -v -g 0x0 /dev/ttyUSB0

stlink_flash: $(TARGET).bin
	@$(STLINK) write "$<" 0x08000000

erase: $(MCU_OP)_erase
verify: $(MCU_OP)_verify
reset: $(MCU_OP)_reset
flash: $(MCU_OP)_flash
