BINA = boot1
BINB = bootstrap
OBJS = crt0.o go.o disable.o warez_load.o 
OBJS_B1 = startup.o main.o video.o maple.o biossymbol.o scramble.o sleep.o crt1.o go.o disable.o warez_load.o patch.o patcher.o graphics.o menu.o

include ./Makefile.config

INCS=
LIBS= -lgcc

all: $(BINA).bin #$(BINB).bin
	ls -l *.bin

IP:
	./maketmpl IP.BIN boot1.bin IP.BIN.HAK ;echo ;

%.bin: %.elf
	strip-bin $(basename $<) $@

$(BINA).elf: $(OBJS_B1)
	$(CC) -Xlinker -Map=MAP_bin1.map $(DREAM_CFLAGS) $(DREAM_LDFLAGS) -o $@ $(OBJS_B1)

#$(BINB).elf: $(OBJS)
#	$(CC) -Map=MAP_bootstrap.map -Wl,-Ttext,8c00e000 -nostdlib -nostartfiles -o $@ $(OBJS) $(LIBS)

%.o: %.c 
	$(CC) -fomit-frame-pointer $(DREAM_CFLAGS) $(DREAM_LDFLAGS)  -c $< -o $@
%.o: %.S
	$(CC) -fomit-frame-pointer -Wa,-little $(DREAM_CFLAGS) $(DREAM_LDFLAGS)  -c $< -o $@
%.o: %.s
	$(AS) -little   $< -o $@
	
clean:
	-rm -f *.o *.elf 1ST_READ.BIN *.bck $(EXTRA_CLEAN)

reallyclean: clean
	-rm -f *.bin *.srec

force:
	-rm -f $(BIN).elf
	make

