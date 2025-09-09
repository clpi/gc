CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g

# Try to discover ncurses via pkg-config; fall back to -lncurses
PKG_CONFIG ?= pkg-config
NCURSES_CFLAGS ?= $(shell $(PKG_CONFIG) --cflags ncurses 2>/dev/null)
NCURSES_LIBS ?= $(shell $(PKG_CONFIG) --libs ncurses 2>/dev/null)
ifeq ($(strip $(NCURSES_LIBS)),)
  NCURSES_CFLAGS := $(shell $(PKG_CONFIG) --cflags ncursesw 2>/dev/null)
  NCURSES_LIBS := $(shell $(PKG_CONFIG) --libs ncursesw 2>/dev/null)
endif
ifeq ($(strip $(NCURSES_LIBS)),)
  NCURSES_LIBS := -lncurses
endif

CFLAGS += $(NCURSES_CFLAGS)
LDFLAGS = $(NCURSES_LIBS) -lm

SRCDIR = src
OBJDIR = obj
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET = dungeon3d

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET)

install:
	sudo apt-get update && sudo apt-get install -y libncurses5-dev

run: $(TARGET)
	./$(TARGET)
