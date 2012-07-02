TARGET := mancala
C_SOURCES :=
CPP_SOURCES := mancala.cpp
SOURCES := $(C_SOURCES) $(CPP_SOURCES)
OBJECTS := ${C_SOURCES:.c=.o} ${CPP_SOURCES:.cpp=.o}

INCLUDE_DIRS :=
LIBRARY_DIRS :=
LIBRARIES :=

CPPFLAGS += $(foreach includedir,$(INCLUDE_DIRS),-I$(includedir))
CPPFLAGS += -O3 -Wall -std=c++0x
LDFLAGS += $(foreach librarydir,$(LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(LIBRARIES),-l$(library))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LINK.cc) -o $@ $<

.PHONY: clean cleanest

clean:
	@-$(RM) $(OBJECTS)

cleanest:
	@-$(RM) $(OBJECTS) $(TARGET)
