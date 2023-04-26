#include <cstdio>
#include "os_mem.h"
#include <cstdlib>
#include <cstring>
int destr = 0;
int creat = 0;
struct MemoryManager {
    int num_pages;
    int page_size;
    char* memory;
    int* page_table;
};

MemoryManager* mm;
//size: для страничных менеджеров - размер одной страницы памяти
// num_pages: для страничных менеджеров - количество физических страниц
// возвращаемое значение: 1 - успех, 0 - неудача

int create(int size, int num_pages)
{
    if(creat == 0)
    {
        mm = (MemoryManager *) malloc(sizeof(MemoryManager));
        if (size <= 0 || num_pages <= 0)
            return 0;
        mm->num_pages = num_pages;
        mm->page_size = size;
        mm->memory = (char *) malloc(num_pages * size);
        mm->page_table = (int *) malloc(num_pages * sizeof(int));
        if (!mm->memory || !mm->page_table)
            return 0;

        memset(mm->page_table, 0, num_pages * sizeof(int));
        creat = 1;
        destr = 0;
        return 1;
    }
    else
        return 0;
}
// записывает страницу физической памяти real_page в файл с именем page_<virtual_page>
// данные записываются в бинарном формате
// в случае успеха выводит на экран сообщение: save_page <real_page> <virtual_page>
// возвращаемое значение: 1 - успех, 0 - неудача (например, файл не открывается)
int save_page(int real_page, int virtual_page)
{
    if (real_page <= 0 || real_page >= mm->num_pages)
        return 0;


    char file_name[32] = "file1.txt";
    FILE* fp = fopen(file_name, "wb");

    sprintf(file_name, "page_%d", virtual_page);


    if (!fp)
        return 0;


    char* page_data = (mm->memory + (real_page * mm->page_size));
    size_t written = fwrite(page_data, 1, mm->page_size, fp);
    if (written != mm->page_size) {

        fclose(fp);
        return 0;
    }

    fclose(fp);
    printf("save_page %d %d\n", real_page, virtual_page);
    return 1;
}

// считывает из файла с именем page_<virtual_page> страницу памяти и загружает ее в физическую страницу real_page.
// данные считываются в бинарном формате
// если файл с таким именем отсутствует, физическая страница заполняется нулями.
// в случае успеха выводит на экран сообщение: load_page <virtual_page> <real_page>
// возвращаемое значение: 1 - успех (отсутствующий файл также является успехом), 0 - неудача (например номер страницы меньше нуля или больше общего числа страниц)
int load_page(int virtual_page, int real_page)
{
    if (real_page < 0 || real_page >= mm->num_pages)
        return 0;


    char file_name[32] = "file1.txt";
    FILE* fp = fopen(file_name, "rb");

    sprintf(file_name, "page_%d", virtual_page);


    if (!fp)
    {
        memset((mm->memory + (real_page * mm->page_size)), 0, mm->page_size);
        printf("load_page %d %d\n", virtual_page, real_page);
        return 1;
    }

    size_t read = fread((mm->memory + (real_page * mm->page_size)), 1, mm->page_size, fp);
    if (read != mm->page_size)
    {
        fclose(fp);
        return 0;
    }

    fclose(fp);
    printf("load_page %d %d\n", virtual_page, real_page);
    return 1;
}

// записывает по указанному виртуальному адресу addr указанное значение (байт) val
// в случае, если соответствующая виртуальная страница отсутствует в памяти, данные не записываются
// возвращаемое значение: 1 - успех, 2 - виртуальная страница отсутствует, 0 - неудача (например, адрес меньше 0)
int set_byte(int addr, char val)
{
    if (addr < 0)
        return 0;


    int page_num = addr / mm->page_size;
    int page_idx = mm->page_table[page_num];

    if (page_idx == 0)
        return 2;


    int offset = addr % (mm->page_size);
    *(mm->memory + (page_idx * (mm->page_size)) + offset) = val;
    return 1;
}
// считывает значение (байт) из ячейки виртуальной памяти, расположенной по указанному адресу addr, и кладет в буфер dst
// возвращаемое значение: 1 - успех, 2 - виртуальная страница отсутствует, 0 - неудача (например, адрес меньше 0)
int get_byte(int addr, char* dst) {
    if (addr < 0)
        return 0;


    int page_num = addr / mm->page_size;
    int page_idx = mm->page_table[page_num];

    if (page_idx == 0)
        return 2;

    int offset = addr % mm->page_size;
    *dst = *(mm->memory + (page_idx * mm->page_size) + offset);
    return 1;
}
// возвращает номер виртуальной страницы памяти, содержащей указанный блок
// возвращаемое значение: номер страницы в случае успеха, -1 в случае неудачи (если указанный блок целиком не содержится ни в одной странице)
int page_num(mem_handle_t block)
{
    int size = block.addr;
  //  int offset = mm->page_size  - block.size;
    //if(offset < 0)
      //  return -1;
    int page_idx = block.addr % mm->page_size;
    if(page_idx > mm->page_size)
        return -1;
    if(page_idx == 0 && block.size > mm->page_size)
        return -1;
    if((mm->page_size - page_idx >= page_idx) && mm->page_size)
    {
        int num = block.addr / mm->page_size;
        return num;
    }
    else
        return -1;
}
// удаление менеджера памяти
// возвращаемое значение: 1 - успех, 0 - неудача (например, менеждер не был создан)
int destroy()
{
    if(destr == 0)
    {
        free(mm->memory);
        free(mm->page_table);
        free(mm);
        destr = 1;
        creat = 0;
        return 1;
    }
    else
        return 0;

}
void setup_memory_manager(memory_manager_t* mm)
{
    mm -> create = create;
    mm -> destroy = destroy;
    mm -> page_num = page_num;
    mm -> get_byte = get_byte;
    mm -> set_byte = set_byte;
    mm -> save_page = save_page;
    mm -> load_page = load_page;
}

