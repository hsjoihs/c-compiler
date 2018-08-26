# To do

## Features used within the code

- `_Noreturn`
- `enum Foo {Bar = 1, Baz};`
- `typedef` (`stderr` is `FILE *`, though it seems to be `struct __sFILE *` internally)
- `extern` (`stderr`)
- `#include`, `#define`, `#ifdef`

## Features not used within the code

- defining `struct`s and `enum`s locally
- `goto`, `union`, `short`, `long long`
- `unsigned` (ignoring `calloc`)
- `int array[] = {0, 1, 2, 3};`
- `for(...) ;`
