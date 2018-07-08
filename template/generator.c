#include <stdio.h>
#include <string.h>
 
#define bufSize 1024

const char* typelist[] = {
	"Token"
};

void foo(FILE* output, const char* path, const char* replacer)
{
	int c;
	FILE *file;
	file = fopen(path, "r");
	if (file) {
		while ((c = getc(file)) != EOF){
			if(c == 'T') {
				fprintf(output, "%s", replacer);
			} else {
				fputc(c, output);
			}
		}
		fclose(file);
	}
}

int main(int argc, char *argv[])
{
	if(argc < 4) {
		fprintf(stderr, 
			"first argument: path of the template.\n"
			"second argument: path for output.\n"
			"third argument: path of the head.\n"
		);
		return 1;
	}
	
	FILE *output;
	output = fopen(argv[2], "w");
	fprintf(output, "/* AUTOMATICALLY GENERATED. DO NOT EDIT. */\n");

	foo(output, argv[3], "T");

	for(int i = 0; i < sizeof(typelist) / sizeof(typelist[0]); ++i){
		foo(output, argv[1], typelist[i]);
	}
}
