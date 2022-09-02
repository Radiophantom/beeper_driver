obj-m += beeper_drv.o
EXTRA_CFLAGS := -I$(src)

all:
	make -C ~/sys_intern/soc-linux M=$(PWD) modules

clean:
	make -C ~/sys_intern/soc-linux M=$(PWD) clean

.PHONY: all clean
