# written by ChatGPT
MAKEFLAGS+=-r -j

# Directories
BUILD_DIR := build
BIN_DIR := bin
EXAMPLES := examples
EXAMPLES_DIR := examples
TESTS := tests
TESTS_DIR := tests
MINIGRAPH_DIR := minigraph

# Source files
EXAMPLE_SRCS := $(wildcard $(EXAMPLES_DIR)/**/*.cpp)
TEST_SRCS := $(wildcard $(TESTS_DIR)/*.cpp)
MINIGRAPH_HDRS := $(wildcard $(MINIGRAPH_DIR)/*.hpp)

# Object files
EXAMPLE_OBJS := $(patsubst $(EXAMPLES_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(EXAMPLE_SRCS))
TEST_OBJS := $(patsubst $(TESTS_DIR)/%.cpp,$(BUILD_DIR)/$(TESTS_DIR)/%.o,$(TEST_SRCS))
MINIGRAPH_DEPS := $(patsubst $(MINIGRAPH_DIR)/%.hpp,$(BUILD_DIR)/%.d,$(MINIGRAPH_SRCS))

# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17

# Generate a list of targets for each subdirectory in EXAMPLES_DIR
EXAMPLE_TARGETS_FULL := $(patsubst $(EXAMPLES_DIR)/%/,$(BIN_DIR)/$(EXAMPLES_DIR)/%,$(dir $(EXAMPLE_SRCS)))

# Targets
all: $(EXAMPLES) $(TESTS) 

$(EXAMPLES): $(EXAMPLE_TARGETS_FULL)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(BUILD_DIR)/$(TESTS_DIR): $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/$(TESTS_DIR)
# $(BIN_DIR)/tests:
# 	mkdir -p $(BIN_DIR)/tests

$(BUILD_DIR)/$(EXAMPLES_DIR):$(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/examples
$(BIN_DIR)/$(EXAMPLES_DIR):$(BIN_DIR)
	@mkdir -p $(BIN_DIR)/examples

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@
$(BUILD_DIR)/$(TESTS_DIR)/%.o: $(TESTS_DIR)/%.cpp | $(BUILD_DIR)/$(TESTS_DIR) $(MINIGRAPH_HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generate a list of targets for each subdirectory in EXAMPLES_DIR
EXAMPLE_TARGETS := $(patsubst $(EXAMPLES_DIR)/%/,%,$(dir $(EXAMPLE_SRCS)))

# Rule to build each example target
define EXAMPLE_RULE
$$(BUILD_DIR)/$(EXAMPLES_DIR)/$(1)/%.o: $(EXAMPLES_DIR)/$(1)/%.cpp  | $$(BUILD_DIR)/$$(EXAMPLES_DIR) $$(MINIGRAPH_HDRS)
	@mkdir -p $$(BUILD_DIR)/$$(EXAMPLES_DIR)/$(1)
	$$(CXX) $(CXXFLAGS) -c $$< -o $$@
$$(BIN_DIR)/$(EXAMPLES_DIR)/$(1): $$(patsubst $$(EXAMPLES_DIR)/$(1)/%.cpp,$$(BUILD_DIR)/examples/$(1)/%.o,$$(wildcard $$(EXAMPLES_DIR)/$(1)/*.cpp)) | $$(BIN_DIR)/$$(EXAMPLES_DIR)
	$$(CXX) $$^ -o $$@
endef
$(foreach example,$(EXAMPLE_TARGETS),$(eval $(call EXAMPLE_RULE,$(example))))

# Link executables
$(TESTS): $(TEST_OBJS) | $(BIN_DIR)
	$(CXX) $(TEST_OBJS) -o $(BIN_DIR)/$@

# Clean target
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# PHONY targets
.PHONY: all clean