CONFIG := NONE

ifeq ($(CONFIG),NONE)
$(error Must specify CONFIG to make (e.g. make CONFIG=DEBUG))
endif

CXX := g++

SRC_DIR := ./src
DEP_DIR := ./.dep

ifeq ($(CONFIG),DEBUG)
OBJ_DIR := ./obj-debug
OUT_DIR := ./bin-debug
else ifeq ($(CONFIG),RELEASE)
OBJ_DIR := ./obj
OUT_DIR := ./bin
endif

OUT_EXE := $(OUT_DIR)/main.exe

SRC_SUBDIRS := $(wildcard $(SRC_DIR)/**)
SRC_FILES := $(wildcard $(SRC_DIR)/**/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

CXXFLAGS_DEBUG := -g -Og
LDFLAGS_DEBUG := -g -Og
CXXFLAGS_RELEASE := -g -Ofast
LDFLAGS_RELEASE := -g -Ofast

ifeq ($(CONFIG),DEBUG)
CXXFLAGS := -std=c++17 $(CXXFLAGS_DEBUG)
LDFLAGS := $(LDFLAGS_DEBUG)
else ifeq ($(CONFIG),RELEASE)
CXXFLAGS := -std=c++17 $(CXXFLAGS_RELEASE)
LDFLAGS := $(LDFLAGS_RELEASE)
endif

EMPTY :=
SPACE := $(EMPTY) $(EMPTY)
INCLUDE_FLAG := $(EMPTY) -I$(EMPTY)
CXXFLAGS += $(INCLUDE_FLAG)$(subst $(SPACE),$(INCLUDE_FLAG),$(SRC_SUBDIRS))

ifeq (,$(wildcard $(DEP_DIR)))
$(shell mkdir -p $(DEP_DIR))
endif

ifeq (,$(wildcard $(OBJ_DIR)))
$(shell mkdir -p $(OBJ_DIR))
endif

ifeq (,$(wildcard $(OUT_DIR)))
$(shell mkdir -p $(OUT_DIR))
endif

$(DEP_DIR)/%.d: $(SRC_DIR)/%.cpp 
ifeq (,$(wildcard $(dir $@)))
	mkdir -p $(dir $@) 
endif
	$(CXX) $(CXXFLAGS) -MM -MF $@ -MP $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEP_DIR)/%.d
ifeq (,$(wildcard $(dir $@)))
	mkdir -p "$(dir $@)"
endif
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OUT_EXE): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^