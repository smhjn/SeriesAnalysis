# neste template de Makefile so muda a lista
# dos SOURCES e o nome do EXECUTABLE.

CC=g++
CFLAGS=-c -Wall -Weffc++ -pedantic -std=c++0x  
LDFLAGS= -lgtest -lgsl -lgslcblas -lm -lpthread -lproxime
SOURCES=   ../src/rp.cpp ../src/rqa.cpp ../src/utils.cpp  ../src/chaos.cpp ../src/attractor.cpp main.cpp ../src/time_series.cpp ../src/statistics.cpp  ../src/patterns/patterns.cpp ../src/statistics/src/statistics.cpp ../src/statistics/src/histogram.cpp
 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=tests


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC)  $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

 
