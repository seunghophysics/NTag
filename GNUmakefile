#
# In case of building locally, set SKOFL_ROOT variable 
#      setenv SKOFL_ROOT   ... 
#  or directly set in this makefile 
# SKOFL_ROOT = /skofl
#

ifndef SKOFL_ROOT
  SKOFL_ROOT = ../..
endif

include $(SKOFL_ROOT)/config.gmk


LOCAL_INC	= 

LOCAL_LIBS	= 

#
#  Objects
#

OBJS   =   main 

all: $(OBJS) 

clean: 
	$(RM) *.o *~ core fort.* *.log *.hbk $(OBJS) output.* 

