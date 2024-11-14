MESON ?= meson

CROSS := cc-m32

build := build
build_release := $(build)/opt.release
build_trace := $(build)/opt.trace
build_windows := $(build)/opt.windows

.PHONY: all
all: $(build)/build.ninja
	$(MESON) compile -C $(build)

.PHONY: release
release: $(build_release)/build.ninja
	$(MESON) compile -C $(build_release)

.PHONY: trace
trace: $(build_trace)/build.ninja
	$(MESON) compile -C $(build_trace)

.PHONY: windows
windows: $(build_windows)/build.ninja
	$(MESON) compile -C $(build_windows)

.PHONY: setup_all
.NOTPARALLEL: setup_all
setup_all: setup setup_release setup_trace setup_windows

.PHONY: setup
setup: $(build)/build.ninja

.PHONY: setup_release
setup_release: $(build_release)/build.ninja

.PHONY: setup_trace
setup_trace: $(build_trace)/build.ninja

.PHONY: setup_windows
setup_windows: $(build_windows)/build.ninja

.PHONY: test_all
test_all: setup_all .WAIT test test_release test_trace test_windows

.PHONY: test
test: $(build)/build.ninja
	$(MESON) test -C $(build)

.PHONY: test_release
test_release: $(build_release)/build.ninja
	$(MESON) test -C $(build_release)

.PHONY: test_trace
test_trace: $(build_trace)/build.ninja
	$(MESON) test -C $(build_trace)

.PHONY: test_windows
test_windows: $(build_windows)/build.ninja
	wineserver -p1800 || test $$? = 2
	$(MESON) test -C $(build_windows)

.PHONY: patchgen
patchgen: $(build)/build.ninja
	$(MESON) compile -C $(build) patchgen

.PHONY: package_release
package_release: $(build_release)/build.ninja
	rm -rf $(build_release)/install
	$(MESON) install -C $(build_release) --destdir install

.PHONY: package_windows
package_windows: $(build_windows)/build.ninja
	rm -rf $(build_windows)/install
	$(MESON) install -C $(build_windows) --destdir install

.PHONY: clean
clean:
	! test -f $(build)/build.ninja || \
		$(MESON) compile -C $(build) --clean
	! test -f $(build_release)/build.ninja || \
		$(MESON) compile -C $(build_release) --clean
	! test -f $(build_trace)/build.ninja || \
		$(MESON) compile -C $(build_trace) --clean
	! test -f $(build_windows)/build.ninja || \
		$(MESON) compile -C $(build_windows) --clean

.PHONY: distclean
distclean:
	rm -rf $(build)
	$(MESON) subprojects purge --confirm || true

$(build)/build.ninja:
	$(MESON) setup --cross-file meson/$(CROSS).ini $(build)

$(build_release)/build.ninja:
	@mkdir -p $(build)
	$(MESON) setup --cross-file meson/$(CROSS).ini $(build_release) \
		--prefix / --buildtype release

$(build_trace)/build.ninja:
	@mkdir -p $(build)
	$(MESON) setup --cross-file meson/$(CROSS).ini $(build_trace) \
		-Dtrace=true

$(build_windows)/build.ninja:
	@mkdir -p $(build)
	$(MESON) setup --cross-file meson/i686-w64-mingw32.ini $(build_windows) \
		--prefix / -Dtrace=true
