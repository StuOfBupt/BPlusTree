#include <time.h>
#include "BplusTree.h"
#include <unistd.h>
using namespace std;

int main() {
    auto pid = getpid();
    clock_t start_time = clock();
    int M = 5;
    unsigned int count = 0;
    BplusTree tree(M);

    //读取 dict.txt 的 keywords，插入到哈希表
    FILE *infile = fopen("../dict.txt", "rb");
    if(infile == NULL) {
        printf("Failed to open dict.txt\n");
        return -1;
    }
    while(!feof(infile)) {
        char buff[128];             //用于存储一个keyword
        int ch = fgetc(infile);     //读取一个字符
        int idx = 0;
        //逐个读取字符，直到行尾或文件尾
        while(ch != 0x0A && ch != -1) {
            buff[idx] = ch;
            ++idx;
            ch = fgetc(infile);
        }
        buff[idx] = '\0';           //一个keyword读取结束
        tree.Insert(buff);               //插入哈希表
    }
    fclose(infile);
    printf("Finished reading dict.txt\n");

    //读取 string.txt 的 keywords，查找哈希表，将查找到的 keywords 输出到 result.txt
    infile = fopen("../string.txt", "rb");
    FILE *outfile = fopen("../bupt_03_1.txt", "wb");
    if(infile == nullptr) {
        printf("Failed to open string.txt\n");
        return -1;
    }
    if(outfile == nullptr) {
        printf("Failed to open result.txt\n");
        return -1;
    }
    while(!feof(infile)) {
        char buff[128];
        int ch = fgetc(infile);     //用于存储一个keyword
        int idx = 0;                //读取一个字符
        //逐个读取字符，直到行尾或文件尾
        while(ch != 0x0A && ch != -1) {
            buff[idx] = ch;
            ++idx;
            ch = fgetc(infile);
        }
        buff[idx] = '\0';           //一个keyword读取结束
        if(tree.Find(buff)) {         //哈希表中查找keyword
            fputs(buff, outfile);
            fputs("\n", outfile);
            count ++;
        }
    }
    clock_t end_time = clock();
    printf("Finished reading string.txt\n");
    fclose(outfile);
    fclose(infile);

    printf("M = %d, string_match: %d, runtime: %lu ms\n", M, count, (end_time - start_time) * 1000/CLOCKS_PER_SEC);
    //tree.FormatPrint();

    sleep(3);
    return 0;
}

