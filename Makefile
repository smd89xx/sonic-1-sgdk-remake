export GDK ?= /opt/toolchains/mars/m68k-elf
ifndef CTRMML_DIR
	@echo "Can not find MDSDRV music linker! Please set the CTRMML_DIR environment variable to its path."
	exit 1
endif
MMLNK = $(CTRMML_DIR)/mdslink
MMLNK_ARGS = -o ./res/mdsseq.bin ./res/mdsbin.bin -h ./res/mdsseq.h
mdsdrv:
	$(MMLNK) $(MMLNK_ARGS) res/mus/*.mds
include $(GDK)/makefile.gen