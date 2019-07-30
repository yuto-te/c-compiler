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

// 左辺と右辺を受け取る2項演算子
Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

// 数値
Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

// 次のトークンが期待している記号のときには，トークンを1つ読み進めて真を返す．
// それ以外の場合には偽を返す．
bool consume(char *op) {
  if (token->kind != TK_RESERVED || token->len != strlen(op) || memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

Token *consume_ident() {
  if (token->kind == TK_IDENT) {
    Token *tok = token;
    token = token->next;
    return tok;
  }
  return NULL;
}

// 次のトークンが期待している記号のときには，トークンを1つ読み進める．
// それ以外の場合にはエラーを報告する．
void expect(char *op) {
  if (token->kind != TK_RESERVED || token->len != strlen(op) || memcmp(token->str, op, token->len))
    error("%s: '%c'ではありません", token->str, op);
  token = token->next;
}

// 次のトークンが数値の場合，トークンを1つ読み進めてその数値を返す．
// それ以外の場合にはエラーを報告する．
int expect_number() {
  if (token->kind != TK_NUM) {
    error("数ではありません");
  }
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

// program = stmt*
void program() {
  int i = 0;
  while(!at_eof())
    code[i++] = stmt();
  code[i] = NULL;
}

// parse return
Node *parse_return() {
  Node *node;
  node = calloc(1, sizeof(Node));
  node->kind = ND_RETURN;
  token = token->next;
  node->lhs = expr();
  expect(";");
  return node;
}

// parse if
Node *parse_if() {
  Node *node;
  node = calloc(1, sizeof(Node));
  node->kind = ND_IF;
  token = token->next;
  expect("(");
  node->if_.test = expr();
  expect(")");
  node->if_.stmt = stmt();
  node->if_.else_stmt = NULL;
  if (token->kind == TK_ELSE) {
    node->if_.else_stmt = stmt();
  }
  return node;
}

// parse while
Node *parse_while() {
  Node *node;
  node = calloc(1, sizeof(Node));
  node->kind = ND_WHILE;
  token = token->next;
  expect("(");
  node->while_.test = expr();
  expect(")");
  node->while_.stmt = stmt();
  return node;
}

// parse for
Node *parse_for() {
  Node *node;
  node = calloc(1, sizeof(Node));
  node->kind = ND_FOR;
  token = token->next;
  expect("(");
  if (consume(";"))
    node->for_.init = NULL;
  else {
    node->for_.init = expr();
    expect(";");
    if (consume(";"))
      node->for_.test = NULL;
    else {
      node->for_.test = expr();
      expect(";");
      if (consume(";"))
        node->for_.update = NULL;
      else {
        node->for_.update = expr();
        expect(";");
      }
    }
  }
  expect(")");
  node->for_.stmt = stmt();
  return node;
}

// stmt    = expr ";"
//         | "if" "(" expr ")" stmt ("else" stmt)?
//         | "while" "(" expr ")" stmt
//         | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//         | "return" expr ";"
Node *stmt() {
  if (token->kind == TK_RETURN)
    return parse_return();

  if (token->kind == TK_IF)
    return parse_if();

  if (token->kind == TK_WHILE)
    return parse_while();

  if (token->kind == TK_FOR)
    return parse_for();

  Node *node = expr();
  expect(";");
  return node;
}

// expr = assign
Node *expr() {
  return assign();
}

// assign = equality ("=" assign)?
Node *assign() {
  Node *node = equality();
  if (consume("="))
    node = new_node(ND_ASSIGN, node, assign());
  return node;
}

// equality = relational ("==" relational | "!=" relational)*
Node *equality(){
  Node *node = relational();

  if (consume("=="))
    node = new_node(ND_EQ, node, relational());
  else if (consume("!="))
    node = new_node(ND_NEQ, node, relational());

  return node;
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational(){
  Node *node = add();

  if (consume("<="))
    node = new_node(ND_ELT, node, add());
  else if (consume("<"))
    node = new_node(ND_LT, node, add());
  // 両辺を入れ替えることで>と>=を<と<=に変換する
  else if (consume(">="))
    node = new_node(ND_ELT, add(), node);
  else if (consume(">"))
    node = new_node(ND_LT, add(), node);

  return node;
}

// add = mul ("+" mul | "-" mul)*
Node *add(){
  Node *node = mul();

  for (;;) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

// mul  = unary ("*" unary | "/" unary)*
Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

// unary = ("+" | "-")? term
Node *unary(){
  if (consume("+"))
    return term();
  else if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), term());
  else
    return term();
}

// term = num | ident | "(" expr ")"
Node *term() {
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(tok);
    if (lvar)
      node->offset = lvar->offset;
    else {
      lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->name = tok->str;
      lvar->len = tok->len;
      // 一つ目の変数はオフセットを直接指定する
      if (!locals)
        lvar->offset = 8;
      else
        lvar->offset = locals->offset + 8;
      node->offset = lvar->offset;
      locals = lvar;
    }
    return node;
  }

  // そうでなければ数値のはず
  return new_node_num(expect_number());
}

// 変数を名前で検索する。見つからなかった場合はNULLを返す。
LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next)
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
      return var;
  return NULL;
}
