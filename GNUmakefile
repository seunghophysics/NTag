include $(ATMPD_ROOT)/config.gmk

NTAG_GD_ROOT = $(shell pwd)

ROOT_INC     = $(shell root-config --cflags)
ROOT_LIBS    = $(shell root-config --libs)

LOCAL_INC = -I$(NTAG_GD_ROOT)/include -I$(ATMPD_ROOT)/src/recon/fitqun -I$(ATMPD_ROOT)/src/analysis/neutron/ntag_gd
LOCAL_LIBS = $(APLIB) -lTMVA -lMinuit -lXMLIO -lMLP

APLIB =  -lapdrlib -laplib -lringlib  -ltp -ltf -lringlib \
	 -laplib -lmsfit -lmslib -lseplib -lmsfit -lprtlib -lmuelib \
	 -lffit -lodlib -lstmu -laplowe -laplib -lfiTQun -ltf -lmslib -llelib -lntuple_t2k

SRCS = $(wildcard src/*.cc)
OBJS = $(patsubst src/%.cc, obj/%.o, $(SRCS))

bin/main: $(OBJS) obj/main.o bin
	@echo "[NTagGd] Building the main program..."
	@LD_RUN_PATH=$(SKOFL_LIBDIR):$(A_LIBDIR) $(CXX) $(CXXFLAGS) -o $@ $(OBJS) obj/main.o $(LDLIBS)
	@echo "[NTagGd] Done!"

obj/main.o: main.cc obj
	@echo "[NTagGd] Building $*..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o: src/%.cc obj
	@echo "[NTagGd] Building $*..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

bin obj:
	@mkdir $@

.PHONY: clean
clean:
	@$(RM) -r *.o *~ *.log obj bin