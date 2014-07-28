CFLAGS = -O2 -Wall $(INCLUDE_DIRS:%=-I%) $(EXTRA_CFLAGS) $(CONFIG) $(FEATURES)
LDFLAGS = $(LIB_DIRS:%=-L%) $(EXTRA_LDFLAGS)

DEMOS = AeDemo1 AeDemo2 AeDemo3 AeDemo4 AeDemo5 AeDemo6
COMMON_OBJS = AeDemoCommon.o
OBJS = $(COMMON_OBJS) $(DEMOS:%=%.o)
DEPS = $(OBJS:%.o=%.d)

.PHONY: all clean

.PRECIOUS: %.o

all: $(DEMOS)

clean:
	rm -f $(OBJS) $(DEPS) $(DEMOS)

AeDemo%: $(COMMON_OBJS) AeDemo%.o $(AE_LIB)
	$(CC) $(LDFLAGS) -o $@ $(COMMON_OBJS) AeDemo$*.o -lAgentEmbedded $(LIBS)

.c.o:
	$(CC) -MMD -MP $(CFLAGS) -c -o $@ $<

-include $(DEPS)
