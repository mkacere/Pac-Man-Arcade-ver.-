# =============================
# Include Local Configuration
# =============================
-include config.mk

# =============================
# Project Metadata
# =============================
TARGET       = PACMAN
BUILD_DIR    = build

# =============================
# Source Files
# =============================
SRC_FILES = \
	main.cpp \
	Game.cpp \
	StateManager.cpp \
	MainMenuState.cpp \
	AnimatedSprite.cpp \
	PlayingState.cpp \
	Player.cpp \
	Ghost.cpp \
	Blinky.cpp \
	Pinky.cpp \
	Inky.cpp \
	Clyde.cpp \
	EndGameState.cpp

OBJECTS = $(addprefix $(BUILD_DIR)/, $(SRC_FILES:.cpp=.o))

# =============================
# SFML Configuration
# =============================
SFML_DIR         ?= /usr/local
SFML_INCLUDE_DIR ?= $(SFML_DIR)/include
SFML_LIB_DIR     ?= $(SFML_DIR)/lib
SFML_LIBS        = -lsfml-graphics -lsfml-window -lsfml-system

# =============================
# Compiler Configuration
# =============================
CXX              ?= g++
CPPVERSION       = -std=c++17
CXXFLAGS_DEBUG   = -g
CXXFLAGS_WARN    = -Wall -Wextra -Wconversion -Wdouble-promotion -Wunreachable-code -Wshadow -Wpedantic

# =============================
# Platform-Specific Settings
# =============================
ifeq ($(shell uname -s), Windows)
	TARGET_FULL = $(BUILD_DIR)/$(TARGET).exe
	DEL         = del
else
	TARGET_FULL = $(BUILD_DIR)/$(TARGET)
	DEL         = rm -f
endif

# =============================
# Default Target
# =============================
all: $(TARGET_FULL)

# =============================
# Build Rules
# =============================

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link the final executable
$(TARGET_FULL): $(OBJECTS) | $(BUILD_DIR)
	$(CXX) -o $@ $(OBJECTS) -L$(SFML_LIB_DIR) $(SFML_LIBS)

# Compile each source file
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CPPVERSION) $(CXXFLAGS_DEBUG) $(CXXFLAGS_WARN) -I$(SFML_INCLUDE_DIR) -c $< -o $@

# =============================
# Utility Targets
# =============================

# Build and run the project
.PHONY: run
run: $(TARGET_FULL)
	./$(TARGET_FULL)

# Clean build artifacts
.PHONY: clean
clean:
	$(DEL) -r $(BUILD_DIR)
