MESON ?= meson

CROSS := cc-m32

build := build

.PHONY: all
all: $(build)/build.ninja
	$(MESON) compile -C $(build)

.PHONY: release
release: $(build)/opt.release/build.ninja
	$(MESON) compile -C $(build)/opt.release

.PHONY: trace
trace: $(build)/opt.trace/build.ninja
	$(MESON) compile -C $(build)/opt.trace

.PHONY: setup
setup: $(build)/build.ninja
setup: $(build)/opt.release/build.ninja
setup: $(build)/opt.trace/build.ninja

.PHONY: clean
clean:
	! test -f $(build)/build.ninja || $(MESON) compile -C $(build) --clean
	! test -f $(build)/opt.release/build.ninja || \
		$(MESON) compile -C $(build)/opt.release --clean
	! test -f $(build)/opt.trace/build.ninja || \
		$(MESON) compile -C $(build)/opt.trace --clean

.PHONY: distclean
distclean:
	rm -rf $(build)

$(build)/build.ninja:
	$(MESON) setup --cross-file meson/$(CROSS).ini $(build)

$(build)/opt.release/build.ninja:
	@mkdir -p $(build)
	$(MESON) setup --cross-file meson/$(CROSS).ini $(build)/opt.release \
		--buildtype release

$(build)/opt.trace/build.ninja:
	@mkdir -p $(build)
	$(MESON) setup --cross-file meson/$(CROSS).ini $(build)/opt.trace \
		-Dtrace=true
