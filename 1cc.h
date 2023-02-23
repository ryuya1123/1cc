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
	ND_LT,	// < (>)
	ND_LE,	// <= (>=)
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

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
bool startswith(char *p, char *q);
Token *tokenize(char *p);
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
void gen(Node *node);
int main(int argc, char **argv);
static Token *token;
static char *user_input;
