#include "1cc.h"

int main(int argc, char **argv){
	if ( argc != 2 ){
		error("%d: 引数の個数が正しくありません",argc);
	}
	// Doing tokenize to parse
	user_input = argv[1];
	token = tokenize();
	Function *prog = program();

	// Assign offsets to local variables
	int offset = 0;
	for(LVar *lvar=prog->locals; lvar; lvar=lvar->next){
		offset += 8;
		lvar->offset = offset;
	}
	prog->stack_size = offset;

	// Traverse the AST to emit assembly
	codegen(prog);
	return 0;
}
