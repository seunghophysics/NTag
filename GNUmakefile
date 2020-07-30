include $(ATMPD_ROOT)/config.gmk

NTAG_GD_ROOT = $(shell pwd)

LOCAL_INC = -I$(NTAG_GD_ROOT)/include
LOCAL_LIBS =

#  Objects

OBJS = src/%.o

main: $(OBJS)



clean:
	$(RM) *.o *~ *.log $(OBJS)

