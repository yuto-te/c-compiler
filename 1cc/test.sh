#!/bin/bash
try() {
  expected="$1"
  input="$2"

  ./1cc "$input" > tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

try 42 "42;"
try 21 '5+20-4;'
try 41 " 12 + 34 - 5 ;"
try 47 "5+6*7;"
try 15 "5*(9-6);"
try 4 "(3+5)/2;"
try 15 "-5*+3+30;"
try 1 "(1+3)==2*2;"
try 0 "3+5 == 1+3;"
try 0 "(1+3)!=2*2;"
try 1 "3+5 != 1+3;"
try 1 "3 < 5-1;"
try 0 "3<5-2;"
try 1 "3<=5-2;"
try 1 "4+2 > 5;"
try 0 "4+2 > 2*3;"
try 1 "4+2>=2*3;"
try 1 "(3-1==4/2) != (10/5 > 1*2);"
try 3 "fo0 = 1; fo0 + 2;"
try 14 "foo = 3; _bar = 5 * 6 - 8; foo + _bar / 2;"
try 14 "areturn = 3; b = 5 * 6 - 8; return areturn + b / 2;"
try 5 "return 5; return 6;"
try 2 "a = 1; if (a == 1) return 2;"
try 4 "a = 1; if (a != 1) return 3; else return 4;"
try 16 "i=1; while(i<10) i=i+i; i;"
echo OK