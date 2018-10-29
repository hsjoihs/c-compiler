void debug_write(char *fmt, ...);

int main()
{
    debug_write("%s", "Hello, va_list!\n");
}