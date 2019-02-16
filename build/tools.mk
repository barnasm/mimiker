
TARGET = mipsel-mimiker-elf
ifdef MIMIKER_ARM
  TARGET = aarch64-mimiker-elf
endif


TOOLCHAIN_FOUND = $(shell which $(TARGET)-gcc > /dev/null; echo $$?)
ifneq ($(TOOLCHAIN_FOUND), 0)
  $(error $(TARGET) toolchain not found. Please refer to README.md)
endif

ifeq ($(TARGET), mipsel-mimiker-elf)
  CC       = $(TARGET)-gcc -mips32r2 -EL -g
  AS       = $(TARGET)-as  -mips32r2 -EL -g
  
else ifeq ($(TARGET), aarch64-mimiker-elf)
  CC       = $(TARGET)-gcc -march=armv8-a -g
  AS       = $(TARGET)-as  -march=armv8-a -g
endif

AR       = $(TARGET)-ar
NM       = $(TARGET)-nm
GDB      = $(TARGET)-gdb
RANLIB	 = $(TARGET)-ranlib
READELF  = $(TARGET)-readelf
OBJCOPY  = $(TARGET)-objcopy
OBJDUMP  = $(TARGET)-objdump
STRIP    = $(TARGET)-strip

CPIO    = cpio --format=crc
CSCOPE  = cscope -b
CTAGS   = ctags
FORMAT  = clang-format-6.0 -style=file 
INSTALL = install -D
RM      = rm -v -f
TAR	= tar
CURL	= curl -J -L
GIT     = git
PATCH   = patch
