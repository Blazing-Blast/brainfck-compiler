# BFC, a simple brainf*ck compiler.
# Copyright (C) 2024  blazing-blast

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

BFC = bfc
BFC_OBJS = src/brainfuck.o
CC = g++
LD = g++
CC_ARGS = -O3 -flto
AS = nasm
AS_ARGS = -f elf64 -F dwarf
TESTS =  \
	examples/hello \
	examples/zero

all: $(BFC)

check: $(TESTS)
	

$(BFC): $(BFC_OBJS)
	@$(LD) $^ -o $@ $(CC_ARGS)
	@echo LD    $@

clean:
	@find -name *.o -delete
	@rm $(BFC) -f
	@find ./examples -type f -executable -delete
	@echo 'RM    objects'
	@echo 'RM    bfc'
	@echo 'RM    examples'
	

%: %.o src/wrapper.o
	@$(CC) $^ -o $@ -fno-pie -no-pie
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