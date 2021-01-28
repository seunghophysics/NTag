SKOFL_ROOT = /home/skofl/sklib_gcc4.8.5/skofl-trunk/
include $(SKOFL_ROOT)/config.gmk

NTAG_GD_ROOT = $(shell pwd)

ROOT_INC     = $(shell root-config --cflags)
ROOT_LIBS    = $(shell root-config --libs)

OLD_ROOT     = $(ATMPD_ROOT)/src/analysis/neutron/ntag_gd
TMVASYS      = /disk02/usr6/han/Apps/TMVA
TMVALIB      = $(TMVASYS)/lib

LOCAL_INC    =  -I$(NTAG_GD_ROOT)/include -I$(ATMPD_ROOT)/src/recon/fitqun \
			    -I$(OLD_ROOT) \
			    -I$(TMVASYS)/inc $(ROOT_INC) \
				-I$(SKOFL_ROOT)/include -I$(SKOFL_ROOT)/inc/lowe

FORTRANINCLUDES += -I$(SKOFL_FORTRAN_INCDIR)/lowe

LOCAL_LIBS   = $(APLIB) \
			   -lsklowe_7.0 -lsollib_4.0 -lwtlib_5.1 -lbonsai_3.3 -lstmu -lska \
			   -L$(TMVASYS)/lib -lTMVA.1 \
			   $(ROOT_LIBS) -lMinuit -lXMLIO -lMLP -lTreePlayer

APLIB =  -lapdrlib -laplib -lringlib -ltp -ltf -lringlib \
	 -laplib -lmsfit -lmslib -lseplib -lmsfit -lprtlib -lmuelib \
	 -lffit -lodlib -lstmu -laplowe -laplib -lfiTQun -ltf -lmslib -llelib -lntuple_t2k

CXXFLAGS += -std=c++11

SRCS = $(wildcard src/*.cc)
OBJS = $(patsubst src/%.cc, obj/%.o, $(SRCS))

all: src/NTagDict.cc bin/NTag lib/libNTag.so

src/NTagDict.cc: include/NTagLinkDef.hh obj
	@echo "[NTag] Building NTagLinkDef..."
	@rootcint -f $@ -c $<
	@$(CXX) $(CXXFLAGS) -c $@ -o obj/NTagDict.o
	
bin/NTag: obj/bonsai.o $(OBJS) obj/main.o bin out obj/pfdodirfit.o
	@echo "[NTag] Building NTag..."
	@LD_RUN_PATH=$(TMVALIB):$(SKOFL_LIBDIR):$(ROOTSYS)/lib:$(LIBDIR):$(A_LIBDIR) $(CXX) $(CXXFLAGS) -o $@ $(OBJS) obj/bonsai.o obj/main.o obj/NTagDict.o $(LDLIBS) obj/pfdodirfit.o
	@chmod +x path/bash path/csh
	@if [ ! -d "weights/new" ]; then mkdir weights/new; fi
	@echo "[NTag] Done!"

lib/libNTag.so: obj/bonsai.o $(OBJS) lib obj/pfdodirfit.o
	@echo "[NTag] Building shared library..."
	@$(CXX) $(CXXFLAGS) -o $@ $(OBJS) -shared

obj/bonsai.o: src/bonsai.F obj
	@echo "[NTag] Building BONSAI..."
	@$(FC) $(FCFLAGS) -c $< -o $@

obj/main.o: main.cc obj
	@echo "[NTag] Building main..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o: src/%.cc obj
	@echo "[NTag] Building $*..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

obj/pfdodirfit.o: src/pfdodirfit.F obj
	@$(FC) $(FCFLAGS) -c $< -o $@

bin obj out lib:
	@mkdir $@

.PHONY: clean

clean:
	@$(RM) -rf *.o *~ *.log obj bin src/NTagDict.*
