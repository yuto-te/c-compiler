#include "1cc.h"
// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// Returns true if s1 starts with s2.
bool startwith(char *s1, char *s2) {
  return !strncmp(s1, s2, strlen(s2));
}

// 与えられた文字が英数字化アンダースコアかどうかを判定する
int is_alnum(char c) {
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') ||
         (c == '_');
}

// 新しいトークンを作成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    // 空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (startwith(p, ";")) {
      cur = new_token(TK_RESERVED, cur, p++);
      cur->len = 1;
      continue;
    }

    if (startwith(p, "==") || startwith(p, "!=") || startwith(p, ">=") || startwith(p, "<=")) {
      cur = new_token(TK_RESERVED, cur, p);
      p += 2;
      cur->len = 2;
      continue;
    }

    if (startwith(p, ">") || startwith(p, "<")) {
      cur = new_token(TK_RESERVED, cur, p++);
      cur->len = 1;
      continue;
    }

    if (startwith(p, "+") || startwith(p, "-") || startwith(p, "*") || startwith(p, "/") || startwith(p, "(") || startwith(p, ")") || startwith(p, "=")) {
      cur = new_token(TK_RESERVED, cur, p++);
      cur->len = 1;
      continue;
    }

    if (!is_alnum(*(p-1)) && startwith(p, "if") && !is_alnum(p[2])) {
      cur = new_token(TK_IF, cur, p);
      p += 2;
      cur->len = 2;
      continue;
    }

    if (!is_alnum(*(p-1)) && startwith(p, "else") && !is_alnum(p[4])) {
      cur = new_token(TK_ELSE, cur, p);
      p += 4;
      cur->len = 4;
      continue;
    }

    if (!is_alnum(*(p-1)) && startwith(p, "while") && !is_alnum(p[5])) {
      cur = new_token(TK_WHILE, cur, p);
      p += 5;
      cur->len = 5;
      continue;
    }

    if (!is_alnum(*(p-1)) && startwith(p, "for") && !is_alnum(p[3])) {
      cur = new_token(TK_FOR, cur, p);
      p += 3;
      cur->len = 3;
      continue;
    }

    if (!is_alnum(*(p-1)) && startwith(p, "return") && !is_alnum(p[6])) {
      cur = new_token(TK_RETURN, cur, p);
      p += 6;
      cur->len = 6;
      continue;
    }

    if (cur->kind != TK_IDENT && (*p == '_' || ('a' <= *p && *p <= 'z'))) {
      cur = new_token(TK_IDENT, cur, p++);
      cur->len = 1;
      continue;
    }

    if (cur->kind == TK_IDENT && is_alnum(*p)) {
      cur->len +=1;
      p++;
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }

    error("トークナイズできません");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}