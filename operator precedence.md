## full list

| precedence | operator | fixity |
 --- | --- | ---
| 0 | `() [] . -> ++ --` | unary postfix |
| -1 | `! ~ ++ -- + - * & sizeof` | unary prefix |
| -2 | `(type name)` | unary, right |
| -3 | `* / %` | binary, left |
| -4 | `+ -` | binary, left |
| -5 | `<< >>` | binary, left |
| -6 | `< <= > >=` | binary, left |
| -7 | `== !=` | binary, left |
| -8 | `&` | binary, left |
| -9 | `|` | binary, left|
| -10 | `&&` | binary, left |
| -11 | `||` | binary, left |
| -12 | `?:` | ternary, right |
| -13 | `= += -= *= /= %= &= ^= |= <<= >>=` | binary, right |
|-14| `,` | binary, left |


## currently implementable

| precedence | operator | fixity |
 --- | --- | ---
| -1 | `! ~ ` | unary prefix |
| -3 | `* / %` | binary, left |
| -4 | `+ -` | binary, left |
| -5 | `<< >>` | binary, left |
| -6 | `< <= > >=` | binary, left |
| -7 | `== !=` | binary, left |
| -8 | `&` | binary, left |
| -9 | `|` | binary, left|
| -10 | `&&` | binary, left |
| -11 | `||` | binary, left |
|-14| `,` | binary, left |
