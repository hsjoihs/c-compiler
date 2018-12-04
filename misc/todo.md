# To do

## Features used within the code


## Features not used within the code

- defining `struct`s and `enum`s locally
- `goto`, `union`, `short`
- `enum Foo {Bar = 1, Baz};`
- `unsigned`, (`long long` or `uint64_t`) (required to correctly support `size_t`)
- `int array[] = {0, 1, 2, 3};`
- `for(...) ;`
- `typedef` (`stderr` is `FILE *`, though it seems to be `struct __sFILE *` or something internally. Also needed for `va_list`.)
- 7 or more arguments
- passing structs to a function
- function-style macros
- `#define` that expands to multiple tokens
