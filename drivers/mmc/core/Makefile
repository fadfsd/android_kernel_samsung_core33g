#
# Makefile for the kernel mmc core.
#

obj-$(CONFIG_MMC)		+= mmc_core.o
<<<<<<< HEAD
mmc_core-y			:= core.o bus.o host.o \
=======
mmc_core-y			:= core-sprd.o bus.o host.o \
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
				   mmc.o mmc_ops.o sd.o sd_ops.o \
				   sdio.o sdio_ops.o sdio_bus.o \
				   sdio_cis.o sdio_io.o sdio_irq.o \
				   quirks.o slot-gpio.o

mmc_core-$(CONFIG_DEBUG_FS)	+= debugfs.o
