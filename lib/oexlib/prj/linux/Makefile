
# Compile flags
LDFLAGS := -g 
LDLIBS := -lstdc++
CFLAGS := -g -MMD -I.

# Compiler
CC := gcc
LD := gcc

# Vars
TARGETS :=

# Target info
SOURCES := $(wildcard ../../src/*.cpp)
OBJS := $(patsubst %.cpp,%.o,$(SOURCES))
TARGETS += ../../bin/linux/obj

all:	setup $(TARGETS)

rebuild: clean all

setup:
	-mkdir ../../bin/linux
	-@mkdir ../../bin/linux/obj

clean:
	rm -rf ../../bin/linux

../../bin/linux/obj: $(OBJS)

../../bin/linux/obj/%.o: $(SOURCES)
	$(CC) -c $(CFLAGS) $< -o $@


