WARN = -Wall -Wstrict-prototypes

CC = $(CROSS_COMPILE)gcc -msoft-float
AR = $(CROSS_COMPILE)ar

TESTLOGOBJ = list.o testlog.o
LDFLAGS = -L.
LIBS = -ltestlog

all : libtestlog.a test

test : testtestlog testtestlog2 testtestlog3 testtestlog4

clean :
	rm -f libtestlog.a testtestlog testtestlog2 \
	      testtestlog3 testtestlog4  *.o

libtestlog.a : $(TESTLOGOBJ)
	$(AR) r libtestlog.a $(TESTLOGOBJ)

list.o : list.h list.c
	${CC} ${CFLAGS} ${WARN} -c list.c

testlog.o : testlog.h testlog.c
	${CC} ${CFLAGS} ${WARN} -c testlog.c
	
testtestlog : testtestlog.c
	${CC} ${CFLAGS} testtestlog.c $(LDFLAGS) $(LIBS) -o testtestlog
	
testtestlog2 : testtestlog2.c
	${CC} ${CFLAGS} testtestlog2.c $(LDFLAGS) $(LIBS) -o testtestlog2
	
testtestlog3 : testtestlog3.c
	${CC} ${CFLAGS} testtestlog3.c $(LDFLAGS) $(LIBS) -o testtestlog3
	
testtestlog4 : testtestlog4.c
	${CC} ${CFLAGS} testtestlog4.c $(LDFLAGS) $(LIBS) -o testtestlog4
