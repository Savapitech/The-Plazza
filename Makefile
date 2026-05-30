MAKE_FLAGS += -j

SRC := $(wildcard src/*.cpp) $(wildcard src/Pizza/*.cpp) $(wildcard src/Pizza/Pizzas/*.cpp) $(wildcard src/IPC/*.cpp) $(wildcard src/Logger/*.cpp) $(wildcard src/Parsing_orders/*.cpp) $(wildcard src/Concurrency/*.cpp) $(wildcard src/Kitchen/*.cpp) $(wildcard src/Reception/*.cpp) $(wildcard src/Terminal/*.cpp)

BUILD_DIR := .build

CXXFLAGS += -Wall -Wextra -Werror=write-strings -g
CXXFLAGS += -Wno-unused-parameter -Wunused-result
CXXFLAGS += -Wp,-U_FORTIFY_SOURCE -Wcast-qual
CXXFLAGS += -Wformat=2 -Wshadow -fno-builtin -Wno-unused-command-line-argument 
CXXFLAGS += -Wstrict-aliasing=0 -Wunreachable-code
CXXFLAGS += -Wwrite-strings -Werror=format-nonliteral -Werror=return-type
CXXFLAGS += -std=c++20 -iquote src -iquote src/Server -iquote src/Client -iquote libs/myteams
CXXFLAGS += -Isrc/Pizza
CXXFLAGS += -Isrc/Pizza/Pizzas
CXXFLAGS += -Isrc/Parsing_orders
CXXFLAGS += -Isrc/IPC
CXXFLAGS += -Isrc/Logger
CXXFLAGS += -Isrc/Concurrency
CXXFLAGS += -Isrc/Kitchen
CXXFLAGS += -Isrc/Reception
CXXFLAGS += -Isrc/Terminal

LDLIBS += -lpthread

include utils.mk

.PHONY: _start all
_start: all

CXX = clang++

# call mk-profile release, SRC, additional CFLAGS
define mk-profile

NAME_$(strip $1) := $4
OBJ_$(strip $1) := $$($(strip $2):%.cpp=$$(BUILD_DIR)/$(strip $1)/%.o)

$$(BUILD_DIR)/$(strip $1)/%.o: %.cpp
	@ mkdir -p $$(dir $$@)
	@ $$(COMPILE.cpp) $$(CXXFLAGS) $(strip $3) $$< -o $$@
	@ $$(LOG_TIME) "$$(C_GREEN) CC $$(C_PURPLE) $$(notdir $$@) $$(C_RESET)"

$$(NAME_$(strip $1)): $$(OBJ_$(strip $1))
	@ $$(LINK.cpp) $$(OBJ_$(strip $1)) $$(LDFLAGS) $$(LDLIBS) $(strip $3) -o $$@
	@ $$(LOG_TIME) "$$(C_BLUE) LD $$(C_PURPLE) $$(notdir $$@) $$(C_RESET)"
	@ $$(LOG_TIME) "$$(C_GREEN) OK  Compilation finished $$(C_RESET)"

endef

$(eval $(call mk-profile, release, SRC, , plazza))

all: $(NAME_release)

debug: CXXFLAGS += -D DEBUG_MODE
debug: all

format:
	@ find ./ -name "*.cpp" -type f -exec clang-format -i {} ";"
	@ find ./ -name "*.hpp" -type f -exec clang-format -i {} ";"
	@ $(LOG_TIME) "$(C_BLUE) CF $(C_GREEN) Code formated  $(C_RESET)"

check_format:
	@ find ./ -name "*.cpp" -type f -exec clang-format --dry-run --Werror {} ";" 2>&1 | wc -m | grep 0 > /dev/null
	@ find ./ -name "*.hpp" -type f -exec clang-format --dry-run --Werror {} ";" 2>&1 | wc -m | grep 0 > /dev/null
	@ $(LOG_TIME) "$(C_BLUE) CF $(C_GREEN) Code formated  $(C_RESET)"

clean:
	@ $(RM) $(OBJ)
	@ $(LOG_TIME) "$(C_YELLOW) RM $(C_PURPLE) $(OBJ) $(C_RESET)"

fclean:
	@ $(RM) -r $(NAME_release) $(BUILD_DIR) tests/unit/unit_tests .cov $(PROF_DIR)
	@ $(LOG_TIME) "$(C_YELLOW) RM $(C_PURPLE) $(NAME_release) $(BUILD_DIR) $(C_RESET)"

.NOTPARALLEL: re
re:	fclean all

TESTS_DIR := tests

UNIT_SRC := $(TESTS_DIR)/unit/test_plazza.cpp \
            src/Parsing_orders/Order.cpp \
            src/Parsing_orders/Parsing.cpp \
            src/Pizza/APizza.cpp \
            src/Pizza/PizzaFactory.cpp \
            src/Pizza/Pizzas/Americana.cpp \
            src/Pizza/Pizzas/Fantasia.cpp \
            src/Pizza/Pizzas/Margarita.cpp \
            src/Pizza/Pizzas/Regina.cpp \
            src/Concurrency/Thread.cpp \
            src/Concurrency/ThreadPool.cpp

UNIT_INC := -std=c++20 \
            -Isrc -Isrc/Pizza -Isrc/Pizza/Pizzas -Isrc/Parsing_orders \
            -Isrc/IPC -Isrc/Logger -Isrc/Concurrency \
            -Isrc/Kitchen -Isrc/Reception -Isrc/Logger \
            -Wall -Wextra

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
CRITERION_INC  := -I/opt/homebrew/opt/criterion/include
CRITERION_LIBS := -L/opt/homebrew/opt/criterion/lib -lcriterion
else
CRITERION_INC  :=
CRITERION_LIBS := -lcriterion
endif

$(eval $(call mk-profile, unit, UNIT_SRC, , $(TESTS_DIR)/unit/unit_tests))

$(NAME_unit): CXXFLAGS := $(UNIT_INC) $(CRITERION_INC)
$(NAME_unit): LDLIBS := $(CRITERION_LIBS) -lpthread

.PHONY: tests_unit
tests_unit: $(NAME_unit)
	./$(NAME_unit) --verbose

.PHONY: tests_functional
tests_functional: all
	@ python3 $(TESTS_DIR)/functional/test_plazza.py

.PHONY: tests_run
tests_run: tests_unit tests_functional

COV_FLAGS := -fprofile-instr-generate -fcoverage-mapping -O0
PROF_DIR  := .cov
PROF_DATA := $(PROF_DIR)/merged.profdata

$(eval $(call mk-profile, unit_cov, UNIT_SRC, , $(PROF_DIR)/unit_tests_cov))
$(eval $(call mk-profile, main_cov, SRC,       , $(PROF_DIR)/plazza_cov))

$(PROF_DIR):
	@ mkdir -p $@

$(NAME_unit_cov): CXXFLAGS := $(UNIT_INC) $(CRITERION_INC) $(COV_FLAGS)
$(NAME_unit_cov): LDLIBS   := $(CRITERION_LIBS) -lpthread
$(NAME_unit_cov): LDFLAGS  := $(COV_FLAGS)
$(NAME_unit_cov): | $(PROF_DIR)

$(NAME_main_cov): CXXFLAGS := $(CXXFLAGS) $(COV_FLAGS)
$(NAME_main_cov): LDFLAGS  := $(COV_FLAGS)
$(NAME_main_cov): | $(PROF_DIR)

.PHONY: cov
cov: $(NAME_unit_cov) $(NAME_main_cov)
	@ mkdir -p $(PROF_DIR)
	@ rm -f $(PROF_DIR)/*.profraw
	LLVM_PROFILE_FILE="$(PROF_DIR)/unit-%p.profraw" \
		./$(NAME_unit_cov) || true
	LLVM_PROFILE_FILE="$(PROF_DIR)/func-%p.profraw" \
		PLAZZA_BIN="$(abspath $(NAME_main_cov))" \
		python3 $(TESTS_DIR)/functional/test_plazza.py || true
	llvm-profdata merge -sparse $(PROF_DIR)/*.profraw -o $(PROF_DATA)
	llvm-cov report $(NAME_main_cov) $(NAME_unit_cov) \
		-instr-profile=$(PROF_DATA) \
		--ignore-filename-regex='test_plazza\.cpp'

.PHONY: all clean fclean re tests_unit tests_functional tests_run cov

PREFIX ?=
BINDIR ?= $(PREFIX)/bin
