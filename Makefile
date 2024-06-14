MESON ?= meson

CROSS := cc-m32

build := build

.PHONY: all
all: $(build)/build.ninja
	$(MESON) compile -C $(build)

.PHONY: windows
windows: $(build)/opt.windows/build.ninja
	$(MESON) compile -C $(build)/opt.windows

.PHONY: release
release: $(build)/opt.release/build.ninja
	$(MESON) compile -C $(build)/opt.release

.PHONY: trace
trace: $(build)/opt.trace/build.ninja
	$(MESON) compile -C $(build)/opt.trace

.PHONY: setup_all
.NOTPARALLEL: setup_all
setup_all: setup setup_windows setup_release setup_trace

.PHONY: setup
setup: $(build)/build.ninja

.PHONY: setup_windows
setup_windows: $(build)/opt.windows/build.ninja

.PHONY: setup_release
setup_release: $(build)/opt.release/build.ninja

.PHONY: setup_trace
setup_trace: $(build)/opt.trace/build.ninja

.PHONY: test_all
test_all: setup_all .WAIT test test_windows test_release test_trace

.PHONY: test
test: $(build)/build.ninja
	$(MESON) test -C $(build)

.PHONY: test_windows
test_windows: $(build)/opt.windows/build.ninja
	$(MESON) test -C $(build)/opt.windows

.PHONY: test_release
test_release: $(build)/opt.release/build.ninja
	$(MESON) test -C $(build)/opt.release

.PHONY: test_trace
test_trace: $(build)/opt.trace/build.ninja
	$(MESON) test -C $(build)/opt.trace

.PHONY: clean
clean:
	! test -f $(build)/build.ninja || $(MESON) compile -C $(build) --clean
	! test -f $(build)/opt.windows/build.ninja || \
		$(MESON) compile -C $(build)/opt.windows --clean
	! test -f $(build)/opt.release/build.ninja || \
		$(MESON) compile -C $(build)/opt.release --clean
	! test -f $(build)/opt.trace/build.ninja || \
		$(MESON) compile -C $(build)/opt.trace --clean

.PHONY: distclean
distclean:
	rm -rf $(build)
	$(MESON) subprojects purge --confirm

$(build)/build.ninja:
	$(MESON) setup --cross-file meson/$(CROSS).ini $(build)

$(build)/opt.windows/build.ninja:
	@mkdir -p $(build)
	$(MESON) setup --cross-file meson/i686-w64-mingw32.ini $(build)/opt.windows \
		-Dtrace=true

$(build)/opt.release/build.ninja:
	@mkdir -p $(build)
	$(MESON) setup --cross-file meson/$(CROSS).ini $(build)/opt.release \
		--buildtype release

$(build)/opt.trace/build.ninja:
	@mkdir -p $(build)
	$(MESON) setup --cross-file meson/$(CROSS).ini $(build)/opt.trace \
		-Dtrace=true
