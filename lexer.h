#define GARBAGE_INT 0xCCCCCCCC
#define BOOL int

enum TokenKind {
	OP_PLUS,
	OP_MINUS,
	END,
	LIT_DEC_INTEGER
};

struct Token{
	enum TokenKind kind;
	int int_value;
};

