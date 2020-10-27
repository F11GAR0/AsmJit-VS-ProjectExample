#include "main.h"
#include "asmjit/asmjit.h"

using namespace asmjit;

typedef void (*asmfunc)(CodeHolder *);
typedef  void* (*func)(void*);
class CodeExecuter {
private:
	JitRuntime* m_Runtime;
public:
	CodeExecuter(JitRuntime *rt) {
		m_Runtime = rt;
	}
	func CreateFunction(asmfunc f) {
		CodeHolder code;
		code.init(m_Runtime->environment());
		f(&code);
		func ret;
		Error err = m_Runtime->add(&ret, &code);
		if (err) throw new std::exception("Runtime code adding error\n");
		return ret;
	}
};

int main() {
	JitRuntime rt;
	CodeExecuter* obf = new CodeExecuter(&rt);

	auto f = obf->CreateFunction([](CodeHolder* code) {
		x86::Assembler a(code);
		a.pushad();
		a.mov(x86::eax, x86::dword_ptr(x86::eax));
		a.push(x86::eax);
		a.mov(x86::eax, "f11gar0 number: %d\n");
		a.push(x86::eax);
		a.call(printf);
		a.pop(x86::eax); //printf result
		a.pop(x86::eax); //argv
		a.mov(x86::ecx, x86::eax);
		a.mov(x86::edi, 4); //edi = 4
		a.call(malloc); //returns address to rax
		a.mov(x86::ptr(x86::rax), x86::ecx);
		a.mov(x86::eax, x86::ecx);
		a.popad();
		a.push(x86::eax);
		a.mov(x86::eax, "f11gar0 pointer: 0x%x\n");
		a.push(x86::eax);
		a.call(printf);
		a.pop(x86::eax); //printf result
		a.pop(x86::eax); //argv
		a.ret();
	});

	int number = 8;
	int *n = (int*)f(&number);
	std::cout << *n;
	
	free(n);
	rt.release(f);
	std::cin.get();
	return 0;
}