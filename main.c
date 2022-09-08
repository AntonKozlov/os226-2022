#include <stdio.h>
#include <string.h>

#define INPUT_SIZE 2048
int last_ret_code = 0;

int echo(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        printf("%s%c", argv[i], i == argc - 1 ? '\n' : ' ');
    }
    return argc - 1;
}

int retcode(int argc, char *argv[]) {
    printf("%d\n", last_ret_code);
    return 0;
}


void exec(char* input){
    char *args[128];
    int arg_ptr = 0;
    args[arg_ptr++] = strtok(input, " ");
    while (args[arg_ptr - 1] != NULL)
        args[arg_ptr++] = strtok(NULL, " ");
    if(arg_ptr < 2)
        return;
    if(strcmp(args[0], "echo") == 0)
        last_ret_code = echo(arg_ptr - 1, args);
    if(strcmp(args[0], "retcode") == 0)
        last_ret_code = retcode(arg_ptr - 1, args);
}

void parse(char* input){
    char current_command[1024];
    int input_ptr = 0, cmd_ptr = 0;
    char c;
    for(;;){
        c = input[input_ptr++];
        // Accepted letters are eng alphabet and digits
        if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c >= '0' && c <= '9' || c == ' ')
            current_command[cmd_ptr++] = c;

        if (c == '\n' || c == ';' || c == '\0'){
            current_command[cmd_ptr++] = '\0';
            exec(current_command);
            cmd_ptr = 0;
        }

        if (c == '\0') break;
    }
}

int main(int argc, char *argv[]) {
    char input_buffer[INPUT_SIZE];
    while(fgets(input_buffer, INPUT_SIZE, stdin) != NULL) {
        parse(input_buffer);
    }

    return 0;
}
