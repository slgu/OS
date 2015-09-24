# Set this to the name of your program
TARGET = ptree_test

# Edit this variable to point to all
# of your sources files (make sure
# to put a complete relative path)
SOURCES = test.c

# -----------------------------------------------
# 
# Don't touch things below here unless
# you know what you're doing :-)
# 
OBJECTS = $(SOURCES:%.c=%.c.o)
INCLUDE =
CFLAGS = -g -Wall --sysroot=$(SYSROOT)
LDFLAGS = --sysroot=$(SYSROOT)
CC = arm-linux-androideabi-gcc
LD = arm-linux-androideabi-gcc

default: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	@echo [Arm-LD] $@
	@$(LD) $(LDFLAGS) $(OBJECTS) -o $@

%.c.o: %.c
	@echo [Arm-CC] $<...
	@$(CC) -c $(CFLAGS) $< -o $@

clean: .PHONY
	@echo [CLEAN]
	@rm -f $(OBJECTS) $(TARGET)

.PHONY:
