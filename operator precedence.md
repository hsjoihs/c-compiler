## full list

| precedence | operator | fixity | finished? |
 --- | --- | --- | ---
| 0 | `() [] . -> ++ --` | unary postfix | no |
| -1 | `! ~ ++ -- + - * & sizeof` | unary prefix | no (`!` and `~` finished) |
| -2 | `(type name)` | unary, right | no |
| -3 | `* / %` | binary, left | yes |
| -4 | `+ -` | binary, left | yes |
| -5 | `<< >>` | binary, left | yes |
| -6 | `< <= > >=` | binary, left | yes |
| -7 | `== !=` | binary, left | yes |
| -8 | `&` | binary, left | yes |
| -9 | `|` | binary, left| yes |
| -10 | `&&` | binary, left | no |
| -11 | `||` | binary, left | no |
| -12 | `?:` | ternary, right | no |
| -13 | `= += -= *= /= %= &= ^= |= <<= >>=` | binary, right | no |
| -14 | `,` | binary, left | yes |


## currently implementable

| precedence | operator | fixity | finished? |
 --- | --- | --- | ---
| -1 | `! ~ ` | unary prefix | yes |
| -3 | `* / %` | binary, left | yes |
| -4 | `+ -` | binary, left | yes |
| -5 | `<< >>` | binary, left | yes |
| -6 | `< <= > >=` | binary, left | yes |
| -7 | `== !=` | binary, left | yes |
| -8 | `&` | binary, left | yes |
| -9 | `|` | binary, left| yes |
| -14 | `,` | binary, left | yes |
