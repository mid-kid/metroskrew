MESON ?= meson

CROSS := cc-m32

build := build

.PHONY: all
all: $(build)/build.ninja
	$(MESON) compile -C $(build)

.PHONY: setup
setup: $(build)/build.ninja

.PHONY: clean
clean:
	$(MESON) compile -C $(build) --clean

.PHONY: distclean
distclean:
	rm -r $(build)

$(build)/build.ninja:
	$(MESON) setup --cross-file meson/$(CROSS).ini $(build)
