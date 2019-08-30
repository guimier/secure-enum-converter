SHELL = bash
export LANG = C

TESTS_DIR = tests
TOOLS = $(CURDIR)/../tools

TESTS_CF_DIR = $(TESTS_DIR)/compile_fail
TESTS_CF_SRC = $(filter-out %.out,$(wildcard $(TESTS_CF_DIR)/*/))
TESTS_CF_NAMES = $(TESTS_CF_SRC:$(TESTS_CF_DIR)/%/=%)
TESTS_CF_TARGETS = $(TESTS_CF_NAMES:%=virt/run-cf-%)
ALL_TESTS_TARGETS = $(TESTS_CF_TARGETS)

##### Targets #####

virt/all: virt/all-tests

virt/all-tests: virt/ok-tests virt/cf-tests
	$(TOOLS)/check_cf_out.sh

virt/ok-tests: $(find bidi* -name \*.rs -or -name \*.toml)
	cargo test

virt/cf-tests: $(TESTS_CF_TARGETS)

define TESTS_CF_GENERATOR
virt/run-cf-$(1): $$(find $$(TESTS_CF_DIR) -name \*.rs)
	@cd "$$(TESTS_CF_DIR)/$(1)"; rm -rf target; $(TOOLS)/run_cf.sh "$(1)" "../$(1).out" cargo build
	@sed -i "/^   Compiling/d" "$$(TESTS_CF_DIR)/$(1).out"
endef
$(foreach i,$(TESTS_CF_NAMES),$(eval $(call TESTS_CF_GENERATOR,$(i))))

.PHONY: clean virt/all virt/all-tests virt/ok-tests virt/cf-test $(ALL_TESTS_TARGETS)
