BIN = boot1
OBJS = startup.o libc.o main.o graphics.o video.o maple.o biossymbol.o scramble.o sleep.o crt1.o go.o disable.o warez_load.o menu.o sq.o cache.o

include ./Makefile.config

INCS=
LIBS= -lgcc

all: $(BIN).bin
	ls -l *.bin

IP:
	./maketmpl IP.BIN boot1.bin IP.BIN.HAK ;echo ;

%.bin: %.elf
	strip-bin $(basename $<) $@

$(BIN).elf: $(OBJS)
	$(CC) -Xlinker -Map=MAP_bin1.map $(DREAM_CFLAGS) $(DREAM_LDFLAGS) -o $@ $(OBJS)
	
%.o: %.c 
	$(CC) -fomit-frame-pointer $(DREAM_CFLAGS) $(DREAM_LDFLAGS)  -c $< -o $@
%.o: %.S
	$(CC) -fomit-frame-pointer -Wa,-little $(DREAM_CFLAGS) $(DREAM_LDFLAGS)  -c $< -o $@
%.o: %.s
	$(AS) -little   $< -o $@
	
clean:
	-rm -f *.o *.elf 1ST_READ.BIN *.bck $(EXTRA_CLEAN)

reallyclean: clean
	-rm -f $(BIN).bin rm IP.BIN.HAK

force:
	-rm -f $(BIN).elf
	make

