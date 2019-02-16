TOPDIR = $(realpath ../..)

NAME	= rpi3-jtag.elf
KERNEL	= kernel.img
# #CROSS	= aarch64-linux-gnu-
CROSS	= aarch64-mimiker-elf-
CFLAGS	+= -ggdb3 -std=gnu11 -Wall -g -march=armv8-a  -fno-builtin -nostdinc -nostdlib -ffreestanding
LDFLAGS += -Bstatic --gc-sections -nostartfiles -nostdlib -L/home/michal/studia/praca_inz/mimiker/sys
CPPFLAGS += -I/home/michal/studia/praca_inz/mimiker/include/

SOURCES = \
	main.c \
	../switch.S

%.o: %.asm
	${AS} -o $@ $<


#include $(TOPDIR)/build/common.mk
include $(TOPDIR)/build/build.kobj.mk


.PHONY: all
all: $(KERNEL)


$(NAME): ../switch.o main.o startup.o 
	$(TARGET)-ld  $(LDFLAGS) -o $@ -T linkerscript.ld  $^

$(KERNEL): $(NAME)
	${OBJCOPY} --gap-fill=0xff -j .text -j .rodata -j .data -O binary $< $@

clean:
	rm -f $(KERNEL) $(NAME) main.o startup.o

