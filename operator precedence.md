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

| precedence | operator | fixity | finished? |
 --- | --- | --- | ---
| -1 | `! ~ ` | unary prefix | no |
| -3 | `* / %` | binary, left | yes |
| -4 | `+ -` | binary, left | yes |
| -5 | `<< >>` | binary, left | yes |
| -6 | `< <= > >=` | binary, left | yes |
| -7 | `== !=` | binary, left | no |
| -8 | `&` | binary, left | no |
| -9 | `|` | binary, left| no |
| -10 | `&&` | binary, left | no |
| -11 | `||` | binary, left | no |
|-14| `,` | binary, left | yes |
