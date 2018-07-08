#include <stdio.h>
#include <string.h>
 
#define bufSize 1024

const char* typelist[] = {
	"Token"
};

int main(int argc, char *argv[])
{
	if(argc < 3) {
		fprintf(stderr, "first argument: path of the template.\nsecond argument: path for output.\n");
		return 1;
	}
	int c;
	FILE *file;
	FILE *output;
	output = fopen(argv[2], "w");
	fprintf(output, "/* AUTOMATICALLY GENERATED. DO NOT EDIT. */\n");

	for(int i = 0; i < sizeof(typelist) / sizeof(typelist[0]); i++){
		file = fopen(argv[1], "r");
		if (file) {
			while ((c = getc(file)) != EOF){
				if(c == 'T') {
					fprintf(output, "%s", typelist[i]);
				} else {
					fputc(c, output);
				}
			}
			fclose(file);
		}
	}
}
