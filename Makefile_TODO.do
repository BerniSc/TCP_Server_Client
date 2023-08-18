INCLUDEDIR = include/
COMPILER = g++
CXXFLAGS = -I$(INCLUDEDIR)

OUTPUTDIR = out

LIBDIR = 
LIBS =

_DEPENDENCIES_SERVER = server.hpp
DEPENDENCIES_SERVER = $(patsubst %,$(INCLUDEDIR)/%,$(_DEPENDENCIES))

_DEPENDENCIES_CLIENT = client.hpp
DEPENDENCIES_CLIENT= $(patsubst %,$(INCLUDEDIR)/%,$(_DEPENDENCIES))

_OBJECTS = server.o client.o 
OBJECTS = $(patsubst %,$(OUTPUTDIR)/%,$(_OBJECTS))


$(OUTPUTDIR)/%.o: %.cpp $(DEPENDENCIES)
	$(COMPILER) -c -o $@ $< $(CXXFLAGS)

server: $(OBJECTS)
	$(COMPILER) -o $@ $^ $(CXXFLAGS) $(LIBS)

client: $(CLIENT_OBJECTS)
	$(COMPILER) -o $@ $^ $(CXXFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(OUTPUTDIR)/*.o *~ core $(INCLUDEDIR)/*~ 