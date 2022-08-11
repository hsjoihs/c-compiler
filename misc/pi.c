#include "std_io.h"
// Taken from https://xn--w6q13e505b.jp/program/spigot.html
int nume[52514];
int i;
int n;
int carry;
int digit;
int base;
int denom;
int first;
 
int main() {
  base = 10000;
  for (n = 52500; n > 0; n -= 14) {
    carry %= base;
    digit = carry;
    for (i = n - 1; i > 0; --i) {
      denom = 2 * i - 1;
      carry = carry * i + base * (first ? nume[i] : (base / 5));
      nume[i] = carry % denom;
      carry /= denom;
    }
    first = printf("%04d", digit + carry / base);
  }
  return 0;
}