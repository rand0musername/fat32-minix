# Makefile for FAT32 Server (fat32s)
PROG=	fat32s
SRCS=	main.c fat32.c read.c mount.c print.c util.c tree.c

DPADD+=	${LIBSYS}
LDADD+=	-lsys -ltimers
CFLAGS+= -std=c99

# To help avoid rebuilding the whole kernel image

BOOT_FILE = /boot/minix_latest/mod11_fat32s.gz
hdboot: $(PROG)
	rm -rf staging
	echo Creating empty staging directory...
	mkdir -p staging
	echo Copying binary to staging...
	cp $(PROG) staging/$(PROG)
	echo Stripping symbols from the binary...
	strip --strip-all staging/$(PROG)
	echo Gzipping binary...
	gzip staging/$(PROG)
	echo Copying binary from staging to boot directory...
	cp -f staging/$(PROG).gz $(BOOT_FILE)
	echo Done. Reboot to run the new server.

hdreboot: hdboot
	reboot

.include <minix.service.mk>