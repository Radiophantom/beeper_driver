obj-m += mem.o
EXTRA_CFLAGS := -I$(src)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

