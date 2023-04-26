#include "os_mem.h"
#include <cstdio>


int main()
{
    memory_manager_t mm;
    setup_memory_manager(&mm);

    char* res = (char*)calloc(20, sizeof(char));

    printf("create %d\n", mm.create(32, 100));
    printf("page_num %d\n", mm.page_num(mem_handle_t(0, 32)));
    printf("page_num %d\n", mm.page_num(mem_handle_t(0, 64)));
    printf("page_num %d\n", mm.page_num(mem_handle_t(40, 10)));
    printf("set_byte %d\n", mm.set_byte(0, 'a'));
    printf("get_byte %d\n", mm.get_byte(0, res));
}