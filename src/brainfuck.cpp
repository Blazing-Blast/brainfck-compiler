#include <iostream>
#include <fstream>
#include <vector>

void compile(std::ostream &out, std::istream &in);

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
void compile(std::ostream &out, std::istream &in)
{
	loop_vec.push_back(__SIZE_MAX__);
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
		next = in.get();
		size_t n = loop_vec.back();
		switch (next)
		{
		case '+':
			out << "    inc  al\n";
			break;
		case '-':
			out << "    dec  al\n";
			break;
		case '[':
			loop_vec.push_back(loops);
			out << ".L" << loops << ":\n"
				<< "    test rax, rax\n"
				<< "    jz   .L" << loops << "end\n";

			loops++;
			break;
		case ']':
			out
				<< "    jmp  .L" << n << '\n'
				<< ".L" << n << "end:\n";

			loop_vec.pop_back();
			break;
		case '<': // Only read/write memory when it is needed. Otherwise just keep it in a register.
			out << "    mov [array + rcx], al\n"
				<< "    dec  rcx\n"
				<< "    mov  al, [array + rcx]\n";
			len--;
			break;
		case '>':
			out << "    mov [array + rcx], al\n"
				<< "    inc  rcx\n"
				<< "    mov  al, [array + rcx]\n";
			len++;
			if (len > max_len)
				max_len = len;
			break;
		case '.':
			out << "    mov  rdi, rax\n"
				<< "    mov  rsi, [stdout]\n"
				<< "    push rax\n"
				<< "    push rcx\n"
				<< "    call putc\n"
				<< "    pop  rcx\n"
				<< "    pop  rax\n";
			break;
		case ',':
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
			break;
		}
	}
compile_end:
	out << "    ret\n"
		<< "section .data\n"
		<< "array times " << max_len << " db 0\n"
		<< "string dq 0, 0\n";
}
