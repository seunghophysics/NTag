##### VARIABLES #####

SKOFL_ROOT = /home/skofl/sklib_gcc4.8.5/skofl-trunk/
ATMPD_ROOT = /home/skofl/sklib_gcc4.8.5/atmpd-trunk/

NTAG_GD_ROOT = $(shell pwd)

OLD_NTAG_GD_ROOT = $(ATMPD_ROOT)/src/analysis/neutron/ntag_gd
TMVASYS      = /disk02/usr6/han/Apps/TMVA

SKOFLINCLUDE = -I $(SKOFL_ROOT)/include -I src/SKLibrary
SKOFLLIB = -L $(SKOFL_ROOT)/lib -lgeom -lskrd -lastro -lzbs -lgeom -lsklib -llibrary -liolib -lrfa \
								-lskroot -lDataDefinition -ltqrealroot -lloweroot -latmpdroot -lmcinfo -lsofttrgroot -lidod_xtlk_root
ATMPDINCLUDE = -I $(ATMPD_ROOT)/include -I $(OLD_NTAG_GD_ROOT) -I $(ATMPD_ROOT)/src/recon/fitqun
ATMPDLIB = -L $(ATMPD_ROOT)/lib -lapdrlib -laplib -lringlib -ltp -ltf -lringlib \
	       -laplib -lmsfit -lmslib -lseplib -lmsfit -lprtlib -lmuelib \
	       -lffit -lodlib -lstmu -laplowe -laplib -lfiTQun -ltf -lmslib -llelib -lntuple_t2k
ROOTINCLUDE = -I $(ROOTSYS)/include
ROOTLIB = $(shell root-config --libs) -lMinuit -lXMLIO -lMLP -lTreePlayer
CERNLIB = `cernlib graflib grafX11 packlib mathlib kernlib lapack3 blas` -L $(CERN_ROOT)/lib -ljetset74
TMVAINCLUDE = -I $(TMVASYS)/include
TMVALIB = -L $(TMVASYS)/lib -lTMVA.1

all: NTag lib/libToolFramework.so lib/libNTagTools.so
	@echo "[NTag] Done!"

## TOOL TEST ##

TOOLCXX = g++
TOOLCXXFLAGS = -std=c++11 -fPIC -g
FC = gfortran
FCFLAGS += -w -fPIC -lstdc++

TOOLFRAMEWORKSRCS = $(wildcard src/ToolFramework/*/*.cc) $(wildcard src/ToolFramework/*/*/*.cc) $(wildcard src/ToolFramework/*/*/*/*.cc)
TOOLFRAMEWORKOBJS = $(patsubst src/ToolFramework/%.cc, src/ToolFramework/%.o, $(TOOLFRAMEWORKSRCS))
UTILOBJS = $(patsubst src/Utilities/%.cc, src/Utilities/%.o, $(wildcard src/Utilities/*/*.cc))

TOOLFRAMEWORKINCLUDE := $(addprefix -I , $(sort $(dir $(shell find src/ToolFramework -name '*.hh')))) 
UTILINCLUDE := $(addprefix -I , $(sort $(dir $(wildcard src/Utilities/*/*.hh))))

# ToolFramework
$(TOOLFRAMEWORKOBJS): src/ToolFramework/%.o: src/ToolFramework/%.cc src/ToolFramework/%.hh
	@echo "[NTag] Building ToolFramework: $(word $(words $(subst /, , $*)), $(subst /, , $*))..."
	@$(TOOLCXX) $(TOOLCXXFLAGS) -o $@ -c $< $(UTILINCLUDE) $(TOOLFRAMEWORKINCLUDE) $(ROOTINCLUDE) $(SKOFLINCLUDE)

# ToolFramework shared library
lib/libToolFramework.so: $(UTILOBJS) $(TOOLFRAMEWORKOBJS)
	@echo "[NTag] Building ToolFramework shared library..."
	@cp `find src/Utilities src/ToolFramework -name '*.hh'` include
	@$(RUNPATHOPTION) $(TOOLCXX) $(TOOLCXXFLAGS) -shared -o $@ $^ $(ROOTLIB)

# Utilities
$(UTILOBJS): src/Utilities/%.o: src/Utilities/%.cc src/Utilities/%.hh
	@echo "[NTag] Building Utility: $(word 1, $(subst /, , $*))..."
	@$(TOOLCXX) $(NTAGCXXFLAGS) -o $@ -c $< $(TOOLFRAMEWORKINCLUDE) $(ROOTINCLUDE) $(SKOFLINCLUDE)
	
NTAGTOOLINCLUDE :=  $(addprefix -I , $(sort $(dir $(wildcard src/Tools/*/*.hh))))
NTAGCXXFLAGS = -std=c++11 -fPIC -lgfortran -g
NTAGTOOLOBJS = $(patsubst src/Tools/%.cc, src/Tools/%.o, $(wildcard src/Tools/*/*.cc))
SKLIBOBJS = $(patsubst src/SKLibrary/%.F, src/SKLibrary/%.o, $(wildcard src/SKLibrary/*.F))

# NTagTools
$(NTAGTOOLOBJS): src/Tools/%.o: src/Tools/%.cc src/Tools/%.hh
	@echo "[NTag] Building Tool: $(word 1, $(subst /, , $*))..."
	@$(TOOLCXX) $(NTAGCXXFLAGS) -o $@ -c $< $(UTILINCLUDE) $(NTAGTOOLINCLUDE) $(TOOLFRAMEWORKINCLUDE) $(TMVAINCLUDE) $(ROOTINCLUDE) $(SKOFLINCLUDE) $(ATMPDINCLUDE)

src/SKLibrary/%.o: src/SKLibrary/%.F
	@echo "[NTag] Building SKLibrary: $*..."
	@$(FC) $(FCFLAGS) -c $< -o $@ -I $(SKOFL_ROOT)/inc -I $(ATMPD_ROOT)/inc -I $(SKOFL_ROOT)/lowe

# NTagTools shared library
lib/libNTagTools.so: $(UTILOBJS) $(NTAGTOOLOBJS) $(TOOLFRAMEWORKOBJS) $(SKLIBOBJS)
	@echo "[NTag] Building NTagTools shared library..."
	@cp `find src/Utilities src/Tools -name '*.hh'` include
	@ $(RUNPATHOPTION) $(TOOLCXX) $(TOOLCXXFLAGS) -shared -o $@ $^ $(TMVALIB) $(ROOTLIB) $(ATMPDLIB)

src/%.o: src/%.cc
	@echo "[NTag] Building $*.o..."
	@$(TOOLCXX) $(NTAGCXXFLAGS) -o $@ -c $< $(UTILINCLUDE) $(NTAGTOOLINCLUDE) $(TOOLFRAMEWORKINCLUDE) $(ROOTINCLUDE) $(SKOFLINCLUDE)

src/git_info.c: .git/HEAD .git/index
	@echo "const char* gitcommit = \"$(shell git rev-parse HEAD)\";" > $@
	@echo "const char* gitcommitdate = \"`git log -1 --format="%cd"`\";" >> $@
	
src/git_info.o: src/git_info.c
	@echo "[NTag] Building git_info..."
	@gcc -c -o $@ $^

# executable
NTag: $(UTILOBJS) $(TOOLFRAMEWORKOBJS) $(NTAGTOOLOBJS) $(SKLIBOBJS) src/git_info.o src/NTag.o
	@echo "[NTag] Building $@..."
	@LD_RUN_PATH=$(TMVASYS)/lib $(TOOLCXX) $(NTAGCXXFLAGS) -o $@ $^ $(TMVALIB) $(ATMPDLIB) $(SKOFLLIB) $(ROOTLIB) $(CERNLIB)

clean:
	@rm -rf NTag src/*.o src/git_info.c src/SKLibrary/*.o src/Utilities/*/*.o src/Tools/*/*.o lib/* include/* src/ToolFramework/*/*.o src/ToolFramework/DataModel/*.o src/ToolFramework/DataModel/*/*.o src/ToolFramework/DataModel/*/*/*.o 