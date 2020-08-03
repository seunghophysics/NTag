include $(ATMPD_ROOT)/config.gmk

NTAG_GD_ROOT = $(shell pwd)

ROOT_INC     = $(shell root-config --cflags)
ROOT_LIBS    = $(shell root-config --libs)

LOCAL_INC = -I$(NTAG_GD_ROOT)/include -I$(ATMPD_ROOT)/src/recon/fitqun \
			-I$(ATMPD_ROOT)/src/analysis/neutron/ntag_gd

FORTRANINCLUDES += -I$(SKOFL_FORTRAN_INCDIR)/lowe

LOCAL_LIBS = $(APLIB) \
			-lsklowe_7.0 -lsollib_4.0 -lwtlib_5.1 -lbonsai_3.3 \
			-L/home/iida/relic/lib -lrelic \
			-lTMVA -lMinuit -lXMLIO -lMLP

APLIB =  -lapdrlib -laplib -lringlib  -ltp -ltf -lringlib \
	 -laplib -lmsfit -lmslib -lseplib -lmsfit -lprtlib -lmuelib \
	 -lffit -lodlib -lstmu -laplowe -laplib -lfiTQun -ltf -lmslib -llelib -lntuple_t2k

CXXFLAGS += -std=c++11 -L/usr/lib/gcc/x86_64-redhat-linux/4.8.2/ -lgfortran

SRCS = $(wildcard src/*.cc)
OBJS = $(patsubst src/%.cc, obj/%.o, $(SRCS))

bin/main: obj/bonsai.o $(OBJS) obj/main.o bin
	@echo "[NTagGd] Building the main program..."
	@LD_RUN_PATH=$(SKOFL_LIBDIR):$(A_LIBDIR) $(CXX) $(CXXFLAGS) -o $@ $(OBJS) obj/bonsai.o obj/main.o $(LDLIBS)
	@echo "[NTagGd] Done!"

obj/bonsai.o: src/bonsai.F obj
	@echo "[NTagGd] Building BONSAI..."
	@$(FC) $(FCFLAGS) -c $< -o $@

obj/main.o: main.cc obj
	@echo "[NTagGd] Building main..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o: src/%.cc obj
	@echo "[NTagGd] Building $*..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

bin obj:
	@mkdir $@

.PHONY: clean
clean:
	@$(RM) -r *.o *~ *.log obj bin