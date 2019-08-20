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
  TK_IF,       // if
  TK_ELSE,     // else
  TK_WHILE,    // while
  TK_FOR,      // for
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
  ND_IF,     // if
  ND_ELSE,   // else
  ND_WHILE,  // while
  ND_FOR,    // for
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
  struct {
    Node *test;      // test expression inside the parenthesis ()
    Node *stmt;      // statements inside the body of if
    Node *else_stmt; // optional else statement
  } if_;
  struct {
    Node *test;     // test expression
    Node *stmt;     // statement inside the body of loop
  } while_;
  struct {
    Node *init;     // initialization statement
    Node *test;     // test expression
    Node *update;   // update statement
    Node *stmt;     // statement inside the body of loop
  } for_;
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

void program();
Node *parse_return();
Node *parse_if();
Node *parse_while();
Node *parse_for();
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