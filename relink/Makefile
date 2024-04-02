ASFLAGS := -g -I ../bins $(ASFLAGS)
LDFLAGS := -g -Wl,-z,noexecstack -no-pie
TARGET_ARCH := -m32
TARGET_MACH := --32

objects := \
	entry.o \
	patch.o \
	utils.o \
	mwccarm.o \
	dlls/advapi32.o \
	dlls/kernel32.o \
	dlls/lmgr8c.o \
	dlls/user32.o \
	dlls/version.o \
	dlls/ws2_32.o \
	dlls/ole32.o

.PHONY: all
all: mwccarm

.PHONY: clean
clean:
	rm -f mwccarm mwccarm.s $(objects)

mwccarm: mwccarm.ld $(objects)
	$(LINK.o) $(OUTPUT_OPTION) -T $^

mwccarm.s: ../main
	../main ../bins/mwccarm.exe > $@
