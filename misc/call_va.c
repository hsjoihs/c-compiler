void debug_write(const char *fmt, ...);

int main()
{
    debug_write("%s", "Hello, va_list!\n");
    debug_write("%s", "Hello, va_list 2!\n");
    debug_write("%s %s %d%c\n", "Hello,", "va_list", 3, '!');
    debug_write("%s %s %f%c\n", "Hello,", "va_list", 4.0, '!');
    debug_write("%f %f %f %f %f %f %f %f%c\n", 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, '!');
    debug_write("%s%s%c %s%s%s%s %f%c\n", "He", "llo", ',', "va", "_", "li", "st", 13.0, '!');
}
