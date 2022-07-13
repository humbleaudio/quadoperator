# Copyright 2013 Olivier Gillet.
#
# Author: Olivier Gillet (ol.gillet@gmail.com)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# 
# See http://creativecommons.org/licenses/MIT/ for more information.

# System specifications
F_CRYSTAL      = 24576000L
F_CPU          = 212336640L
SYSCLOCK       = SYSCLK_FREQ_HSE
FAMILY         = f7xx
SUB_FAMILY     = STM32F767xx
DENSITY        =
MEMORY_MODE    = flash
# USB            = enabled
# EVAL           = enabled
# DEBUG          = enabled
# TEST           = enabled

OCD_DEBUG_SERVER_CMD  = /usr/local/bin/openocd \
		-f quadop/openocd.cfg \
		-c "init" \
		-c "reset" \
		-c "halt"


# Preferred upload command
UPLOAD_COMMAND  = upload_combo_jtag

# Packages to build
TARGET         = quadop
PACKAGES       = quadop quadop/drivers quadop/dsp lib lib/dsp lib/math test stmlib/system
RESOURCES      = quadop/resources

include stmlib/makefile.inc


# Rules for building the SysEx update file.
SYSEX_FLAGS    = --page_size=512 --device_id=11

HEX2SYSEX = python tools/hex2sysex/hex2sysex.py

$(BUILD_DIR)%.syx: $(BUILD_DIR)%.bin
	$(HEX2SYSEX) $(SYSEX_FLAGS) --syx -o $@ $<

syx: $(BUILD_DIR)$(TARGET).syx
