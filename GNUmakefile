SKOFL_ROOT = /usr/local/sklib_gcc8/skofl-trunk/
include $(SKOFL_ROOT)/config.gmk

NTAG_GD_ROOT = $(shell pwd)

ROOT_INC     = $(shell root-config --cflags)
ROOT_LIBS    = $(shell root-config --libs)

OLD_ROOT     = $(ATMPD_ROOT)/src/analysis/neutron/ntag_gd

LOCAL_INC    =  -I$(NTAG_GD_ROOT)/include -I$(ATMPD_ROOT)/src/recon/fitqun \
				-I$(OLD_ROOT) \
				-I $(ROOT_INC) \
				-I$(SKOFL_ROOT)/include -I$(SKOFL_ROOT)/inc/lowe

FORTRANINCLUDES += -I$(SKOFL_FORTRAN_INCDIR)/lowe

LOCAL_LIBS   = $(APLIB) \
			   -lsklowe_7.0 -lsollib_4.0 -lwtlib_5.1 -lbonsai_3.3 -lstmu -lska \
			   $(ROOT_LIBS) -lMinuit -lXMLIO -lMLP -lTreePlayer -lTMVA

SKOFLINCLUDE = -I $(SKOFL_ROOT)/include -I $(SKOFL_ROOT)/include/lowe -I $(SKOFL_ROOT)/lowe/bonsai
SKOFLLIB = -L $(SKOFL_ROOT)/lib -lgeom -lskrd -lastro -lzbs -lgeom -lsklib -llibrary -liolib -L $(SKOFL_ROOT)/src/rfa_dummy -lrfa -L $(SKOFL_ROOT)/lib -lmon \
								-lskroot -lDataDefinition -ltqrealroot -lloweroot -latmpdroot -lmcinfo -lsofttrgroot -lidod_xtlk_root \
								-lbonsai_3.3 -lsklowe_7.0 -lwtlib_5.1 -lsollib_4.0 -lskrd -llibrary -liolib -lsklib

APLIB =  -lapdrlib -laplib -lringlib -ltp -ltf -lringlib \
	 -laplib -lmsfit -lmslib -lseplib -lmsfit -lprtlib -lmuelib \
	 -lffit -lodlib -lstmu -laplowe -laplib -lfiTQun -ltf -lmslib -llelib -lntuple_t2k

CERN_ROOT = /usr/local/sklib_gcc8/cern/2005
CERNLIB = `$(CERN_ROOT)/bin/cernlib graflib grafX11 packlib mathlib kernlib lapack3 blas` -L $(CERN_ROOT)/lib -ljetset74

CXXFLAGS += -std=c++11 -fPIC -g -O0 -lgfortran -Wl,-z -Wl,muldefs -Wno-misleading-indentation
FCFLAGS += -w
FCFLAGS := $(filter-out -I -pthread, $(FCFLAGS))

OBJS = $(patsubst src/%.cc, obj/%.o, $(wildcard src/*.cc))
OBJS += $(patsubst src/%.F, obj/%.o, $(wildcard src/*.F))
OBJS += obj/main.o

all: src/NTagDict.cc lib/libNTag.so bin/NTag

src/NTagDict.cc: include/NTagLinkDef.hh obj
	@echo "[NTag] Building NTagLinkDef..."
	@rootcint -f $@ -c $<
	@$(CXX) $(CXXFLAGS) -c $@ -o obj/NTagDict.o

bin/NTag: $(OBJS) bin out
	@echo "[NTag] Building NTag..."
#	@LD_RUN_PATH=$(TMVALIB):$(SKOFL_LIBDIR):$(ROOTSYS)/lib:$(LIBDIR):$(A_LIBDIR) $(CXX) $(CXXFLAGS) -o $@ $(OBJS) obj/NTagDict.o $(LDLIBS)
	@LD_RUN_PATH=$(TMVALIB):$(SKOFL_LIBDIR):$(ROOTSYS)/lib:$(LIBDIR):$(A_LIBDIR) $(CXX) $(CXXFLAGS) -o $@ $(OBJS) obj/NTagDict.o -L $(ATMPD_ROOT)/lib $(LOCAL_LIBS) $(SKOFLLIB) $(ROOT_LIBS) $(CERNLIB)
	@chmod +x path/bash path/csh
	@if [ ! -d "weights/new" ]; then mkdir weights/new; fi
	@echo "[NTag] Done!"

lib/libNTag.so: $(OBJS) lib
	@echo "[NTag] Building shared library..."
	@$(CXX) $(CXXFLAGS) -o $@ $(OBJS) -shared

obj/main.o: main.cc obj
	@echo "[NTag] Building main..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o: src/%.cc obj
	@echo "[NTag] Building $*..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o: src/%.F obj
	@echo "[NTag] Building $*..."
	@$(FC) $(FCFLAGS) -c $< -o $@

bin obj out lib:
	@mkdir $@

.PHONY: clean

clean:
	@$(RM) -rf $(OBJS) obj bin src/NTagDict.*
