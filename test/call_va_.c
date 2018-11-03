void debug_write(const char *fmt, ...);

int main()
{
	debug_write("Hello, va_list%c\n", '!');
	debug_write("%s", "Hello, va_list!\n");
    debug_write("%s", "Hello, va_list 2!\n");
    debug_write("%s %s %d%c\n", "Hello,", "va_list", 3, '!');
}
