
PKG_NAME := TestMasterSlave_codegen

CF_BASE_DIR := $(EXTERNAL_BUILD_DIR)/project/canfestival/libcanfestival
CF_INSTALL_DIR := $(wildcard $(CF_BASE_DIR)/canfestival-*)
CF_EXAMPLE_DIR := $(CF_INSTALL_DIR)/examples/TestMasterSlave

INSTALL_INC_DIR := $(PKG_INSTALL_DIR)/include

$(CONFIGURE) :
	$(MKDIR) $(INSTALL_INC_DIR)

	python3 $(CF_INSTALL_DIR)/objdictgen/objdictgen.py $(CF_EXAMPLE_DIR)/TestSlave.dcf $(INSTALL_INC_DIR)/TestSlave.c
	python3 $(CF_INSTALL_DIR)/objdictgen/objdictgen.py $(CF_EXAMPLE_DIR)/TestMaster.dcf $(INSTALL_INC_DIR)/TestMaster.c

	touch $@

$(BUILD) :
	touch $@

$(INSTALL) :
	$(MKDIR) $(INSTALL_INC_DIR)

	$(CP) $(CF_EXAMPLE_DIR)/Slave.h $(CF_EXAMPLE_DIR)/TestSlave.h $(CF_EXAMPLE_DIR)/Master.h $(CF_EXAMPLE_DIR)/TestMaster.h $(CF_EXAMPLE_DIR)/TestMasterSlave.h $(INSTALL_INC_DIR)/
	$(CP) $(CF_EXAMPLE_DIR)/TestMasterSlave.c $(CF_EXAMPLE_DIR)/Slave.c $(CF_EXAMPLE_DIR)/Master.c $(INSTALL_INC_DIR)/

	sed -i -e '1i #include "canfestival.h"\n#include "nmtMaster.h"' \
		-e 's/UNS32 SINC_cicle=0;/__attribute__((unused)) UNS32 SINC_cicle=0;/' \
		-e 's/UNS8 data_type = 0;/__attribute__((unused)) UNS8 data_type = 0;/' \
		-e 's/UNS8 res;/__attribute__((unused)) UNS8 res;/' \
		$(INSTALL_INC_DIR)/Master.c

	sed -i '1i #include <stdio.h>' $(INSTALL_INC_DIR)/TestMasterSlave.h

	sed -i -e '38i #include "timers_driver.h"\n#include "nmtMaster.h"' \
		-e '/LibraryPath=/d' \
		-e '/LibraryPath = optarg;/d' \
		-e '/#ifndef NOT_USE_DYNAMIC_LOADING/,/#endif/d' \
		-e 's/pause();/sleep(100000);/g' \
		$(INSTALL_INC_DIR)/TestMasterSlave.c

	touch $@
