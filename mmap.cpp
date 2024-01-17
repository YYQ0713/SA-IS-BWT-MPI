#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <omp.h>

#define MiB * (1024LL * 1024)

int main() {
    // 打开文件
    int fd = open("test_100M.txt", O_RDWR);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    // 获取文件大小
    off_t f_size = lseek(fd, 0, SEEK_END);

    // 映射文件
    void *file_data = mmap(NULL, f_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (file_data == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return 1;
    }\
    int t_size;

#pragma omp parallel num_threads(8)
{
    t_size = omp_get_num_threads();
    
    
    for (int i = omp_get_thread_num(); i < f_size; i += t_size) {
        *(uint8_t *)(file_data + i) = 'P';
    }
}
    // 修改第1GB位置的字符为Y
    //*(uint8_t *)(file_data + 1) = 'P';

    // 修改第18GB位置的字符为Y
    //*(uint8_t *)(file_data + 880 MiB) = 'O';

    // 输出修改后的位置
    //printf("Character at 1GB position: %c\n", *(unsigned char *)(file_data + 1));
    //printf("Character at 18GB position: %c\n", *(uint8_t *)(file_data + 280 GiB));

    // 解除文件映射
    if (munmap(file_data, f_size) == -1) {
        perror("Error unmapping file");
    }

    // 关闭文件
    close(fd);

    printf("File modification complete.\n");

    return 0;
}

