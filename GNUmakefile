include include.gmk

##### Rules #####

.PHONY: all float dirs inc clean cleanobj main docs

all: float inc main
	@echo "[NTagLib] Done!"

float:
ifneq "$(origin USE_DOUBLE)" "undefined"
	$(info [NTagLib] Using double instead of float!)
endif

dirs:
	@mkdir -p lib include bin obj

inc: dirs
	@cp `find src/ -name '*.hh'` include

include/%.hh:
	@:

SRCS = $(sort $(shell find src -name '*.cc'))
OBJS = $(patsubst src/%, obj/%.o, $(basename $(SRCS)))
FORTRANSRCS = $(sort $(shell find src -name '*.F'))
FORTRANOBJS = $(patsubst src/%, obj/%.o, $(basename $(FORTRANSRCS)))
INC := $(addprefix -I , $(sort $(dir $(shell find src -name '*.hh'))))

$(OBJS): obj/%.o: src/%.cc src/%.hh
	@mkdir -p $(@D)
	@echo "[NTagLib] Building library: $(word $(words $(subst /, , $*)), $(subst /, , $*))..."
	@$(CXX) $(CXXFLAGS) -o $@ -c $< $(INC) $(ROOTINCLUDE) $(SKOFLINCLUDE) $(ATMPDINCLUDE)

$(FORTRANOBJS): obj/%.o: src/%.F
	@mkdir -p $(@D)
	@echo "[NTagLib] Building FORTRAN code: $*..."
	@$(FC) $(FCFLAGS) -c $< -o $@ -I $(SKOFL_ROOT)/inc -I $(SKOFL_ROOT)/inc/lowe -I $(ATMPD_ROOT)/inc

lib/libNTagLib.a: $(OBJS) $(FORTRANOBJS)
	@echo "[NTagLib] Building static shared library..."
	@ar crf $@ $^

lib/libNTagLib_double.a: $(OBJS) $(FORTRANOBJS)
	@echo "[NTagLib] Building static shared library (for double)..."
	@ar crf $@ $^

clean:
	@rm -rf obj lib include bin

cleanobj:
	@rm -rf obj

# main

MAINSRCS = $(wildcard main/*.cc)
MAINOBJS = $(patsubst main/%.cc, obj/main/%.o, $(MAINSRCS))
MAINBINS = $(patsubst main/%.cc, bin/%, $(MAINSRCS))

main: $(MAINBINS)
	
$(MAINOBJS): obj/main/%.o: main/%.cc lib/libNTagLib.a
	@mkdir -p obj/main
	@$(CXX) $(CXXFLAGS) -o $@ -c $< $(INC) $(ROOTINCLUDE) $(SKOFLINCLUDE) $(ATMPDINCLUDE)
	
$(MAINBINS): bin/%: obj/main/%.o
	@mkdir -p bin
	@echo "[NTagLib] Building executable: $(word $(words $(subst /, , $*)), $(subst /, , $*))..."
	@LD_RUN_PATH=$(ROOTSYS)/lib:$(SKOFL_ROOT)/lib $(CXX) -o $@ $^ $(ATMPDLIB) -L lib -lNTagLib $(ATMPDLIB) $(SKOFLLIB) $(ROOTLIB) $(CERNLIB) $(CXXFLAGS)

double: CXXFLAGS+=-DUSE_DOUBLE=1
double: cleanobj dirs inc lib/libNTagLib_double.a

docs:
	@cd docs/doxygen && doxygen Doxyfile