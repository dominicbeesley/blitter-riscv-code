		include ../Makefile.defs

ASMINCS=$(wildcard $(TOP)/asm-includes/*.inc) $(filter-out version-date.inc, $(wildcard *.inc))
DEPS=$(INCS) $(ASMINCS)
OBJS=	start module_test


TARGETS=module_test

TARGETS_NODEPS=clean

OBJS_O = $(addprefix $(BUILD)/, $(addsuffix .o, $(OBJS)))
TARGETS_O = $(addprefix $(BUILD)/, $(addsuffix .mos, $(TARGETS)))

DEPFLAGS = -MT $@ -MMD -MP -MF $(BUILD)/$*.d



.PHONY: all clean

all:	$(TARGETS_O)
ssd:	all
deploy:	all

.PRECIOUS:$(BUILD)/%.o $(BUILD)/%.s $(BUILD)/%.d $(BUILD)/%.elf


RELFLAGS=-fPIE -fPIC

$(BUILD)/%.s:	%.c 
	$(RISCV_CC) $(RISCV_CC_FLAGS) $(DEPFLAGS) $(RELFLAGS) -DDEBUG_PRINT -fomit-frame-pointer -O3 -Os -c -S -o $@ -g $<

$(BUILD)/%.o: 	$(BUILD)/%.s $(DEPS) 
	$(RISCV_AS) $(RISCV_AS_FLAGS) $(RELFLAGS) -c -o $@ -g -Wa,-alghmns=$(basename $@).lst $<


$(BUILD)/%.o: 	%.S $(DEPS) 
	$(RISCV_AS) $(RISCV_AS_FLAGS) $(RELFLAGS) -c -o $@ -g -Wa,-alghmns=$(basename $@).lst $<

$(BUILD)/%.elf: $(OBJS_O) %.lds
	$(RISCV_LD) $(RISCV_LD_FLAGS) --static-pie $(RELFLAGS) --specs=nano.specs -Os -nostartfiles -o $@ \
	-Wl,--build-id=none,-Bstatic,-T,$(filter %.lds, $^),-Map,$(basename $@).map,--strip-debug \
		$(filter %.o,$^) -lgcc 
	$(RISCV_NM) --format=posix $@ >$(basename $@).sy1
	sed -n -E 's/^(\w+)\s+\w\s+([0-9a-f]+)/DEF \1 \2/ip' <$(basename $@).sy1 >$(basename $@).noi


$(BUILD)/module_test.mos: $(BUILD)/module_test.elf
	$(RISCV_OBJCOPY) -O binary $< $@
	echo "M.MODTEST FFFFFFFF FFFFFFFF" >$@.inf

#DEPFILES := $(OBJS_O:%.o=%.d)
#$(DEPFILES):

#include $(wildcard $(DEPFILES))


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


 