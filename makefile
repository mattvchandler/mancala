TARGET := mancala
C_SOURCES :=
CXX_SOURCES := appstart.cpp mancala.cpp
SOURCES := $(C_SOURCES) $(CXX_SOURCES)
C_OBJECTS := ${C_SOURCES:.c=.o}
CXX_OBJECTS := ${CXX_SOURCES:.cpp=.o}
OBJECTS = $(C_OBJECTS) $(CXX_OBJECTS)

INCLUDE_DIRS :=
LIBRARY_DIRS :=
LIBRARIES :=

CPPFLAGS += $(foreach includedir,$(INCLUDE_DIRS),-I$(includedir))
CFLAGS   +=
CXXFLAGS += -O3 -Wall -std=c++0x
LDFLAGS  += $(foreach librarydir,$(LIBRARY_DIRS),-L$(librarydir))
LDFLAGS  += $(foreach library,$(LIBRARIES),-l$(library))

CC := gcc
CXX := g++

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

-include ${OBJECTS:.o=.d}

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM $< > $*.d
	@cp -f $*.d $*.d.tmp
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	    sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@$(RM) $*.d.tmp

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<
	@$(CC) $(CPPFLAGS) $(CFLAGS) -MM $< > $*.d
	@cp -f $*.d $*.d.tmp
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	    sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@$(RM) $*.d.tmp

.PHONY: clean cleanest

clean:
	@-$(RM) $(OBJECTS) ${OBJECTS:.o=.d}

cleanest:
	@-$(RM) $(OBJECTS) $(TARGET) ${OBJECTS:.o=.d}
