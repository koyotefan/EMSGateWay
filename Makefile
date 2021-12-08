include $(PCF_DEV_HOME)/makehead

EXE       =EmsGWMain							
SRCS      =$(wildcard *.cpp)
HEADERS   =$(filter-out ,$(wildcard *.hpp))
OBJS      =$(SRCS:.cpp=.o)
OPTIMIZER += -Wno-write-strings
PCF_LIB   += -lPDB

#CFLAGS += -D__T_DEBUG

all :: $(EXE) 

$(EXE) : $(OBJS) $(HEADERS)
	$(CXX) -o $(EXE) $(OBJS) $(LDFLAGS) 


install:
	@if [ ! -d "$(PCF_PKG_HOME)" ]; then mkdir -p "$(PCF_PKG_HOME)";  fi
	$(CP) $(EXE) $(PCF_PKG_HOME)/BIN/


clean:
	$(RM) $(OBJS) $(EXE) core.* *.d

DEPS := $(OBJS:.o=.d)

-include $(DEPS)
