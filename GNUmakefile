include $(ATMPD_ROOT)/config.gmk

NTAG_GD_ROOT = $(shell pwd)

LOCAL_INC = -I$(NTAG_GD_ROOT)/include
LOCAL_LIBS =

CXXFLAGS += $(LOCAL_INC)

SRCS = $(wildcard src/*.cc)
OBJS = $(patsubst src/%.cc, obj/%.o, $(SRCS))

main: $(OBJS) obj/main.o
	@echo "[NTagGd] Building the main program..."
	@LD_RUN_PATH=$(SKOFL_LIBDIR):$(A_LIBDIR) $(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)
	@echo "[NTagGd] Done!"

obj/%.o: src/%.cc
	@echo "[NTagGd] Building $*..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

obj/main.o: main.cc
	@echo "[NTagGd] Building $*..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@$(RM) *.o *~ *.log $(OBJS) main