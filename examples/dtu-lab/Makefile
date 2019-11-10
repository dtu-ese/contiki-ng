CONTIKI_PROJECT = dtu-lab

PLATFORMS_ONLY = cc26x0-cc13x0

all: $(CONTIKI_PROJECT)

CONTIKI = ../..

include $(CONTIKI)/Makefile.dir-variables
MODULES += $(CONTIKI_NG_SERVICES_DIR)/shell

MAKE_ROUTING=MAKE_ROUTING_NULLROUTING

include $(CONTIKI)/Makefile.include
