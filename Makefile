SHELL = bash
export LANG = C

SRC_DIR   = src
OUT_DIR   = out
OBJ_DIR   = $(OUT_DIR)/.o
TESTS_DIR = tests

TESTS_CF_DIR = $(TESTS_DIR)/compile_fail
TESTS_OK_DIR = $(TESTS_DIR)/passing
TESTS_IN_DIR = $(TESTS_DIR)/integration
TESTS_CO_DIR = $(TESTS_DIR)/common

SRC_FILES = $(wildcard $(SRC_DIR)/*)
TESTS_CO_FILES = $(wildcard $(TESTS_CO_DIR)/*)
ALL_DEPENDENCIES = $(SRC_FILES) $(TESTS_CO_FILES)

TESTS_CF_SRC = $(wildcard $(TESTS_CF_DIR)/*.cpp)
TESTS_OK_SRC = $(wildcard $(TESTS_OK_DIR)/*.cpp)
TESTS_IN_SRC = $(wildcard $(TESTS_IN_DIR)/*.cpp)

TESTS_IN_OBJ = $(TESTS_IN_SRC:$(TESTS_DIR)/%.cpp=$(OBJ_DIR)/%.o)

TESTS_CF_NAMES = $(TESTS_CF_SRC:$(TESTS_CF_DIR)/%.cpp=%)
TESTS_OK_NAMES = $(TESTS_OK_SRC:$(TESTS_OK_DIR)/%.cpp=%)

TESTS_OK_EXECS = $(TESTS_OK_EXECS:%=$(OUT_DIR)/rf-%)

TESTS_CF_TARGETS = $(TESTS_CF_NAMES:%=virt/run-cf-%)
TESTS_OK_TARGETS = $(TESTS_OK_NAMES:%=virt/run-ok-%)

ALL_TESTS_TARGETS = $(TESTS_CF_TARGETS) $(TESTS_OK_TARGETS) $(TESTS_IN_TARGETS)

CXXFLAGS = -iquote $(SRC_DIR) -iquote $(TESTS_DIR)/common -std=c++17 -Wfatal-errors

##### Targets #####

virt/all-tests: virt/cf-tests virt/ok-tests virt/integration
	tools/check_cf_out.sh

virt/cf-tests: $(TESTS_CF_TARGETS)

virt/ok-tests: $(TESTS_OK_TARGETS)

$(OUT_DIR):
	@ mkdir -p $@

virt/all-tests-deps: $(OUT_DIR) $(ALL_DEPENDENCIES)

define TESTS_CF_GENERATOR
virt/run-cf-$(1): $$(TESTS_CF_DIR)/$(1).cpp
	! $$(CXX) $$(CXXFLAGS) $$< -o /dev/null 2>$$(TESTS_CF_DIR)/$(1).out
endef
$(foreach i,$(TESTS_CF_NAMES),$(eval $(call TESTS_CF_GENERATOR,$(i))))

define TESTS_OK_GENERATOR
$$(OUT_DIR)/$(1): $$(TESTS_OK_DIR)/$(1).cpp virt/all-tests-deps
	$$(CXX) $$(CXXFLAGS) $$< -o $$@

virt/run-ok-$(1): $$(OUT_DIR)/$(1)
	$$<
endef
$(foreach i,$(TESTS_OK_NAMES),$(eval $(call TESTS_OK_GENERATOR,$(i))))

$(OBJ_DIR)/%.o: $(TESTS_DIR)/%.cpp virt/all-tests-deps
	mkdir -p "$$(dirname "$@")"
	$(CXX) $(CXXFLAGS) $< -c -o $@

$(OUT_DIR)/integration: $(TESTS_IN_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

virt/integration: $(OUT_DIR)/integration
	$<

clean:
	rm -rf $(OUT_DIR)

.PHONY: clean virt/all-tests virt/cf-tests virt/ok-tests virt/all-tests-deps $(ALL_TESTS_TARGETS)
