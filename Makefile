ifndef CC
CC=gcc
endif
ifndef PICC
PICC=picc
endif
ifndef PICOS_DIR
PICOS_DIR=../picos
endif

CHIP=16F870
XTAL_FREQ=1000000
INCLUDES=-I$(PICOS_DIR) -I.
PICOS_OBJS=$(PICOS_DIR)/picos_time.c $(PICOS_DIR)/io.c $(PICOS_DIR)/utils.c
DRIVERS=7seg.c input.c

all: sim pic

sim:
	$(CC) -DHAVE_CONFIG_H -funsigned-char -g -DLINUX_SIM $(INCLUDES) -o sim_clock sim_clock.c $(PICOS_OBJS) $(DRIVERS) -lncurses -lrt

pic:
	$(PICC) -D_XTAL_FREQ=$(XTAL_FREQ) -DHAVE_CONFIG_H --chip=$(CHIP) $(INCLUDES) -Oclock.hex clock.c $(PICOS_OBJS) $(DRIVERS)