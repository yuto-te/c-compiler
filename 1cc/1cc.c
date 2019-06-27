#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの型を表す値
enum{
  TK_NUM = 256, // 整数トークン
  TK_EOF        // 入力の終わりを示すトークン
};

typedef struct Token_
{
  int ty;      // トークンの型
  int val;     // tyがTK_NUMのとき，その数値
  char *input; // トークン文字列(エラーメッセージ用)
} Token;

// 入力プログラム
char *user_input;

// トークナイズした結果のトークン列はこの配列に保存する
// 100個以上のトークンは来ないものとする
Token tokens[100];

// エラーを報告するための関数
// printfと同じ引数を取る
void error(char *format, ...) {
  va_list ap;
  // 可変長引数を１個の変数にまとめる
  va_start(ap, format);
  // まとめられた変数で処理する
  vfprintf(stderr, format, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// エラー箇所を報告するための関数
void error_at(char *loc, char *msg) {
  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, ""); // pos個の空白を出力
  fprintf(stderr, "^ %s\n", msg);
  exit(1);
}

// user_inputがさしている文字列をトークンに分割してtokensに保存する
void tokenize() {
  char *p = user_input;
  int i = 0; // dummy index

  while (*p) {
    // 空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }
    // 加減算
    if (*p == '+' || *p == '-') {
      tokens[i].ty = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }
    // 数値
    if (isdigit(*p)) {
      tokens[i].ty = TK_NUM;
      tokens[i].input = p;
      tokens[i].val = strtol(p, &p, 10);
      i++;
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  tokens[i].ty = TK_EOF;
  tokens[i].input = p;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  user_input = argv[1];
  tokenize();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");
  // 式の最初は数でなければならないので、それをチェックして最初のmov命令を出力
  if (tokens[0].ty != TK_NUM)
    error_at(tokens[0].input, "数ではありません");
  printf("  mov rax, %d\n", tokens[0].val);

  int i = 1;
  while (tokens[i].ty != TK_EOF) {
    if (tokens[i].ty == '+') {
      i++;
      if (tokens[i].ty != TK_NUM)
        error_at(tokens[i].input, "数ではありません");
      printf("  add rax, %d\n", tokens[i].val);
      i++;
      continue;
    }
    if (tokens[i].ty == '-') {
      i++;
      if (tokens[i].ty != TK_NUM)
        error_at(tokens[i].input, "数ではありません");
      printf("  sub rax, %d\n", tokens[i].val);
      i++;
      continue;
    }
    error_at(tokens[i].input, "予期しないトークンです");
  }
  printf("  ret\n");
  return 0;
}