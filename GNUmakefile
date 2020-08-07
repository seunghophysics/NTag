include $(SKOFL_ROOT)/config.gmk
#include $(ATMPD_ROOT)/config.gmk

#SHELL = /bin/bash
shelltype = setup

NTAG_GD_ROOT = $(shell pwd)

ROOT_INC     = $(shell root-config --cflags)
ROOT_LIBS    = $(shell root-config --libs)

OLD_ROOT     = $(ATMPD_ROOT)/src/analysis/neutron/ntag_gd
TMVASYS      = /disk02/usr6/han/Apps/TMVA
TMVALIB      = $(TMVASYS)/lib

LOCAL_INC    = -I$(NTAG_GD_ROOT)/include -I$(ATMPD_ROOT)/src/recon/fitqun \
			   -I$(OLD_ROOT) \
			   -I$(TMVASYS)/inc \
				 -I$(SKOFL_ROOT)/include -I$(SKOFL_ROOT)/inc/lowe


FORTRANINCLUDES += -I$(SKOFL_FORTRAN_INCDIR)/lowe

LOCAL_LIBS   = $(APLIB) \
			   -lsklowe_7.0 -lsollib_4.0 -lwtlib_5.1 -lbonsai_3.3 -lstmu -lska \
			   -L$(TMVASYS)/lib -lTMVA.1 \
			   $(ROOT_LIBS) -lMinuit -lXMLIO -lMLP

APLIB =  -lapdrlib -laplib -lringlib -ltp -ltf -lringlib \
	 -laplib -lmsfit -lmslib -lseplib -lmsfit -lprtlib -lmuelib \
	 -lffit -lodlib -lstmu -laplowe -laplib -lfiTQun -ltf -lmslib -llelib -lntuple_t2k

CXXFLAGS += -std=c++11

SRCS = $(wildcard src/*.cc)
OBJS = $(patsubst src/%.cc, obj/%.o, $(SRCS))

bin/main: obj/bonsai.o $(OBJS) obj/main.o bin out
	@echo "[NTag] Building the main program..."
	@LD_RUN_PATH=$(SKOFL_LIBDIR):$(A_LIBDIR):$(TMVALIB) $(CXX) $(CXXFLAGS) -o $@ $(OBJS) obj/bonsai.o obj/main.o $(LDLIBS)
	@echo "[NTag] Done!"

obj/bonsai.o: src/bonsai.F obj
	@echo "[NTag] Building BONSAI..."
	@$(FC) $(FCFLAGS) -c $< -o $@

obj/main.o: main.cc obj
	@echo "[NTag] Building main..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o: src/%.cc obj
	@echo "[NTag] Building $*..."
	@LD_RUN_PATH=$(A_LIBDIR) $(CXX) $(CXXFLAGS) -c $< -o $@


bin obj out:
	@mkdir $@


.PHONY: clean
	
clean:
	@$(RM) -rf *.o *~ .rootrc *.rootmap *.C *.log obj bin
