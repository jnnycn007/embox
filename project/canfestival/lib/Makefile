
PKG_NAME    := canfestival
PKG_VER     := 400e119f56bae7730f6942174cc829eb06538319
PKG_SOURCES := https://github.com/embox/canfestival/archive/$(PKG_VER).zip
PKG_MD5     := e3013d5d24e00f6f48d45bb3f33a8620

CF_SOURCE_DIR = $(PKG_SOURCE_DIR)
CF_BUILD_DIR = $(PKG_SOURCE_DIR)/build

INSTALL_INC_DIR := $(PKG_INSTALL_DIR)/include

$(CONFIGURE) :
	$(MKDIR) $(CF_BUILD_DIR) && cd $(CF_BUILD_DIR) && ( \
		cmake $(CF_SOURCE_DIR) \
			-DCMAKE_BUILD_TYPE:STRING=None \
			-DCMAKE_SYSTEM_NAME:STRING=Generic \
			-DCMAKE_C_COMPILER:PATH=$(EMBOX_GCC) \
			-DCMAKE_C_FLAGS="-DCF_MAX_NB_CAN_PORTS=2" \
			-DCF_TARGET=embox \
			-DCF_CAN_DRIVER=embox \
	)

	python3 $(CF_SOURCE_DIR)/objdictgen/objdictgen.py $(CF_SOURCE_DIR)/examples/TestMasterSlave/TestSlave.dcf $(CF_SOURCE_DIR)/examples/TestMasterSlave/TestSlave.c
	python3 $(CF_SOURCE_DIR)/objdictgen/objdictgen.py $(CF_SOURCE_DIR)/examples/TestMasterSlave/TestMaster.dcf $(CF_SOURCE_DIR)/examples/TestMasterSlave/TestMaster.c
	touch $@

$(BUILD) :
	cd $(CF_BUILD_DIR) && ( \
		$(MAKE) MAKEFLAGS='$(EMBOX_IMPORTED_MAKEFLAGS)'; \
	)
	touch $@

$(INSTALL) :
	$(MKDIR) $(INSTALL_INC_DIR)
	$(CP) $(CF_BUILD_DIR)/src/libcanfestival.a $(PKG_INSTALL_DIR)
	$(CP) $(CF_BUILD_DIR)/drivers/libcanfestival_embox.a $(PKG_INSTALL_DIR)
	$(CP) $(CF_BUILD_DIR)/drivers/libcanfestival_can_embox.a $(PKG_INSTALL_DIR)
	$(CP) -r $(CF_BUILD_DIR)/include/* $(INSTALL_INC_DIR)/
	$(CP) -r $(CF_SOURCE_DIR)/include/* $(INSTALL_INC_DIR)/
	$(CP) -r $(CF_SOURCE_DIR)/include/embox/* $(INSTALL_INC_DIR)/
	$(CP) $(CF_SOURCE_DIR)/examples/TestMasterSlave/Slave.h $(INSTALL_INC_DIR)/
	$(CP) $(CF_SOURCE_DIR)/examples/TestMasterSlave/TestSlave.h $(INSTALL_INC_DIR)/
	$(CP) $(CF_SOURCE_DIR)/examples/TestMasterSlave/Master.h $(INSTALL_INC_DIR)/
	$(CP) $(CF_SOURCE_DIR)/examples/TestMasterSlave/TestMaster.h $(INSTALL_INC_DIR)/
	$(CP) $(CF_SOURCE_DIR)/examples/TestMasterSlave/TestMasterSlave.h $(INSTALL_INC_DIR)/
	$(MKDIR) $(PKG_INSTALL_DIR)/examples
	$(CP) $(CF_SOURCE_DIR)/examples/TestMasterSlave/Slave.c $(PKG_INSTALL_DIR)/examples/
	$(CP) $(CF_SOURCE_DIR)/examples/TestMasterSlave/TestSlave.c $(PKG_INSTALL_DIR)/examples/
	$(CP) $(CF_SOURCE_DIR)/examples/TestMasterSlave/Master.c $(PKG_INSTALL_DIR)/examples/
	$(CP) $(CF_SOURCE_DIR)/examples/TestMasterSlave/TestMaster.c $(PKG_INSTALL_DIR)/examples/
	touch $@
