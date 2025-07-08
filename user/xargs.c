#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int
main(int argc, char *argv[])
{
    char buf[512];
    char *p;
    int i;

    char *xargv[MAXARG];
    for (i = 1; i < argc; i++) {
        xargv[i - 1] = argv[i];
    }
    int prefix_len = argc - 1;

    // 读取标准输入，一行一行处理
    p = buf;
    while (1) {
        char c;
        int n = read(0, &c, 1); // 从 stdin 读一个字符
        if (n <= 0) break;

        if (c == '\n') {
            *p = '\0';
            // 构造参数列表
            xargv[prefix_len] = buf;
            xargv[prefix_len + 1] = 0;

            if (fork() == 0) {
                exec(argv[1], xargv);
                fprintf(2, "exec failed\n");
                exit(1);
            } else {
                wait(0);
            }
            p = buf; // 重置指针准备读下一行
        } else {
            *p++ = c;
        }
    }

    exit(0);
}