include $(ATMPD_ROOT)/config.gmk

NTAG_GD_ROOT = $(shell pwd)

LOCAL_INC = -I$(NTAG_GD_ROOT)/include #-I$(ATMPD_ROOT)/src/analysis/neutron/ntag
LOCAL_LIBS =

CXXFLAGS += $(LOCAL_INC)

SRCS = $(wildcard src/*.cc)
OBJS = $(patsubst src/%.cc, obj/%.o, $(SRCS))

bin/main: $(OBJS) obj/main.o bin
	@echo "[NTagGd] Building the main program..."
	@LD_RUN_PATH=$(SKOFL_LIBDIR):$(A_LIBDIR) $(CXX) $(CXXFLAGS) -o $@ $(OBJS) obj/main.o $(LDLIBS)
	@echo "[NTagGd] Done!"

obj/main.o: main.cc
	@echo "[NTagGd] Building $*..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o: src/%.cc obj
	@echo "[NTagGd] Building $*..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

bin obj:
	@mkdir $@

clean:
	@$(RM) -r *.o *~ *.log obj bin