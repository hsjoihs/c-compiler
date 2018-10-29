void debug_write(const char *fmt, ...);

int main()
{
    debug_write("%s", "Hello, va_list!\n");
    debug_write("%s", "Hello, va_list 2!\n");
    debug_write("%s %s %d%c\n", "Hello,", "va_list", 3, '!');
    debug_write("%s %s %f%c\n", "Hello,", "va_list", 4.0, '!');
}
