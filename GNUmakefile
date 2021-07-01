##### VARIABLES #####

CXX = g++
CXXFLAGS = -std=c++11 -fPIC -g -O0

FC = gfortran
FCFLAGS += -w -fPIC -lstdc++

SKOFL_ROOT = /home/skofl/sklib_gcc4.8.5/skofl-trunk/
ATMPD_ROOT = /home/skofl/sklib_gcc4.8.5/atmpd-trunk/

SKOFLINCLUDE = -I $(SKOFL_ROOT)/include -I src/SKLibrary
SKOFLLIB = -L $(SKOFL_ROOT)/lib -lgeom -lskrd -lastro -lzbs -lgeom -lsklib -llibrary -liolib -lrfa -lmon \
								-lskroot -lDataDefinition -ltqrealroot -lloweroot -latmpdroot -lmcinfo -lsofttrgroot -lidod_xtlk_root
ATMPDINCLUDE = -I $(ATMPD_ROOT)/include -I $(OLD_NTAG_GD_ROOT) -I $(ATMPD_ROOT)/src/recon/fitqun
ATMPDLIB = -L $(ATMPD_ROOT)/lib -lapdrlib -lhutl -laplib -lringlib -ltp -ltf -lringlib \
		   -laplib -lmsfit -lmslib -lseplib -lmsfit -lprtlib -lmuelib \
		   -lffit -lodlib -lstmu -laplowe -laplib -lfiTQun -ltf -lmslib -llelib -lntuple_t2k
ROOTINCLUDE = -I $(ROOTSYS)/include
ROOTLIB = $(shell root-config --libs)
CERNLIB = `cernlib graflib grafX11 packlib mathlib kernlib lapack3 blas` -L $(CERN_ROOT)/lib -ljetset74

##### Rules #####

all: dirs inc lib/libutillib.a
	@echo "[UtilLib] Done!"

dirs:
	@mkdir -p lib include

inc:
	@cp `find src/ -name '*.hh'` include

include/%.hh:
	@:

SRCS = $(wildcard src/*.cc) $(wildcard src/*/*.cc) $(wildcard src/*/*/*.cc)
OBJS = $(patsubst src/%.cc, src/%.o, $(SRCS))
FORTRANOBJS = $(patsubst src/SKIO/%.F, src/SKIO/%.o, $(wildcard src/SKIO/*.F))
INC := $(addprefix -I , $(sort $(dir $(shell find src -name '*.hh'))))

$(OBJS): src/%.o: src/%.cc src/%.hh
	@echo "[UtilLib] Building utility library: $(word $(words $(subst /, , $*)), $(subst /, , $*))..."
	@$(CXX) $(CXXFLAGS) -o $@ -c $< $(INC) $(ROOTINCLUDE) $(SKOFLINCLUDE)
	
$(FORTRANOBJS): src/SKIO/%.o: src/SKIO/%.F
	@echo "[UtilLib] Building $*..."
	@$(FC) $(FCFLAGS) -c $< -o $@ -I $(SKOFL_ROOT)/inc -I $(ATMPD_ROOT)/inc -I $(SKOFL_ROOT)/lowe

lib/libutillib.a: $(OBJS) $(FORTRANOBJS)
	@echo "[UtilLib] Building static shared library..."
	@ar crf $@ $^

clean:
	@rm -rf src/*.o src/*/*.o src/*/*/*.o lib include