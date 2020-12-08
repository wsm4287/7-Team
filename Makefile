PREFIX	=
CXX		= $(PREFIX)g++

CFLAGS	= -g -O2 -Wall -std=c99
LIBS	= 
RM		= rm

TARGET	= ftl_sim
CXXSRCS	= ftl_sim.cpp nand.cpp ftl.cpp
HEADERS	= nand.h ftl.h
OBJS	= $(CXXSRCS:.cpp=.o)

all: $(OBJS)
	$(CXX) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS) 

.cpp.o: $(HEADERS)
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -f $(TARGET) $(OBJS) BANK* *~
