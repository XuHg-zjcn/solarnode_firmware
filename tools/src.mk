sinclude $(TOOL_DIR)/model.mk

CSRC += $(shell find $(DEV_DIR)/Drivers/PY32F0xx_HAL_Driver/Src -name '*.c' | awk -F '_' '{if($$(NF)!="template.c")print $$0}')

SDIR += $(TOP_DIR)/User/Src/
STARTUP_DIR = $(DEV_DIR)/Drivers/CMSIS/Device/PY32F0xx/Source/gcc
ASRC += $(shell ls $(STARTUP_DIR)/startup_*.s | awk '{if(!match($$0, /startup_py32(.+)\.s$$/)){next};a=substr($$0, RSTART+8, RLENGTH-10);gsub("x",".",a);if(match(tolower("$(MODEL_D)"),a)){print $$0}}')
