#include "simplefs.h"

command builtin_cmd[] = {
        {"cd",     &my_cd, "change working dierctory"},
        {"mkdir",  &my_mkdir, "make directories"},
        {"rmdir",  &my_rmdir, "remove empty directories"},
        {"ls",     &my_ls, "list directory contents\n flags:\n -l\t use a long listing format"},
        {"create", &my_create, "create new file"},
        {"rm",     &my_rm, "remove files\n flags:\n -r remove directories"},
        {"write",  &my_write, "write to file\n flags:\n -a append content to file\n -c cover content in specific position to file \n -w rewrite file content"},
        {"read",   &my_read, "read from file"},
        {"exit",   &my_exitsys, "exit system"},
        {"open",   &my_open, "open a file\n flags:\n -l show all file in openfile_list"},
        {"close",  &my_close, "close a file\n flags:\n -a close all opened file"},
        {"pwd",    &my_pwd, "print name of current/working directory"},
        {"help",   &help, "show command help"}
};

/**
 * 显示帮助
 * @param args "command" 命令名
 * @return 1
 */
int help(char **args) {
    if (args[1] == NULL) {
        for (int i = 0; i < sizeof(builtin_cmd) / sizeof(command); i++) {
            printf("%-10s - %s\n", builtin_cmd[i].name, builtin_cmd[i].desc);
        }
        return 1;
    } else {
        for (int i = 0; i < sizeof(builtin_cmd) / sizeof(command); i++) {
            // 匹配内置命令
            if (strcmp(args[1], builtin_cmd[i].name) == 0) {
                printf("%-10s - %s\n", builtin_cmd[i].name, builtin_cmd[i].desc);
                return 1;
            } else {
                printf("wrong command\n");
                return 1;
            }
        }
    }
}

/**
 * 读取用户输入内容
 * @return 当前行内容
 */
char *read_line(void) {
    char *line = NULL;
    ssize_t bufsize = 0;
    getline(&line, &bufsize, stdin);
    return line;
}

/**
 * 分割用户输入的命令和参数
 * @param line 用户输入的一行内容
 * @return 命令及参数列表
 */
char **split_line(char *line) {
    int bufsize = TOK_BUFSIZE, position = 0;
    // 分配内存
    char **tokens = malloc(64 * sizeof(char *));
    char *token;
    // 分配失败则退出
    if (!tokens) {
        exit(EXIT_FAILURE);
    }
    // 分割字符串
    token = strtok(line, TOK_DELIM);
    // token 不为空时将其存入 tokens
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += TOK_BUFSIZE;
            // 重新分配内存空间
            tokens = realloc(tokens, bufsize * sizeof(char *));
            // 分配失败则退出
            if (!tokens) {
                exit(EXIT_FAILURE);
            }
        }
        // 继续分割字符串
        token = strtok(NULL, TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

/**
 * 执行内置命令
 * @param args 内置命令的参数列表
 * @return 1为继续运行，0为停止运行
 */
int execute(char **args) {
    if (args[0] == NULL) {
        return 1;
    }

    for (int i = 0; i < sizeof(builtin_cmd) / sizeof(command); i++) {
        // 匹配内置命令
        if (strcmp(args[0], builtin_cmd[i].name) == 0) {
            // 执行并返回状态码
            return (*builtin_cmd[i].func)(args);
        }
    }
}

int main(void) {
    char **args;
    int status;
    char *line;
    startsys();
    do {
        printf("%s> ", current_dir);
        line = read_line();
        args = split_line(line);
        status = execute(args);
        free(line);
        free(args);
    } while (status);
}
