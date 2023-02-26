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

typedef struct Node Node;
// 抽象構文木のノードの型
struct Node {
	NodeKind kind;	// ノードの型
	Node *next;	// Next node
	Node *lhs;    	// 左辺
	Node *rhs;    	// 右辺
	long val;      	// kindがND_NUMの場合のみ使う
	char offset;   	// kindがND_LVARの場合のみ使う
};

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

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
Token *consume_ident();
void expect(char *op);
long expect_number();
bool at_eof();
Token *tokenize();
Node *program();
void codegen(Node *node);
extern char *user_input;
extern Token *token;

