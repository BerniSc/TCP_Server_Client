COMPILER = g++
OUTPUT_FILE = output_client
SRC = client.cpp
OPTIONS = -O3 -std=c++17 -Wall -L/usr/include/boost -pthread

LIBS =

$(OUTPUT_FILE) : $(SRC)
		$(COMPILER) $(SRC) $(OPTIONS) -o $(OUTPUT_FILE) $(LIBS)
