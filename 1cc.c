#include "1cc.h"


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
