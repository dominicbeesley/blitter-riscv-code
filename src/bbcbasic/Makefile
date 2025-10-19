		include ../Makefile.defs

ASMINCS=$(wildcard $(TOP)/asm-includes/*.inc) $(filter-out version-date.inc, $(wildcard *.inc))
DEPS=$(INCS) $(ASMINCS)
OBJS=crt0 main

SSD=$(SSDS)/bbcbasic-$(FLAVOUR).ssd
DEPLOY=$(DEPLOY_TOP)/romsriscV-$(FLAVOUR)

TARGETS=bbcbasic

TARGETS_NODEPS=clean

OBJS_O = $(addprefix $(BUILD)/, $(addsuffix .o, $(OBJS)))
TARGETS_O = $(addprefix $(BUILD)/, $(addsuffix .bin, $(TARGETS)))

DEPFLAGS = -MT $@ -MMD -MP -MF $(BUILD)/$*.d



.PHONY: all clean

all:	$(TARGETS_O)
ssd: all $(SSD)
deploy:	all

.PRECIOUS:$(BUILD)/%.o $(BUILD)/%.s $(BUILD)/%.d $(BUILD)/%.elf



$(BUILD)/%.s:	%.c 
	$(RISCV_CC) $(RISCV_CC_FLAGS) $(DEPFLAGS) -fomit-frame-pointer -O3 -Os -c -S -o $@ -g $<

$(BUILD)/%.o: 	$(BUILD)/%.s $(DEPS) 
	$(RISCV_AS) $(RISCV_AS_FLAGS) -c -o $@ -g -Wa,-alghmns=$(basename $@).lst $<


$(BUILD)/%.o: 	%.S $(DEPS) 
	$(RISCV_AS) $(RISCV_AS_FLAGS) -I $(BUILD) -c -o $@ -g -Wa,-alghmns=$(basename $@).lst $<

$(BUILD)/%.elf: $(OBJS_O) %.lds
	$(RISCV_LD) $(RISCV_LD_FLAGS) -Os -ffreestanding -nostdlib -o $@ \
	-Wl,--build-id=none,-Bstatic,-T,$(filter %.lds, $^),-Map,$(basename $@).map,--strip-debug \
		$(filter %.o,$^) -lgcc 
	$(RISCV_NM) --format=posix $@ >$(basename $@).sy1
	sed -n -E 's/^(\w+)\s+\w\s+([0-9a-f]+)/DEF \1 \2/ip' <$(basename $@).sy1 >$(basename $@).noi

$(BUILD)/%.S:	%.v
	./vgm2bin.pl $< >$@

$(BUILD)/vgm.o:	$(BUILD)/05_Round_Clear.S



$(BUILD)/bbcbasic.bin: $(BUILD)/bbcbasic.elf
	$(RISCV_OBJCOPY) -O binary $< $@
	echo "$.BBCBAS 00100000 00100000" >$@.inf

#DEPFILES := $(OBJS_O:%.o=%.d)
#$(DEPFILES):

#include $(wildcard $(DEPFILES))

deploy: ssd
	mkdir -p $(DEPLOY)
	dfs read -i -d $(DEPLOY) $(SSD)

$(SSD):	$(TARGETS_O) $(EXTRAS)
	dfs form -80 $(SSD)
	dfs title $(SSD) "bbcbas"
	dfs add $(SSD) $(addsuffix .inf, $(TARGETS_O)) $(filter %.inf, $(EXTRAS))


$(DEPDIR): 
	@mkdir -p $@

clean:
	-rm $(TARGETS_O)
	-rm $(addsuffix .inf, $(TARGETS_O))
	-rm $(patsubst %.mos, %.map, $(TARGETS_O))
	-rm $(patsubst %.mos, %.elf, $(TARGETS_O))
	-rm $(patsubst %.mos, %.sy1, $(TARGETS_O))
	-rm $(patsubst %.mos, %.noi, $(TARGETS_O))
	-rm $(OBJS_O)
	-rm $(patsubst %.o, %.lst, $(OBJS_O))
	-rm $(patsubst %.o, %.s, $(OBJS_O))
	-rm $(patsubst %.o, %.d, $(OBJS_O))

	-rm $(BUILD)/05_Round_Clear.S


