#include "1cc.h"

char *user_input;
Token *token;

void error(char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

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
			strncmp(token->str, op, token->len)){
		return false;
	}
	token = token->next;
	return true;
}

Token *consume_ident(){
	if(token->kind != TK_IDENT){
		return NULL;
	}
	Token *t = token;
	token = token->next;
	return t;
}

// 次のトークンが期待している記号の時には、トークンを一つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op){
	if(token->kind != TK_RESERVED ||
		       strlen(op) != token->len ||
		       strncmp(token->str, op, token->len)){
		error_at(token->str, "expected \"%s\"", op);
	}
	token = token->next;
}

// 次のトークンが数値の場合、トークンを一つ読み進めてその数値を返す。
// それ以外の場合はエラーを報告する。
long expect_number(){
	if(token->kind != TK_NUM ){
		error_at(token->str, "expected a number");
	}
	long val = token->val;
	token = token->next;
	return val;
}

bool at_eof(){
	return token->kind == TK_EOF;
}

// 新しいトークンを作成してcurに繋げる
static Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->len = len;
	cur->next = tok;
	return tok;
}

static bool startswith(char *p, char *q){
	return strncmp(p, q, strlen(q)) == 0;
}

static bool is_alpha(char c){
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

static bool is_alnum(char c){
	return is_alpha(c) || ('0' <= c && c <= '9');
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(){
	char *p = user_input;
	Token head = {};
	Token *cur = &head;

	while (*p){
		// skip space index
		if(isspace(*p)){
			p++;
			continue;
		}

		// keywords
		if (startswith(p, "return") && !is_alnum(p[6])){
				cur = new_token(TK_RESERVED, cur, p, 6);
				p += 6;
				continue;
		}

		// Identifier
		if('a'<= *p && *p <= 'z'){
			cur = new_token(TK_IDENT, cur, p++, single_punctuator);
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
		if(ispunct(*p)){
			cur = new_token(TK_RESERVED, cur, p++, single_punctuator);
			continue;
		}


		// Integer literal
		if (isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p, zero_punctuator);
			char *q = p;
			cur->val = strtol(p, &p, 10);
			cur->len = p - q;
			continue;
		}

		error_at(p, "invalid token");
	}

	new_token(TK_EOF, cur, p, zero_punctuator);
	return head.next;
}
