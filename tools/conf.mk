###########################################################################
# 编译高级设置
# Copyright (C) 2021-2024  Xu Ruijun
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#############################################################################

sinclude $(TOOL_DIR)/config.mk

ifeq ($(VERBOSE),YES)
	ASM     = $(TOOL_CHAIN)gcc
	CC      = $(TOOL_CHAIN)gcc
	CXX     = $(TOOL_CHAIN)g++
	LINK    = $(TOOL_CHAIN)g++
	OBJCOPY = $(TOOL_CHAIN)objcopy
else
	ASM     = @echo "ASM      $<"; $(TOOL_CHAIN)gcc
	CC      = @echo "CC       $<"; $(TOOL_CHAIN)gcc
	CXX     = @echo "CXX      $<"; $(TOOL_CHAIN)g++
	LINK    = @echo "LINK     $@"; $(TOOL_CHAIN)g++
	OBJCOPY = @echo "OBJCOPY  $<"; $(TOOL_CHAIN)objcopy
endif


#更多关于编译器参数请见`man gcc`
COMFLAGS += -mcpu=cortex-m0plus
COMFLAGS += -mthumb#              #压缩指令
COMFLAGS += -fmessage-length=0#   #
COMFLAGS += -fsigned-char#        #
COMFLAGS += -ffunction-sections#  #每个函数作为sections
COMFLAGS += -fdata-sections#      #每个数据作为sections
COMFLAGS += -freorder-functions
COMFLAGS += -fno-common#          #
CXXFLAGS += -Wall#                #开启所有警告
COMFLAGS += -Wunused#             #开启无用变量警告
COMFLAGS += -Wuninitialized#      #开启无初始化警告
COMFLAGS += -D$(MODEL_D)#        #定义型号宏供HAL库使用
COMFLAGS += -DUSE_HAL_DRIVER     #TODO: 根据配置设置是否使用HAL
COMFLAGS += -g#                  #调试

CCFLAGS := $(COMFLAGS)
CCFLAGS += -std=gnu99#            #标准
CCFLAGS += -Os#                   #最小文件大小优化

CXXFLAGS := $(COMFLAGS)
CXXFLAGS += -std=gnu++11#         #标准
CXXFLAGS += -Os#                  #最小文件大小优化

ASMFLAGS := $(COMFLAGS)

LDFILE = $(TOOL_DIR)/py32f003x8.ld
LDFLAGS += -nostartfiles#        #不用标准启动文件
LDFLAGS := $(COMFLAGS)
LDFLAGS += -T $(LDFILE)#     #链接器脚本
LDFLAGS += -Xlinker#             #
LDFLAGS += --gc-sections#        #删除无用sections
LDFLAGS += -Wl,-Map,"$(TARGET).map"#链接器参数
LDFLAGS += --specs=nano.specs
LDFLAGS += --specs=nosys.specs

OCDFLAGS += -f $(OCD_CFG)

CSRC ?= $(wildcard ${DIR}/*.c)
CPPSRC ?= $(wildcard ${DIR}/*.cpp)
ASRCS ?= $(wildcard ${DIR}/*.S)

