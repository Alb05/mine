#
# A Makefile that compiles all .c and .s files in "src" and "res" 
# subdirectories and places the output in a "obj" subdirectory
#

# If you move this project you can change the directory 
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
GBDK_HOME = /home/alberto/Documenti/gbdk/

LCC = $(GBDK_HOME)bin/lcc 

# You can set flags for LCC here
# For example, you can uncomment the line below to turn on debug output
LCCFLAGS += -Wa-l
LCCFLAGS += -Wl-m
LCCFLAGS += -Wf--debug
LCCFLAGS += -Wl-y
LCCFLAGS += -Wl-w


# You can set the name of the .gb ROM file here
PROJECTNAME    = mine

SRCDIR      = src
OBJDIR      = bin
MAPS        = src/maps
TILES       = src/tiles
BINS	    = $(OBJDIR)/$(PROJECTNAME).gb
CSOURCES    = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.c))) $(foreach dir,$(MAPS),$(notdir $(wildcard $(dir)/*.c))) $(foreach dir,$(TILES),$(notdir $(wildcard $(dir)/*.c)))
OBJS        = $(CSOURCES:%.c=$(OBJDIR)/%.o)

all:	prepare $(BINS)

# Compile .c files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.c
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# Compile .c files in "maps/" to .o object files
$(OBJDIR)/%.o:	$(MAPS)/%.c
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# Compile .c files in "tiles/" to .o object files
$(OBJDIR)/%.o:	$(TILES)/%.c
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# Link the compiled object files into a .gb ROM file
$(BINS):	$(OBJS)
	$(LCC) $(LCCFLAGS) -o $(BINS) $(OBJS)

prepare:
	mkdir -p $(OBJDIR)

clean:
#	rm -f  *.gb *.ihx *.cdb *.adb *.noi *.map
	rm -f  $(OBJDIR)/*.*

