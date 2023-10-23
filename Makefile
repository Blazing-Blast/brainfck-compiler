BFC = bfc
BFC_OBJS = src/brainfuck.o
CC = g++
LD = g++
CC_ARGS = -O3 -g
AS = nasm
AS_ARGS = -f elf64 -g -F dwarf
TESTS =  \
	examples/hello \
	examples/zero

all: $(BFC)

check: $(TESTS)
	

$(BFC): $(BFC_OBJS)
	@$(LD) $^ -o $@ $(CC_ARGS) -g
	@echo LD    $@

clean:
	@find -name *.o -delete
	@rm $(BFC) -f
	@find ./examples -type f -executable -delete
	@echo 'RM    objects'
	@echo 'RM    bfc'
	@echo 'RM    examples'
	

%: %.o src/wrapper.o
	@$(CC) $^ -o $@ -fno-pie -no-pie -g
	@echo LD    $@

%.o: %.cpp
	@$(CC) -c $^ -o $@ $(CC_ARGS)
	@echo CC    $@

%.o: %.asm
	@$(AS) $^ -o $@ $(AS_ARGS)
	@echo AS    $@

%.asm: $(BFC) %.bf 
	@echo BFC   $@
	@./$^ $@

.PHONY: clean all