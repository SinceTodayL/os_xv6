#include<kernel/types.h>
#include<user/user.h>

int
main(int argc, char** argv){
    int p2c[2]; // parent to child
    int c2p[2]; // child to parent
    char buf[1];

    /*
        pipe(fds[2]):
        可以创建一个管道, 往 fds[1] 写, 在 fds[0] 读
    */
    pipe(p2c);
    pipe(c2p);
    
    /*
        fork(void):
        复制当前进程，创建一个新的子进程:
        返回值为 0，代表子进程中返回;
        返回值 >0，代表从父进程中返回，返回值是子进程的 pid;
        返回值 <0，创建失败
    */
    int pid = fork();
    
    if(pid < 0){
        fprintf(2, "fork failed\n");
        exit(1);
    }
    if(pid == 0){
        read(p2c[0], buf, 1);
         printf("%d: received ping\n", getpid());
        write(c2p[1], buf, 1);
        exit(0);
    } 
    else {
        write(p2c[1], "x", 1);
        read(c2p[0], buf, 1);
        printf("%d: received pong\n", getpid());
        wait(0);
        exit(0);
    }
}