ASFLAGS := -g -I ../bins $(ASFLAGS)
LDFLAGS := -g -Wl,-z,noexecstack -no-pie
TARGET_ARCH := -m32
TARGET_MACH := --32 -march=i386

name := mwasmarm

objects := \
	$(name).o \
	entry.o \
	patch.o \
	utils.o \
	dlls/advapi32.o \
	dlls/kernel32.o \
	dlls/lmgr8c.o \
	dlls/user32.o \
	dlls/version.o \
	dlls/ws2_32.o \
	dlls/ole32.o

.PHONY: all
all: $(name)

.PHONY: clean
clean:
	rm -f $(name) $(name).s $(objects)

.PHONY: dbg rel
dbg: ASFLAGS := --defsym TRACE=1 $(ASFLAGS)
dbg: all
rel: ASFLAGS := --defsym NDEBUG=1 $(ASFLAGS)
rel: all

$(name): $(name).ld $(objects)
	$(LINK.o) $(OUTPUT_OPTION) -T $^

$(name).s: ../main
	../main ../bins/$(name).exe > $@ || { rm $@; false; }

../main: ;
