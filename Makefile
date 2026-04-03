#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:


#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET		:=	$(notdir $(CURDIR))
BUILD		:=	build
SOURCES		:=	source libraries
DATA		:=	data data/fonts data/animated data/objects data/glow data/portals data/icons data/levels data/sfx data/back_grounds data/perspective data/menu
INCLUDES	:=  libraries

include $(patsubst %/$(BUILD),%,$(CURDIR))/wii_rules

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------


WII_FLAGS = -fomit-frame-pointer -ffast-math -fno-math-errno -ffinite-math-only -fno-strict-aliasing -fno-align-functions -fno-align-labels -fno-align-loops -fno-align-jumps
CFLAGS	= -g -O2 -Wall $(MACHDEP) $(INCLUDE) -MMD -MP -I$(DEVKITPRO)/libogc2/wii/include/ogc $(WII_FLAGS)
CXXFLAGS	=	$(CFLAGS)

LDFLAGS	=	-g $(MACHDEP) -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:= -lPSGL -lpsglu -lcg -lcgGL -lpngdec -lpugixml -laudio -lmp3dec -lm -lz

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
LIBDIRS := \
	$(patsubst %/$(BUILD),%,$(CURDIR))/GRRLIB/GRRLIB/GRRLIB \
	$(PORTLIBS)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CFILES		:=	$(filter-out level_loading.c game.c oggplayer.c custom_mp3player.c,$(CFILES))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
DATA_EXTS   := %.png %.gmd %.ttf %.jpg
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))
BINFILES    :=  $(filter $(DATA_EXTS),$(BINFILES))

ASSET_HEADERS := $(addprefix source/,$(addsuffix .h,$(subst .,_,$(BINFILES))))
ASSET_CFILES  := $(addprefix source/,$(addsuffix .c,$(subst .,_,$(BINFILES))))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES_BIN	:=	$(addsuffix .o,$(subst .,_,$(BINFILES)))
export OFILES_SOURCES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(sFILES:.s=.o) $(SFILES:.S=.o)
export OFILES := $(OFILES_BIN) $(OFILES_SOURCES)

export HFILES := $(ASSET_HEADERS)

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES), -iquote $(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD) \
					-I$(LIBOGC_INC)

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:= -L$(LIBOGC_LIB) $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: $(BUILD) clean

ifeq (,$(wildcard $(DEVKITPRO)/portlibs/ppc/include/tremor/ivorbiscodec.h))

$(BUILD):
	@echo
	@echo "*------------------------------------------------------------------------------------------*"
	@echo
	@echo "Please install libvorbisidec using (dkp-)pacman -S ppc-libvorbisidec"
	@echo
	@echo "*------------------------------------------------------------------------------------------*"
	@echo
else

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

endif
#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).dol

#---------------------------------------------------------------------------------
run:
	wiiload $(TARGET).dol


#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------

# Put the response file in the build dir so it gets cleaned with other intermediates
OBJFILE := $(DEPSDIR)/objects.rsp

# Helper for a newline
define NL :=


endef

$(OUTPUT).dol: $(OUTPUT).elf

$(OUTPUT).elf: $(OBJFILE) 
	$(SILENTMSG) linking ... $(notdir $@)
	$(SILENTCMD)$(LD) @$(OBJFILE) $(LDFLAGS) $(LIBPATHS) $(LIBS) -o $@

$(OBJFILE): $(OFILES)
	$(SILENTMSG) objecting ... $(notdir $@)
	@$(file >$@,$(foreach o,$(OFILES),$(o)$(NL)))


$(OFILES_SOURCES) : $(HFILES)

#---------------------------------------------------------------------------------
# PS3 SNC conversion using the automated tool
#---------------------------------------------------------------------------------
BIN_TO_HEADER := python3 tools/bin_to_header.py

$(HFILES) $(ASSET_CFILES):
	@echo "Auto-converting assets..."
	@$(BIN_TO_HEADER)

# Compile the generated C files
source/%_png.o: source/%_png.c
	@echo $(notdir $<)
	$(CC) $(CFLAGS) -c $< -o $@

source/%_gmd.o: source/%_gmd.c
	@echo $(notdir $<)
	$(CC) $(CFLAGS) -c $< -o $@


source/%_ttf.o: source/%_ttf.c
	@echo $(notdir $<)
	$(CC) $(CFLAGS) -c $< -o $@

source/%_jpg.o: source/%_jpg.c
	@echo $(notdir $<)
	$(CC) $(CFLAGS) -c $< -o $@

source/all_assets.h:
	@echo "Generating all_assets.h"
	@python3 tools/generate_all_assets_h.py

$(OFILES_SOURCES) : source/all_assets.h

source/%.o: source/%.cpp
	@echo $(notdir $<)
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
