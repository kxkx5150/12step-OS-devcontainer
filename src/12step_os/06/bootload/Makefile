PREFIX  = /usr/local
ARCH    = h8300-elf
BINDIR  = $(PREFIX)/bin
ADDNAME = $(ARCH)-

AR      = $(BINDIR)/$(ADDNAME)ar
AS      = $(BINDIR)/$(ADDNAME)as
CC      = $(BINDIR)/$(ADDNAME)gcc
LD      = $(BINDIR)/$(ADDNAME)ld
NM      = $(BINDIR)/$(ADDNAME)nm
OBJCOPY = $(BINDIR)/$(ADDNAME)objcopy
OBJDUMP = $(BINDIR)/$(ADDNAME)objdump
RANLIB  = $(BINDIR)/$(ADDNAME)ranlib
STRIP   = $(BINDIR)/$(ADDNAME)strip

H8WRITE = ../../tools/h8write/h8write

# FreeBSD-4.x:/dev/cuaaX, FreeBSD-6.x:/dev/cuadX, FreeBSD(USB):/dev/cuaUx
# Linux:/dev/ttySx, Linux(USB):/dev/ttyUSBx, Windows:comX
H8WRITE_SERDEV = /dev/cuad0

OBJS  = vector.o startup.o main.o
OBJS += lib.o serial.o xmodem.o elf.o

TARGET = kzload

CFLAGS = -Wall -mh -nostdinc -nostdlib -fno-builtin
#CFLAGS += -mint32 # int��32�ӥåȤˤ���ȳݻ����任���Ǥ��ʤ��ʤ�
CFLAGS += -I.
#CFLAGS += -g
CFLAGS += -Os
CFLAGS += -DKZLOAD

LFLAGS = -static -T ld.scr -L.

.SUFFIXES: .c .o
.SUFFIXES: .s .o

all : $(TARGET)

$(TARGET) :	$(OBJS)
		mkdir -p ../../../../exe && $(CC) $(OBJS) -o ../../../../exe/$(TARGET) $(CFLAGS) $(LFLAGS)
		cp -f ../../../../exe/$(TARGET) ../../../../exe/$(TARGET).elf
		$(STRIP) ../../../../exe/$(TARGET)

.c.o :		$<
		$(CC) -c $(CFLAGS) $<

.s.o :		$<
		$(CC) -c $(CFLAGS) $<

../../../../exe/$(TARGET).mot :	../../../../exe/$(TARGET)
		$(OBJCOPY) -O srec ../../../../exe/$(TARGET) ../../../../exe/$(TARGET).mot

image :		../../../../exe/$(TARGET).mot

write :		../../../../exe/$(TARGET).mot
		$(H8WRITE) -3069 -f20 ../../../../exe/$(TARGET).mot $(H8WRITE_SERDEV)

clean :
		rm -f $(OBJS) ../../../../exe/$(TARGET) ../../../../exe/$(TARGET).elf ../../../../exe/$(TARGET).mot
