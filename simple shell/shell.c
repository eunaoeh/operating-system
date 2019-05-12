#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<errno.h>

#define MAX_SIZE 1000


void parseline(char* cmdline){
    char* cmd[MAX_SIZE];
    char result[MAX_SIZE];
    int cnt = 0;
    strcpy(result, cmdline);
    cmd[cnt] = strtok(result, " ");
    while(cmd[cnt] != NULL){
        cmd[++cnt] = strtok(NULL, " ");
    }
    execvp(cmd[0], cmd);
    perror("execvp failed\n");
}

int check_quit(char *cmdline){
    return (strcmp(cmdline, "quit"));
}

int main(int argc, char **argv){
    pid_t pid; //child id
    int len, tmp = 1;
    char *cmdline = NULL; //cmd line
    char buf[MAX_SIZE];//cmd line save

    //interactive mode
    if(argc == 1){
        while(tmp != 0){
            printf("prompt> ");
            fgets(buf, sizeof(buf), stdin);
            
            len = strlen(buf);
            if( len > 0 && buf[len-1] == '\n'){
                buf[len-1] = '\0';
            }

            cmdline = strtok(buf, ";");
            tmp = check_quit(cmdline);

            while(cmdline != NULL && tmp != 0){
                if((pid = fork()) < 0){
                    perror("fork");
                    exit(0);
                }
                else if(pid == 0){
                    parseline(cmdline);
                }
                cmdline = strtok(NULL, ";"); 
            }
            while((wait(NULL))>0);
        }
    }
    //batch file mode
    if(argc == 2){
        FILE *fp = fopen(argv[1], "r");
        if(fp == NULL){
            printf("File open error");
            exit(1);
        }
        while(tmp != 0 && NULL != fgets(buf, sizeof(buf), fp)){
            len = strlen(buf);       
            if(len > 0 && buf[len-1] == '\n'){
                buf[len-1] = '\0';
            }
            printf("%s\n", buf);
            cmdline = strtok(buf, ";");
            tmp = check_quit(cmdline);

            while(cmdline != NULL && tmp != 0){
                if((pid = fork()) < 0){
                    perror("fork error");
                    exit(0);
                }
                else if(pid == 0){
                    parseline(cmdline);
                }
            cmdline = strtok(NULL, ";");
            }
            while((wait(NULL))>0);
        }
        fclose(fp);       
    }
    else
        printf("Command Error\n");
    return 0;
}
