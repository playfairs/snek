MESON ?= meson
NINJA ?= ninja
BUILD_DIR ?= builddir
PREFIX ?= /usr/local

.PHONY: all build setup clean run install uninstall

all: build

setup:
	$(MESON) setup $(BUILD_DIR) --prefix=$(PREFIX) --wipe 2>/dev/null || $(MESON) setup $(BUILD_DIR) --reconfigure --prefix=$(PREFIX)

build: setup
	$(MESON) compile -C $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

run: build
	./$(BUILD_DIR)/snek

install: build
	$(MESON) install -C $(BUILD_DIR)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/snek
