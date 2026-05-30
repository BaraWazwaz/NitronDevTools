# --- Configuration ---
CXX      := g++
CXXFLAGS := -Wall -Wextra -std=c++20 -I include -MMD -MP -finput-charset=UTF-8 -fexec-charset=UTF-8 -D_GNU_SOURCE -O3 -pthread

# --- Project Structure ---
BIN_DIR  := bin
OBJ_DIR  := build
DOC_DIR  := docs
INC_DIR  := include
SRC_DIR  := src

TARGET   := $(BIN_DIR)/main.exe

SOURCES := $(shell find $(SRC_DIR) -type f -name "*.cpp")
HEADERS := $(shell find $(INC_DIR) -type f \( -name "*.hpp" -o -name "*.tpp" \))

OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))
DEPS    := $(OBJECTS:.o=.d)

# --- Build Rules ---
.PHONY: build clean run docs all

build: $(TARGET)

# --- Link ---
$(TARGET): $(OBJECTS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@

# --- Compile ---
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	-include $(DEPS)

docs: $(DOC_DIR)/output/html/index.html

$(DOC_DIR)/output/html/index.html: $(SOURCES) $(HEADERS) $(DOC_DIR)/Doxyfile
	@echo "Generating documentation..."
	mkdir -p $(DOC_DIR)/output
	doxygen $(DOC_DIR)/Doxyfile

$(DOC_DIR)/Doxyfile:
	mkdir -p $(DOC_DIR)
	doxygen -g $(DOC_DIR)/Doxyfile

run: build
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(DOC_DIR)/output