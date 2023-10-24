/*
	BFC, a simple brainf*ck compiler.
	Copyright (C) 2024  blazing-blast

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <fstream>
#include <vector>

void compile(std::ostream &out, std::istream &in);
int inc(std::istream &in);
int mov(std::istream &in);
int cal(std::istream &in);

std::streambuf *buf;
std::ofstream _of;
std::ifstream _if;

int main(int argc, char **argv)
{
	if (argc <= 2)
		return (EXIT_FAILURE);
	_if.open(argv[1]);
	buf = _if.rdbuf();
	std::istream in(buf);

	_of.open(argv[2]);
	buf = _of.rdbuf();
	std::ostream out = std::ostream(buf);

	compile(out, in);

	return 0;
}

std::vector<size_t> loop_vec(0);
size_t loops = 0; // Every label can only be used once. So make sure to keep a count of loops used.
size_t len = 1;
size_t max_len = 1;
int next;
int num = 0;
void compile(std::ostream &out, std::istream &in)
{
	loop_vec.push_back(__SIZE_MAX__);
	next = in.get();
	out << "section .text\n"
		<< "extern putc\n"
		<< "extern fgets\n"
		<< "extern stdin\n"
		<< "extern stdout\n"
		<< "global brainfuck\n"
		<< "default rel\n"
		<< "brainfuck:\n"
		<< "    xor  rcx, rcx\n"
		<< "    xor  rax, rax\n";
	while (1)
	{
		size_t n = loop_vec.back();
		switch (next)
		{
		case '+':
			next = in.get();
			if (next == '+' || next == '-')
			{
				num = 1;
				out << "    add  al, " << inc(in) << '\n';
			}
			else
				out << "    inc  al\n";
			break;
		case '-':
			next = in.get();
			if (next == '+' || next == '-')
			{
				num = -1;
				out << "    add  al, " << inc(in) << '\n';
			}
			else
				out << "    dec  al\n";
			break;
		case '[':
			next = in.get();
			loop_vec.push_back(loops);
			out << ".L" << loops << ":\n"
				<< "    test rax, rax\n"
				<< "    jz   .L" << loops << "end\n";

			loops++;
			break;
		case ']':
			next = in.get();
			out
				<< "    jmp  .L" << n << '\n'
				<< ".L" << n << "end:\n";

			loop_vec.pop_back();
			break;
		case '<': // Only read/write memory when it is needed. Otherwise just keep it in a register.
			next = in.get();
			out << "    mov [array + rcx], al\n";
			if (next == '<' || next == '>')
			{
				num = -1;
				out << "    add  rcx, " << mov(in) << '\n';
			}
			else
				out << "    dec  rcx\n";
			out << "    mov  al, [array + rcx]\n";
			len += num;
			break;
		case '>':
			next = in.get();
			out << "    mov [array + rcx], al\n";
			if (next == '<' || next == '>')
			{
				num = 1;
				out << "    add  rcx, " << mov(in) << '\n';
			}
			else
				out << "    inc  rcx\n";
			out << "    mov  al, [array + rcx]\n";
			len += num;
			if (max_len < len)
				max_len = len;
			break;
		case '.':
			next = in.get();
			out << "    mov  rdi, rax\n"
				<< "    mov  rsi, [stdout]\n"
				<< "    push rax\n"
				<< "    push rcx\n";
			if (next == '.')
			{
				num = 2;
				for (size_t i = cal(in); i >= 0; i++)
					out << "    call putc\n";
			}
			else
				out << "    call putc\n";
			out << "    pop  rcx\n"
				<< "    pop  rax\n";
			break;
		case ',':
			next = in.get();
			out << "    mov  rdi, string\n"
				<< "    mov  rsi, 2\n"
				<< "    mov  rdx, [stdin]\n"
				<< "	push rcx\n"
				<< "	call fgets\n"
				<< "	pop  rcx\n"
				<< "	mov  al, [string]\n";

			break;
		case std::char_traits<char>::eof():
			goto compile_end;
			break;
		default:
			next = in.get();
			break;
		}
	}
compile_end:
	out << "    ret\n"
		<< "section .data\n"
		<< "array times " << max_len << " db 0\n"
		<< "string dq 0, 0\n";
}

int inc(std::istream &in)
{
	while (1)
	{
		switch (next)
		{
		case '+':
			num++;
			break;
		case '-':
			num--;
			break;
		default:
			return num;
		}
		next = in.get();
	}
}

int mov(std::istream &in)
{
	while (1)
	{
		switch (next)
		{
		case '>':
			num++;
			len++;
			break;
		case '<':
			num--;
			len--;
			break;
		default:
			return num;
		}
		next = in.get();
	}
}

int cal(std::istream &in)
{
	while (1)
	{
		switch (next)
		{
		case '.':
			num++;
			break;
		default:
			return num;
		}
		next = in.get();
	}
}