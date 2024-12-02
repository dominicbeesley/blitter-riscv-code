		include ../../Makefile.defs

ASMINCS=$(wildcard $(TOP)/asm-includes/*.inc) $(filter-out version-date.inc, $(wildcard *.inc))
DEPS=$(INCS) $(ASMINCS)
OBJS=start splash main


TARGETS=testmos

OBJS_O = $(addprefix $(BUILD)/, $(addsuffix .o, $(OBJS)))
TARGETS_O = $(addprefix $(BUILD)/, $(addsuffix .mos, $(TARGETS)))

.PHONY: all clean

all:	$(TARGETS_O)
ssd:	all
deploy:	all

.PRECIOUS:build/*

#$(BUILD)/version-date.inc: $(DEPS) $(filter-out %.da.asm, $(wildcard *.asm))
#	$(SCRIPTS)/gendate.sh $@

#$(BUILD)/blttest.o: $(BUILD)/version-date.inc

$(BUILD)/splash.o:splash.mo7

$(BUILD)/%.s:	%.c
	$(RISCV_CC) $(RISCV_CC_FLAGS) -c -S -o $@ -g $<

$(BUILD)/%.o: 	$(BUILD)/%.s $(DEPS) 
	$(RISCV_AS) $(RISCV_AS_FLAGS) -c -o $@ -g -Wa,-alghmns=$(basename $@).lst $<


$(BUILD)/%.o: 	%.S $(DEPS) 
	$(RISCV_AS) $(RISCV_AS_FLAGS) -c -o $@ -g -Wa,-alghmns=$(basename $@).lst $<

$(BUILD)/%.elf: $(OBJS_O) %.lds
	$(RISCV_LD) $(RISCV_LD_FLAGS) -Os -ffreestanding -nostdlib -o $@ \
	-Wl,--build-id=none,-Bstatic,-T,$(filter %.lds, $^),-Map,$(basename $@).map,--strip-debug \
		$(filter %.o,$^) -lgcc 


$(BUILD)/testmos.mos: $(BUILD)/testmos.elf
	$(RISCV_OBJCOPY) -O binary $< $@
	echo "M.TEST FFFFC000 FFFFC000" >$@.inf



clean:
	-rm $(TARGETS_O)
	-rm $(addsuffix .inf, $(TARGETS_O))
	-rm $(patsubst %.mos, %.map, $(TARGETS_O))
	-rm $(OBJS_O)
	-rm $(patsubst %.o, %.lst, $(OBJS_O))


