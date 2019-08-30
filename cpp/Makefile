SHELL = bash
export LANG = C

SRC_DIR   = src
OUT_DIR   = out
OBJ_DIR   = $(OUT_DIR)/.o
TESTS_DIR = tests

TESTS_CF_DIR = $(TESTS_DIR)/compile_fail
TESTS_OK_DIR = $(TESTS_DIR)/passing
TESTS_IN_DIR = $(TESTS_DIR)/integration
TESTS_11_DIR = $(TESTS_IN_DIR)/cpp11
TESTS_17_DIR = $(TESTS_IN_DIR)/cpp17
TESTS_CO_DIR = $(TESTS_DIR)/common

SRC_FILES = $(wildcard $(SRC_DIR)/*/*)
TESTS_CO_FILES = $(wildcard $(TESTS_CO_DIR)/*)
ALL_DEPENDENCIES = $(SRC_FILES) $(TESTS_CO_FILES)

TESTS_CF_SRC = $(wildcard $(TESTS_CF_DIR)/*.cpp)
TESTS_OK_SRC = $(wildcard $(TESTS_OK_DIR)/*.cpp)
TESTS_11_SRC = $(wildcard $(TESTS_11_DIR)/*.cpp)
TESTS_17_SRC = $(wildcard $(TESTS_17_DIR)/*.cpp)

TESTS_11_OBJ = $(TESTS_11_SRC:$(TESTS_11_DIR)/%.cpp=$(OBJ_DIR)/cpp11-%.o)
TESTS_17_OBJ = $(TESTS_17_SRC:$(TESTS_17_DIR)/%.cpp=$(OBJ_DIR)/cpp17-%.o)

TESTS_CF_NAMES = $(TESTS_CF_SRC:$(TESTS_CF_DIR)/%.cpp=%)
TESTS_OK_NAMES = $(TESTS_OK_SRC:$(TESTS_OK_DIR)/%.cpp=%)

TESTS_OK_EXECS = $(TESTS_OK_EXECS:%=$(OUT_DIR)/rf-%)

TESTS_CF_TARGETS = $(TESTS_CF_NAMES:%=virt/run-cf-%)
TESTS_OK_TARGETS = $(TESTS_OK_NAMES:%=virt/run-ok-%)
TESTS_IN_TARGETS = virt/integration/cpp11 virt/integration/cpp17

ALL_TESTS_TARGETS = $(TESTS_CF_TARGETS) $(TESTS_OK_TARGETS) $(TESTS_IN_TARGETS)

CXXFLAGS = -iquote $(SRC_DIR) -iquote $(TESTS_DIR)/common -g -Wfatal-errors
CXX11FLAGS = $(CXXFLAGS) -std=c++11
CXX17FLAGS = $(CXXFLAGS) -std=c++17

CXXFLAGS_IN = -pedantic -Wall -Wextra -Werror
CXX11FLAGS_IN = $(CXX11FLAGS) $(CXXFLAGS_IN)
CXX17FLAGS_IN = $(CXX17FLAGS) $(CXXFLAGS_IN)

##### Targets #####

virt/all: virt/all-tests virt/lint

virt/lint:
	cpplint --extensions=h,inc $(SRC_FILES)

virt/all-tests: virt/cf-tests virt/ok-tests virt/in-tests
	../tools/check_cf_out.sh

virt/cf-tests: $(TESTS_CF_TARGETS)

virt/ok-tests: $(TESTS_OK_TARGETS)

virt/in-tests: $(TESTS_IN_TARGETS)

$(OUT_DIR):
	@ mkdir -p $@

virt/all-tests-deps: $(OUT_DIR) $(ALL_DEPENDENCIES)

define TESTS_CF_GENERATOR
virt/run-cf-$(1): $$(TESTS_CF_DIR)/$(1).cpp
	@../tools/run_cf.sh "$(1)" "$$(TESTS_CF_DIR)/$(1).out" $$(CXX) $$(CXX17FLAGS) $$< -o /dev/null
endef
$(foreach i,$(TESTS_CF_NAMES),$(eval $(call TESTS_CF_GENERATOR,$(i))))

define TESTS_OK_GENERATOR
$$(OUT_DIR)/$(1): $$(TESTS_OK_DIR)/$(1).cpp virt/all-tests-deps
	$$(CXX) $$(CXX17FLAGS) $$< -o $$@

virt/run-ok-$(1): $$(OUT_DIR)/$(1)
	$$<
endef
$(foreach i,$(TESTS_OK_NAMES),$(eval $(call TESTS_OK_GENERATOR,$(i))))

$(OBJ_DIR)/cpp11-%.o: $(TESTS_11_DIR)/%.cpp virt/all-tests-deps
	mkdir -p "$$(dirname "$@")"
	$(CXX) $(CXX11FLAGS_IN) $< -c -o $@

$(OUT_DIR)/in-cpp11: $(TESTS_11_OBJ)
	$(CXX) $(CXX11FLAGS_IN) $^ -o $@

virt/integration/cpp11: $(OUT_DIR)/in-cpp11
	$<

$(OBJ_DIR)/cpp17-%.o: $(TESTS_17_DIR)/%.cpp virt/all-tests-deps
	mkdir -p "$$(dirname "$@")"
	$(CXX) $(CXX17FLAGS_IN) $< -c -o $@

$(OUT_DIR)/in-cpp17: $(TESTS_17_OBJ)
	$(CXX) $(CXX17FLAGS_IN) $^ -o $@

virt/integration/cpp17: $(OUT_DIR)/in-cpp17
	$<

clean:
	rm -rf $(OUT_DIR)

.PHONY: clean virt/all virt/lint virt/all-tests virt/cf-tests virt/ok-tests virt/in-tests virt/all-tests-deps $(ALL_TESTS_TARGETS)
