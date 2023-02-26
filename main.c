#include "1cc.h"

int main(int argc, char **argv){
	if ( argc != 2 ){
		error("%d: 引数の個数が正しくありません",argc);
	}
	// Doing tokenize to parse
	user_input = argv[1];
	token = tokenize();
	Node *node = program();

	// Traverse the AST to emit assembly
	codegen(node);
	return 0;
}
