include include.gmk

##### Rules #####

.PHONY: all float dirs inc clean cleanobj main

all: float dirs inc main
	@echo "[NTagLib] Done!"

float:
ifneq "$(origin USE_DOUBLE)" "undefined"
	$(info [NTagLib] Using double instead of float!)
endif

dirs:
	@mkdir -p lib include bin

inc: dirs
	@cp `find src/ -name '*.hh'` include

include/%.hh:
	@:

SRCS = $(wildcard src/*.cc) $(wildcard src/*/*.cc) $(wildcard src/*/*/*.cc)
OBJS = $(patsubst src/%.cc, src/%.o, $(SRCS))
FORTRANSRCS = $(wildcard src/*.F) $(wildcard src/*/*.F) $(wildcard src/*/*/*.F)
FORTRANOBJS = $(patsubst src/%.F, src/%.o, $(FORTRANSRCS))
INC := $(addprefix -I , $(sort $(dir $(shell find src -name '*.hh'))))

$(OBJS): src/%.o: src/%.cc src/%.hh
	@echo "[NTagLib] Building library: $(word $(words $(subst /, , $*)), $(subst /, , $*))..."
	@$(CXX) $(CXXFLAGS) -o $@ -c $< $(INC) $(ROOTINCLUDE) $(SKOFLINCLUDE) $(ATMPDINCLUDE)

$(FORTRANOBJS): src/%.o: src/%.F
	@echo "[NTagLib] Building FORTRAN code: $*..."
	@$(FC) $(FCFLAGS) -c $< -o $@ -I $(SKOFL_ROOT)/inc -I $(SKOFL_ROOT)/inc/lowe -I $(ATMPD_ROOT)/inc

lib/libNTagLib.a: $(OBJS) $(FORTRANOBJS)
	@echo "[NTagLib] Building static shared library..."
	@ar crf $@ $^

lib/libNTagLib_double.a: $(OBJS) $(FORTRANOBJS)
	@echo "[NTagLib] Building static shared library (for double)..."
	@ar crf $@ $^

clean:
	@rm -rf src/*.o src/*/*.o src/*/*/*.o lib include bin

cleanobj:
	@rm -rf src/*.o src/*/*.o src/*/*/*.o

# main

MAINSRCS = $(wildcard main/*.cc)
MAINOBJS = $(patsubst main/%.cc, main/%.o, $(MAINSRCS))
MAINBINS = $(patsubst main/%.cc, bin/%, $(MAINSRCS))

main: $(MAINBINS)
	
$(MAINOBJS): main/%.o: main/%.cc lib/libNTagLib.a
	@$(CXX) $(CXXFLAGS) -o $@ -c $< $(INC) $(ROOTINCLUDE) $(SKOFLINCLUDE) $(ATMPDINCLUDE)
	
$(MAINBINS): bin/%: main/%.o
	@mkdir -p bin
	@echo "[NTagLib] Building executable: $(word $(words $(subst /, , $*)), $(subst /, , $*))..."
	@LD_RUN_PATH=$(TMVASYS)/lib:$(ROOTSYS)/lib $(CXX) -o $@ $^ $(ATMPDLIB) -L lib -lNTagLib $(ATMPDLIB) $(SKOFLLIB) $(ROOTLIB) $(CERNLIB) $(CXXFLAGS)	

double: CXXFLAGS+=-DUSE_DOUBLE=1
double: cleanobj dirs inc lib/libNTagLib_double.a