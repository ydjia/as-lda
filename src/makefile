CC=g++
CFLAGS=-c -Wall -g -O3
#LDFLAGS=-L /usr/local/lib 
SOURCES=text_tools.cpp ml_data.cpp lda.cpp main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=../bin/as_lda

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

%.o:%.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -fr *.o *~ $(EXECUTABLE)
