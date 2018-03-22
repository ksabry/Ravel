# Setup for windows

CXX := g++

SRC_DIR := ./src
OBJ_DIR := ./obj
SRC_SUBDIRS := $(wildcard $(SRC_DIR)/**)
SRC_FILES := $(wildcard $(SRC_DIR)/**/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := 
CXXFLAGS := -std=c++17

EMPTY :=
SPACE := $(EMPTY) $(EMPTY)
INCLUDE_FLAG := $(EMPTY) -I$(EMPTY)
CXXFLAGS += $(INCLUDE_FLAG)$(subst $(SPACE),$(INCLUDE_FLAG),$(SRC_SUBDIRS))

DEP_DIR := ./.dep
OUT_DIR = ./bin
OUT_EXE = $(OUT_DIR)/main.exe

ifeq (,$(wildcard $(DEP_DIR)))
$(shell mkdir "$(DEP_DIR)")
endif

ifeq (,$(wildcard $(OBJ_DIR)))
$(shell mkdir "$(OBJ_DIR)")
endif

ifeq (,$(wildcard $(OUT_DIR)))
$(shell mkdir "$(OUT_DIR)")
endif

$(DEP_DIR)/%.d: $(SRC_DIR)/%.cpp 
	IF NOT EXIST $(dir $@) (mkdir "$(dir $@)")
	$(CXX) $(CXXFLAGS) -MM -MF $@ -MP $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEP_DIR)/%.d
	IF NOT EXIST $(dir $@) (mkdir "$(dir $@)")
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OUT_EXE): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^