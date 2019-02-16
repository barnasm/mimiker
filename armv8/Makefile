# vim: tabstop=8 shiftwidth=8 noexpandtab:

TOPDIR = $(realpath ..)

#GENASSYM = $(TOPDIR)/script/genassym.py $(NM)
#GENDEVHINT = $(TOPDIR)/script/gendevhint.py

SOURCES = \
	switch.S 

#OBJECTS += malta_dtb.o

include $(TOPDIR)/build/build.kobj.mk

#build-here: libmips.ka

#$(SOURCES): assym.h
$(SOURCES): 

#%.h: gen%.o
#	@echo "[ASSYM] $(DIR)$@"
#	$(GENASSYM) $< $@

#clean-here:
	#$(RM) assym.h *-devhint.c
