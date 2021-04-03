# My_Shell
This is a homework in OS course. However, it is an interesting homework.
## Problem & Corresponding Solution
### Problems
1. pipe
    - 當 shell 執行一次指令以後，child process 就會消失。因此如果要使用 `pipe()` 讓父子 process 溝通的話，需要在每一次產生新的 child process 之前先`pipe()`。
    <br>

    ```c=
    /*Example Code*/
    int fd[2];
    while (1) {
        pipe(fd);
        pid_t myPID = fork();
        ...
    }
    ```
    - `pipe()` 的使用方式：在使用 `pipe()` 之前通常會先宣告一個 `int array` 。這個 `int` array 通常會取名叫 `fd` 。`fd` 全名叫做 "file descriptor" ， 通常是一個可以容納兩個 element 的 `int array`。使用 `pipe(fd)` 以後，`fd[0]` 跟 `fd[1]` 分別用來讀取跟寫入，需要搭配 `read` 和 `write` 一起使用。 
 
        ![pipe 示意圖](https://i.imgur.com/68szpNZ.png)
        > pipe示意圖
    ```c=
    char buffer[BUFSIZ];
    int fd[2];
    pipe(fd);
    
    pid_t myPID = fork)();
    if (myPID == -1) {
        perror("fork");
        exit(-1);
    } else if ( myPID == 0) {
        //Child process 負責寫入，因此先關閉讀取的開口。
        close(fd[0]);
        
    } else {
        //Parent process 負責讀取，因此先關閉寫入的開口
        close(fd[1]);
    }
    ```
2. 



## Reference
### Read Input From Command Line
- [How to use argc & argv](https://blog.gtwang.org/programming/c-cpp-tutorial-argc-argv-read-command-line-arguments/)
- [pipe](https://docs.oracle.com/cd/E19683-01/806-4125/6jd7pe6bo/index.html)
- [dup2](https://www.man7.org/linux/man-pages/man2/dup.2.html)
- [execvp]()

### Prompt About
- [Get current directory](https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program)
- [Get current username](https://stackoverflow.com/questions/8953424/how-to-get-the-username-in-c-c-in-linux)