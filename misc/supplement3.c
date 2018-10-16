int add_3(int a){return 3 + a;}

void *return_fp(void){return add_3;}

int call_fp(void* q){int (*p)(int) = q;return (*p)(171);}
