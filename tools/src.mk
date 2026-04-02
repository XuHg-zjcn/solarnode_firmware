sinclude $(TOOL_DIR)/model.mk

CSRC += $(shell find $(DEV_DIR)/Drivers/PY32F0xx_HAL_Driver/Src -name '*.c' | awk -F '_' '{if($$(NF)!="template.c")print $$0}')

SDIR += $(TOP_DIR)/User/Src/
STARTUP_DIR = $(DEV_DIR)/Drivers/CMSIS/Device/PY32F0xx/Source/gcc
