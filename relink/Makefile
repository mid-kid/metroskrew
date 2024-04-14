CFLAGS := -g $(CFLAGS)
ASFLAGS := -g -I ../bins $(ASFLAGS)
LDFLAGS := -g -Wl,-z,noexecstack -no-pie
TARGET_ARCH := -m32 -march=i386
TARGET_MACH := --32 -march=i386

tools := mwccarm mwasmarm

objects := \
	entry.o \
	utils.o \
	dlls/advapi32.o \
	dlls/kernel32.o \
	dlls/lmgr8c.o \
	dlls/user32.o \
	dlls/version.o \
	dlls/ws2_32.o \
	dlls/ole32.o

.PHONY: all
all: $(addsuffix .exe,$(tools))

.PHONY: clean
clean:
	rm -f $(foreach x,$(tools),$x.exe $x.s $x.o $x_patch.o) $(objects)

.PHONY: dbg rel
dbg: ASFLAGS := --defsym TRACE=1 $(ASFLAGS)
dbg: all
rel: ASFLAGS := --defsym NDEBUG=1 $(ASFLAGS)
rel: all

$(addsuffix .exe,$(tools)): %.exe: %.ld %.o %_patch.o $(objects)
	$(LINK.o) $(OUTPUT_OPTION) -T $^

$(addsuffix .s,$(tools)): %.s: ../main
	../main ../bins/$*.exe > $@ || { rm $@; false; }

# Shitty include-dependencies
$(addsuffix .o,$(tools)): $(wildcard *.i)
$(addsuffix _patch.o,$(tools)): $(wildcard *.i)
$(objects): $(wildcard *.i)

../main: ;
