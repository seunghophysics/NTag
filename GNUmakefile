##### VARIABLES #####

CXX = g++
CXXFLAGS = -std=c++11 -fPIC -g -O3

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

all: dirs inc lib/libNTagDataModel.so lib/libToolFramework.so lib/libNTagTools.so NTag 
	@echo "[NTag] Done!"
	
dirs:
	@mkdir -p lib include

inc:
	@cp `find src/ -name '*.hh'` include

include/%.hh:
	@:




NTAGDATAMODELSRCS = $(filter-out %Dict.cc, $(wildcard src/DataModel/*.cc) $(wildcard src/DataModel/*/*.cc) $(wildcard src/DataModel/*/*/*.cc))
NTAGDATAMODELOBJS = $(patsubst src/DataModel/%.cc, src/DataModel/%.o, $(NTAGDATAMODELSRCS))
NTAGDATAMODELINCLUDE := $(addprefix -I , $(sort $(dir $(shell find src/DataModel -name '*.hh'))))
NTAGDATAMODELLIB = -L$(NTAG_GD_ROOT)/lib -lNTagDataModel

UTILOBJS = $(patsubst src/Utilities/%.cc, src/Utilities/%.o, $(wildcard src/Utilities/*/*.cc))
UTILINCLUDE := $(addprefix -I , $(sort $(dir $(wildcard src/Utilities/*/*.hh))))

NTAGDATAMODELDICT = src/DataModel/NTagDataModelDict.cc

# Utilities
$(UTILOBJS): src/Utilities/%.o: src/Utilities/%.cc src/Utilities/%.hh
	@echo "[NTag] Building Utility: $(word 1, $(subst /, , $*))..."
	@$(CXX) $(NTAGCXXFLAGS) -o $@ -c $< $(NTAGDATAMODELINCLUDE) $(ROOTINCLUDE) $(SKOFLINCLUDE)

src/DataModel/NTagDataModelDict.hh: $(NTAGDATAMODELDICT)

src/DataModel/NTagDataModelDict.o: $(NTAGDATAMODELDICT)
	@echo "[NTag] Building DataModel: NTagDataModelDict..."
	@$(CXX) $(CXXFLAGS) $(ROOTINCLUDE) -I$(NTAG_GD_ROOT)/include $(SKOFLINCLUDE) -o $@ -c $<

# NTagDataModel
$(filter-out NTagDataModelDict, $(NTAGDATAMODELOBJS)): src/DataModel/%.o: src/DataModel/%.cc src/DataModel/%.hh
	@echo "[NTag] Building DataModel: $(word $(words $(subst /, , $*)), $(subst /, , $*))..."
	@$(CXX) $(CXXFLAGS) -o $@ -c $< $(UTILINCLUDE) $(NTAGDATAMODELINCLUDE) $(ROOTINCLUDE) $(SKOFLINCLUDE)
	
$(NTAGDATAMODELDICT): $(addprefix include/, $(filter-out NTagLinkDef.hh, $(notdir $(shell find src/DataModel -name '*.hh'))))
	@cd include && rootcint -f ../$@ -c -I$(SKOFL_ROOT)/include $(notdir $^) NTagLinkDef.hh && cd ..

# ToolFramework shared library
lib/libNTagDataModel.so: $(UTILOBJS) $(NTAGDATAMODELOBJS) src/DataModel/NTagDataModelDict.o
	@echo "[NTag] Building DataModel shared library..."
	@$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(ROOTLIB) $(SKOFLLIB)








##### TOOLFRAMEWORK #####

TOOLFRAMEWORKSRCS = $(wildcard src/ToolFramework/*/*.cc) 
TOOLFRAMEWORKOBJS = $(patsubst src/ToolFramework/%.cc, src/ToolFramework/%.o, $(TOOLFRAMEWORKSRCS))
TOOLFRAMEWORKINCLUDE := $(addprefix -I , $(sort $(dir $(wildcard src/ToolFramework/*/*.hh))))

# ToolFramework
$(TOOLFRAMEWORKOBJS): src/ToolFramework/%.o: src/ToolFramework/%.cc src/ToolFramework/%.hh
	@echo "[NTag] Building ToolFramework: $(word $(words $(subst /, , $*)), $(subst /, , $*))..."
	@$(CXX) $(CXXFLAGS) -o $@ -c $< $(UTILINCLUDE) $(NTAGDATAMODELINCLUDE) $(TOOLFRAMEWORKINCLUDE) $(ROOTINCLUDE) $(SKOFLINCLUDE)

# ToolFramework shared library
lib/libToolFramework.so: $(TOOLFRAMEWORKOBJS)
	@echo "[NTag] Building ToolFramework shared library..."
	@$(CXX) $(CXXFLAGS) -shared -o $@ $^





##### NTAGTOOLS #####
	
NTAGTOOLINCLUDE :=  $(addprefix -I , $(sort $(dir $(wildcard src/Tools/*/*.hh))))
NTAGCXXFLAGS = $(CXXFLAGS) -lgfortran
NTAGTOOLOBJS = $(patsubst src/Tools/%.cc, src/Tools/%.o, $(wildcard src/Tools/*/*.cc))
SKLIBOBJS = $(patsubst src/SKLibrary/%.F, src/SKLibrary/%.o, $(wildcard src/SKLibrary/*.F))
FC = gfortran
FCFLAGS += -w -fPIC -lstdc++

# NTagTools
$(NTAGTOOLOBJS): src/Tools/%.o: src/Tools/%.cc src/Tools/%.hh
	@echo "[NTag] Building Tool: $(word 1, $(subst /, , $*))..."
	@$(CXX) $(NTAGCXXFLAGS) -o $@ -c $< $(UTILINCLUDE) $(NTAGTOOLINCLUDE) $(TOOLFRAMEWORKINCLUDE) $(NTAGDATAMODELINCLUDE) $(TMVAINCLUDE) $(ROOTINCLUDE) $(SKOFLINCLUDE) $(ATMPDINCLUDE)

src/SKLibrary/%.o: src/SKLibrary/%.F
	@echo "[NTag] Building SKLibrary: $*..."
	@$(FC) $(FCFLAGS) -c $< -o $@ -I $(SKOFL_ROOT)/inc -I $(ATMPD_ROOT)/inc -I $(SKOFL_ROOT)/lowe

# NTagTools shared library
lib/libNTagTools.so: $(UTILOBJS) $(NTAGTOOLOBJS) $(NTAGDATAMODELOBJS) $(TOOLFRAMEWORKOBJS) $(SKLIBOBJS)
	@echo "[NTag] Building NTagTools shared library..."
	@$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(TMVALIB) $(ROOTLIB) $(ATMPDLIB)

src/%.o: src/%.cc
	@echo "[NTag] Building $*.o..."
	@$(CXX) $(NTAGCXXFLAGS) -o $@ -c $< $(UTILINCLUDE) $(NTAGTOOLINCLUDE) $(NTAGDATAMODELINCLUDE) $(TOOLFRAMEWORKINCLUDE) $(ROOTINCLUDE) $(SKOFLINCLUDE)

src/git_info.c: .git/HEAD .git/index
	@echo "const char* gitcommit = \"$(shell git rev-parse HEAD)\";" > $@
	@echo "const char* gitcommitdate = \"`git log -1 --format="%cd"`\";" >> $@
	
src/git_info.o: src/git_info.c
	@echo "[NTag] Building git_info..."
	@gcc -c -o $@ $^

# executable
NTag: $(NTAGDATAMODELOBJS) $(UTILOBJS) $(TOOLFRAMEWORKOBJS) src/DataModel/NTagDataModelDict.o $(SKLIBOBJS) $(NTAGTOOLOBJS) src/git_info.o src/NTag.o
	@echo "[NTag] Building $@..."
	@LD_RUN_PATH=$(TMVASYS)/lib $(CXX) $(NTAGCXXFLAGS) -o $@ $^ $(TMVALIB) $(ATMPDLIB) $(SKOFLLIB) $(ROOTLIB) $(CERNLIB)

clean:
	@rm -rf NTag src/*.o src/git_info.c src/DataModel/NTagDataModelDict.* src/SKLibrary/*.o src/Utilities/*/*.o src/Tools/*/*.o lib include src/ToolFramework/*/*.o src/DataModel/*.o src/DataModel/*/*.o src/DataModel/*/*/*.o 