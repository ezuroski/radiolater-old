
OBJLIST = vectors.o \
          gpio.o \
          radio.o

TARGET = radio

ARMGNU ?= arm-none-eabi

COPS = -Wall -O2 -nostdlib -nostartfiles -ffreestanding

gcc : $(TARGET).hex $(TARGET).bin

all : gcc 

clean :
	rm -f *.o
	rm -f *.bin
	rm -f *.hex
	rm -f *.elf
	rm -f *.list
	rm -f *.img
	rm -f *.bc
	rm -f *.clang.opt.s

%.o : %.s
	$(ARMGNU)-as $< -o $@

%.o : %.c
	$(ARMGNU)-gcc $(COPS) -c $< -o $@

$(TARGET).elf : memmap $(OBJLIST)
	$(ARMGNU)-ld $(OBJLIST) -T memmap -o $@
	$(ARMGNU)-objdump -D $@ > $(TARGET).list

$(TARGET).bin : $(TARGET).elf
	$(ARMGNU)-objcopy $< -O binary $@

$(TARGET).hex : $(TARGET).elf
	$(ARMGNU)-objcopy $< -O ihex $@






