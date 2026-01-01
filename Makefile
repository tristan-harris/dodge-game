CC := gcc
CFLAGS := -std=c99 -Wall -Wextra -MMD -MP -Iinclude -isystem raylib/include

OBJDIR_BASE := obj
BINDIR_BASE := bin

# default build type of debug
BUILD ?= debug

ifeq ($(BUILD),debug)
	CFLAGS += -g -O0 -DDEBUG
	OBJDIR := $(OBJDIR_BASE)/debug
	BINDIR := $(BINDIR_BASE)/debug
endif

ifeq ($(BUILD),release)
	CFLAGS += -O2 -DNDEBUG
	OBJDIR := $(OBJDIR_BASE)/release
	BINDIR := $(BINDIR_BASE)/release
endif

SRCDIR := src
SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

LDOBJS := $(wildcard raylib/lib/*)

LDLIBS := -lm -lpthread -ldl -lX11

# equivalent dependency files (generated with -MMD -MP)
# used to track header file changes
DEPS := $(OBJECTS:.o=.d)

TARGET := $(BINDIR)/game

.PHONY: all clean

all: $(TARGET)

# linking
$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) $(LDOBJS) $(LDLIBS) -o $@

# compilation
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR) $(OBJDIR):
	mkdir -p $@

-include $(DEPS)

clean:
	rm -rf $(OBJDIR_BASE) $(BINDIR_BASE)
