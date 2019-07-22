#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを表すトークン
  TK_RETURN,   // return
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token {
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kindがTK_NUMの場合，その数値
  char *str;      // トークン文字列
  int len         // トークンの長さ
};

// 抽象構文木のノードの種類
typedef enum {
  ND_ADD,    // +
  ND_SUB,    // -
  ND_MUL,    // *
  ND_DIV,    // /
  ND_ASSIGN, // =
  ND_LVAR,   // ローカル変数
  ND_EQ,     // ==
  ND_NEQ,    // !=
  ND_ELT,    // <=
  ND_LT,     // <
  ND_NUM,    // 整数
  ND_RETURN, // return
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind; // ノードの型
  Node *lhs;     // 左辺
  Node *rhs;     // 右辺
  int val;       // kindがND_NUMの場合のみ使う
  int offset;    // kindがND_LVARの場合のみ使う
};

typedef struct LVar LVar;

// ローカル変数の型
struct LVar{
  LVar *next; // 次の変数かNULL
  char *name; // 変数の名前
  int len;    // 名前の長さ
  int offset; // RBPからのオフセット
};

// ローカル変数
LVar *locals;

// 現在着目しているトークン
Token *token;

// program
Node *code[100];

// parse.c
// トークナイズ
void error(char *fmt, ...);
bool startwith(char *s1, char *s2);
Token *new_token(TokenKind kind, Token *cur, char *str);
Token *tokenize(char *p);

// 構文解析
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);
bool consume(char *op);
Token *consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();

Node *program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();

LVar *find_lvar(Token *tok);

// codegen.c
// コード生成
void gen_lval(Node *node);
void gen(Node *node);