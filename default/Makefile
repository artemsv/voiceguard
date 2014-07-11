###############################################################################
# Makefile for the project VG
###############################################################################

## General Flags
PROJECT = VG
MCU = atmega128
TARGET = VG.elf
CC = avr-gcc.exe

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2  -fverbose-asm  -O0 -fsigned-char
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=VG.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0


## Include Directories
INCLUDES = -I"C:\WinAVR\avr\include\avr" -I"E:\Programs\Develop\WinAVR\avr\include\avr" 

## Objects that must be built in order to link
OBJECTS = VG.o twimaster.o tim.o lcd.o kbd.o usart.o ds1307.o spi.o XDevMon.o utils.o GSM.o at45.o fatx.o DTMF.o a2d.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) VG.hex VG.eep VG.lss size

## Compile
VG.o: ../VG.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

twimaster.o: ../twimaster.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

tim.o: ../tim.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

lcd.o: ../lcd.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

kbd.o: ../kbd.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

usart.o: ../usart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ds1307.o: ../ds1307.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

spi.o: ../spi.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

XDevMon.o: ../XDevMon.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

utils.o: ../utils.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

GSM.o: ../GSM.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

at45.o: ../at45.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

fatx.o: ../fatx.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

DTMF.o: ../DTMF.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

a2d.o: ../a2d.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) VG.elf dep/* VG.hex VG.eep VG.lss VG.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

