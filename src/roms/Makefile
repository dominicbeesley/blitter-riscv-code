		include ../Makefile.defs

SSD=$(SSDS)/romsriscV.ssd
DEPLOY=$(DEPLOY_TOP)/romsriscV

SUBDIRS=testmos
OBJS=testmos/testmos.mos
EXTRAS=$(wildcard extras/*.inf)

OBJS_O=$(addprefix $(BUILD)/, $(OBJS))


.PHONY: clean all $(SUBDIRS)

all: $(SUBDIRS)
ssd: all $(SSD)

clean: $(SUBDIRS)
	-rm $(SSD)

deploy: ssd
	mkdir -p $(DEPLOY)
	dfs read -i -d $(DEPLOY) $(SSD)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

$(SSD):	$(OBJS_O) $(EXTRAS)
	dfs form -80 $(SSD)
	dfs title $(SSD) "romsRiscV"
	dfs add $(SSD) $(addsuffix .inf, $(OBJS_O)) $(EXTRAS)

