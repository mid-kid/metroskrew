CC := cc -m32
MESON ?= meson

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
	CC="$(CC)" $(MESON) setup $(build)
