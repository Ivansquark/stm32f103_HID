.PHONY: all clean load

#TARGET= src/main.c
TARGET= src/main.cpp
INC= inc/
LIB= lib/
SRC = src/
MCU= cortex-m3
OBJCOPY=arm-none-eabi-objcopy
CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
SIZE=arm-none-eabi-size
CFLAGS=-mthumb -mcpu=$(MCU) $(INC) -g -O0 -Wall -ffreestanding -fno-common -fno-exceptions \
		-fno-unwind-tables -fno-asynchronous-unwind-tables -nostartfiles -nostdlib   
CPPFLAGS=-mthumb -mcpu=$(MCU) -g -O0 -Wall -ffreestanding -fno-common -fno-exceptions \
		-fno-rtti \
		-fno-unwind-tables -fno-asynchronous-unwind-tables -nostartfiles -nostdlib -fno-threadsafe-statics \
		-L/usr/lib/arm-none-eabi/newlib/ 
LDFLAGS = -nostartfiles -lm -lc -lstdc++ \
		 -specs=nano.specs -specs=nosys.specs \
		 -fno-use-cxa-atexit -fno-exceptions -fno-rtti \
		 -L/usr/lib/arm-none-eabi/newlib/thumb/v7-m/ -L/usr/lib/arm-none-eabi/newlib/ \
		 -L/usr/lib/arm-none-eabi/lib/ \
		 -Xlinker -Map=main.map -z muldefs
#-fno-common -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables #for C++ чтобы избежать появления дополнительных секций, связанных с обработкой исключительных ситуаций.
#--gc-sections    #Компоновщик в состоянии избавиться от секций, на которые никто не ссылается и которые не были явно указаны как необходимые в сценарии компоновки.
#-ffunction-sections -fdata-sections   #флаги компилятора, которые разбивают функции и данные в разные секции: когда каждая функция и объект помещены в независимые секции, компоновщик может от них избавиться:
#-ffreestanding #приложение у нас самостоятельное и ни от каких ОС не зависит.
ASFLAGS=-mthumb -mcpu=$(MCU) -g -Wall

load: main.bin
	st-flash write main.bin 0x08000000

all: main.bin main.lst main.elf
	
main.bin: main.elf
	$(OBJCOPY) main.elf main.bin -O binary
	
main.lst: main.elf
	arm-none-eabi-objdump -D main.elf > main.lst

main.elf: startup.o uart.o usb_hid.o main.o
	$(CC) -o main.elf -T lib/stm32f103.ld startup.o uart.o usb_hid.o main.o $(LDFLAGS)
	$(SIZE) main.elf  
#--specs=nosys.specs -g3 -Wl, -Xlinker --gc-sections
#system_stm32f10x.o
#$(LDFL) 
#	--data-sections
#	--specs=nosys.specs

startup.o: lib/startup.cpp
	$(CC) -c  lib/startup.cpp -o startup.o $(CPPFLAGS)
	
uart.o:	src/uart.cpp	
	$(CC) -c src/uart.cpp -o uart.o -I$(INC) -I$(LIB) $(CPPFLAGS)
usb_hid.o: src/usb_hid.cpp
	$(CC) -c src/usb_hid.cpp -o usb_hid.o -I$(INC) -I$(LIB) $(CPPFLAGS)
main.o: $(TARGET) 
	$(CC)  -c $(TARGET) -o main.o -I$(LIB) -I$(INC) $(CPPFLAGS) 
	#arm-none-eabi-objdump main.o -h
	
clean:
	rm -rf *.o *.elf *.lst *.bin *.map
