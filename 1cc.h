#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define zero_punctuator 0
#define single_punctuator 1
#define multi_punctuator 2


typedef struct LVar LVar;
// type of local variable
struct LVar {
	LVar *next; // next variable is NULL
	char *name; // variable name
	int len;    // long name
	int offset; // offset from RBP
};

// For Node
// 抽象構文木のノードの種類
typedef enum {
	ND_ADD,   // +
	ND_SUB,	  // -
	ND_MUL,	  // *
	ND_DIV,	  // /
	ND_EQ,	  // ==
	ND_NE,	  // !=
	ND_LT,	  // < (>)
	ND_LE,	  // <= (>=)
	ND_ASSIGN,// =
	ND_RETURN,// return
	ND_EXPR_STMT, // Expression statement
	ND_LVAR,  // local variable
	ND_NUM,	  // Integer
} NodeKind;

// kinds token
typedef enum {
	TK_RESERVED, // symbol
	TK_IDENT,    // identifier
	TK_NUM,	     // integer token
	TK_EOF,      // token representing the end of input
} TokenKind;

typedef struct Token Token;
struct Token {
	TokenKind kind; // トークンの型
	Token *next;    // 次の入力トークン
	long val;        // kindがTK_NUMの場合、その数値
	char *str;      // トークン文字列
	int len;	// トークンの長さ
};


typedef struct Node Node;
// 抽象構文木のノードの型
struct Node {
	NodeKind kind;	// ノードの型
	Node *next;	// Next node
	Node *lhs;    	// 左辺
	Node *rhs;    	// 右辺
	long val;      	// kindがND_NUMの場合のみ使う
	LVar *lvar;   	// kindがND_LVARの場合のみ使う
};

typedef struct Function Function;
struct Function {
	Node *node;
	LVar *locals;
	int stack_size;
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
Token *consume_ident();
void expect(char *op);
long expect_number();
bool at_eof();
Token *tokenize();
Function *program();
void codegen(Function *prog);
extern char *user_input;
extern Token *token;
