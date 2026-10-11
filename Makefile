CC ?= cc
AR ?= ar
ARFLAGS := rcs

CPPFLAGS ?=
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
LDFLAGS ?=
LDLIBS ?=

BUILD_DIR := build

IMMUTABLE_OBJECT := $(BUILD_DIR)/immutable/chaines.o
DYNAMIC_OBJECT := $(BUILD_DIR)/dynamic/chaines.o
IMMUTABLE_LIBRARY := $(BUILD_DIR)/libchaines_immutable.a
DYNAMIC_LIBRARY := $(BUILD_DIR)/libchaines_dynamic.a
IMMUTABLE_TEST := $(BUILD_DIR)/test_immutable
DYNAMIC_TEST := $(BUILD_DIR)/test_dynamic

.PHONY: all libs test sanitize clean

all: test

libs: $(IMMUTABLE_LIBRARY) $(DYNAMIC_LIBRARY)

test: $(IMMUTABLE_TEST) $(DYNAMIC_TEST)
	./$(IMMUTABLE_TEST)
	./$(DYNAMIC_TEST)

$(IMMUTABLE_OBJECT): src/immutable/chaines.c include/immutable/chaines.h
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Iinclude/immutable -c $< -o $@

$(DYNAMIC_OBJECT): src/dynamic/chaines.c include/dynamic/chaines.h
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Iinclude/dynamic -c $< -o $@

$(IMMUTABLE_LIBRARY): $(IMMUTABLE_OBJECT)
	$(AR) $(ARFLAGS) $@ $^

$(DYNAMIC_LIBRARY): $(DYNAMIC_OBJECT)
	$(AR) $(ARFLAGS) $@ $^

$(IMMUTABLE_TEST): tests/test_immutable.c $(IMMUTABLE_LIBRARY)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Iinclude/immutable $< $(IMMUTABLE_LIBRARY) $(LDFLAGS) $(LDLIBS) -o $@

$(DYNAMIC_TEST): tests/test_dynamic.c $(DYNAMIC_LIBRARY)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Iinclude/dynamic $< $(DYNAMIC_LIBRARY) $(LDFLAGS) $(LDLIBS) -o $@

sanitize:
	$(MAKE) clean
	$(MAKE) \
		CFLAGS="-std=c11 -O1 -g -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-omit-frame-pointer" \
		LDFLAGS="-fsanitize=address,undefined" \
		test

clean:
	$(RM) -r $(BUILD_DIR)
