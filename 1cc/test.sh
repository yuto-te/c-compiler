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
try 3 "a = 1; a + 2;"
try 14 "a = 3; b = 5 * 6 - 8; a + b / 2;"

echo OK