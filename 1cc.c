#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define zero_punctuator 0
#define single_punctuator 1
#define multi_punctuator 2


// For Node
// 抽象構文木のノードの種類
typedef enum {
	ND_ADD, // +
	ND_SUB,	// -
	ND_MUL,	// *
	ND_DIV,	// /
	ND_EQ,	// ==
	ND_NE,	// !=
	ND_LT,	// <
	ND_LE,	// <=
	ND_NUM,	// Integer
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
	NodeKind kind; // ノードの型
	Node *lhs;     // 左辺
	Node *rhs;     // 右辺
	int val;       // kindがND_NUMの場合のみ使う
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_num(int val){
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
}

// kinds token
typedef enum {
	TK_RESERVED,
	TK_NUM,
	TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token {
	TokenKind kind; // トークンの型
	Token *next;    // 次の入力トークン
	int val;        // kindがTK_NUMの場合、その数値
	char *str;      // トークン文字列
	int len;	// トークンの長さ
};

Token *token;

void error(char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

// 入力プログラム
char *user_input;

// エラー個所を報告する
void error_at(char *loc, char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);

	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, " "); // pos個の空白を出力
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

// 次のトークンが期待している記号の時には、トークンを一つ読み進める。
// 真を返す。それ以外の場合は偽を返す。
bool consume(char *op) {
	if (token->kind != TK_RESERVED ||
		       	strlen(op) != token->len ||
			memcmp(token->str, op, token->len)){
		return false;
	}
	token = token->next;
	return true;
}

// 次のトークンが期待している記号の時には、トークンを一つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op){
	if(token->kind != TK_RESERVED ||
		       strlen(op) != token->len ||
		       memcmp(token->str, op, token->len)){
		error_at(token->str, "expected \"%s\"", op);
	}
	token = token->next;
}

// 次のトークンが数値の場合、トークンを一つ読み進めてその数値を返す。
// それ以外の場合はエラーを報告する。
int expect_number(){
	if(token->kind != TK_NUM ){
		error_at(token->str, "expected a number");
	}
	int val = token->val;
	token = token->next;
	return val;
}

bool at_eof(){
	return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->len = len;
	cur->next = tok;
	return tok;
}

bool startswith(char *p, char *q){
	return memcmp(p, q, strlen(q)) == 0;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p){
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while (*p){
		// skip space index
		if(isspace(*p)){
			p++;
			continue;
		}

		// multi-letter punctuator
		if(startswith(p, "==") || startswith(p, "!=") ||
				startswith(p, "<=") || startswith(p, ">=")){
			cur = new_token(TK_RESERVED, cur, p, multi_punctuator);
			p += 2;
			continue;
		}

		// single-letter punctuator
		if(strchr("+-*/()<>", *p)){
			cur = new_token(TK_RESERVED, cur, p++, single_punctuator);
			continue;
		}

		if (isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p, zero_punctuator);
			char *q = p;
			cur->val = strtol(p, &p, 10);
			cur->len = p - q;
			continue;
		}

		error_at(p, "expected a number");
		error_at(p, "invalid token");
	}

	new_token(TK_EOF, cur, p, zero_punctuator);
	return head.next;
}

// Parser
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

// expr = equality
Node *expr(){
	return equality();
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality(){
	Node *node = relational();

	for (;;) {
		if (consume("=="))
			node = new_node(ND_EQ, node, relational());
		else if (consume("!="))
			node = new_node(ND_NE, node, relational());
		else
			return node;
	}
}

// relational = add("<" add | "<=" add | ">" add | ">=" add)*
Node *relational(){
	Node *node = add();

	for (;;){
		if (consume("<"))
			node = new_node(ND_LT, node, add());
		else if (consume("<="))
			node = new_node(ND_LE, node, add());
		else if (consume(">"))
			node = new_node(ND_LT, add(), node);
		else if (consume(">="))
			node = new_node(ND_LE, add(), node);
		else
			return node;
	}
}

// add = mul("+" mul | "-" mul )*
Node *add(){
	Node *node = mul();

	for(;;) {
		if (consume("+"))
			node = new_node(ND_ADD, node, mul());
		else if (consume("-"))
			node = new_node(ND_SUB, node, mul());
		else
			return node;
	}
}

// mul = unary("*" unary | "/" unary)*
Node *mul(){
	Node *node = unary();

	for(;;){
		if(consume("*"))
			node = new_node(ND_MUL, node, unary());
		else if (consume("/"))
			node = new_node(ND_DIV, node, unary());
		else
			return node;
	}
}

// unary = ("+" | "-")? unary
//        | primary
Node *unary(){
	if(consume("+"))
		return primary();
	if (consume("-"))
		return new_node(ND_SUB, new_node_num(0), unary());
	return primary();
}

// primary = "(" expr ")" | num
Node *primary(){
	// 次のトークンが"("なら、"(" expr ")"のはず
	if (consume("(")) {
		Node *node = expr();
		expect(")");
		return node;
	}

	// そうでなければ数値のはず
	return new_node_num(expect_number());
}

// x86-64ベースのスタックマシン
void gen(Node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  	case ND_ADD:
    		printf("  add rax, rdi\n");
    		break;
  	case ND_SUB:
    		printf("  sub rax, rdi\n");
    		break;
  	case ND_MUL:
    		printf("  imul rax, rdi\n");
    		break;
	case ND_DIV:
	    	printf("  cqo\n");
 		printf("  idiv rdi\n");
 	   	break;
	case ND_EQ:
		printf(" cmp rax, rdi\n");
		printf(" sete al\n");
		printf(" movzb rax, al\n");
		break;
	case ND_NE:
		printf(" cmp rax, rdi\n");
		printf(" setne al\n");
		printf(" movzb rax, al\n");
		break;
	case ND_LT:
		printf(" cmp rax, rdi\n");
		printf(" setl al\n");
		printf(" movzb rax, al\n");
		break;
	case ND_LE:
		printf(" cmp rax, rdi\n");
		printf(" setle al\n");
		printf(" movzb rax, al\n");
		break;
  	}

  printf("  push rax\n");
}

int main(int argc, char **argv) {
	if(argc != 2){
		error("引数の個数が正しくありません\n");
		return 1;
	}

	//トークナイズしてパースする
	user_input = argv[1];
	token = tokenize(user_input);
	Node *node = expr();

	//アセンブリの前半部分を出力	
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	// 抽象構文木を下りながらコード生成
	gen(node);

	// スタックトップに敷き全体の値が残っているはずなので
	// それをRAXにロードして関数からの返り値とする
	printf(" pop rax\n");
	printf(" ret\n");
	return 0;
}
