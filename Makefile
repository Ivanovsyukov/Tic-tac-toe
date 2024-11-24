CC=g++
CFLAGS=-c -pedantic-errors -gdwarf-4 -std=c++20
LDFLAGS=-lws2_32
SOURCES=main.cpp game.cpp Logger.cpp Global_logger.cpp TcpClient.cpp TcpServer.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=TTT_game

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -rf *.o $(EXECUTABLE)
