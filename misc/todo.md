# To do

## Features used within the code

- `int i = 0; `
- `for(int i = 0; ...)`
- `const`, `_Noreturn`
- `enum Foo {Bar = 1, Baz};`
- `typedef` (`stderr` is `FILE *`, though internally it seems to be `struct __sFILE *` internally)
- `extern` (`stderr`)
- `"foo" "bar"`
- escape sequence
- `#include`, `#define`, `#ifdef`
- pointer comparison
- `int f(struct A a)`
- full function prototype

## Features not used within the code

- defining `struct`s and `enum`s locally
- `goto`, `union`, `short`, `long long`
- `unsigned` (ignoring `calloc`)
- `int array[] = {0, 1, 2, 3};`
- `for(...) ;`
