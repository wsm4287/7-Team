PREFIX	=
CC		= $(PREFIX)g++

CFLAGS	= -g -O2 -Wall -std=c99
LIBS	= 
RM		= rm

TARGET	= nand_sim
CSRCS	= nand_sim.cpp nand.cpp
HEADERS	= nand.h
OBJS	= $(CSRCS:.cpp=.o)

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS) 

.cpp.o: $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	$(RM) -f $(TARGET) $(OBJS) bank* *~
