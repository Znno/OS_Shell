#include <stdbool.h>
#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the string by space and returns the array of tokens
*
*/
void free_mem(char** tokens) {
    for(int i=0;tokens[i]!=NULL;i++){
        free(tokens[i]);
    }
    free(tokens);
}
char **tokenize(char *line)
{
    char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
    char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
    int i, tokenIndex = 0, tokenNo = 0;

    for(i =0; i < strlen(line); i++){

        char readChar = line[i];

        if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
            token[tokenIndex] = '\0';
            if (tokenIndex != 0){
                tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
                strcpy(tokens[tokenNo++], token);
                tokenIndex = 0;
            }
        } else {
            token[tokenIndex++] = readChar;
        }
    }

    free(token);
    tokens[tokenNo] = NULL ;
    return tokens;
}
void reap_background_processes() {
    pid_t pid;

    while ((pid = waitpid(-1, NULL, WNOHANG)) >0) {
        printf("Shell: Background process PID %d finished\n", pid);
    }
}
long long todelete[1000];
int deletenum=0;
int foregroundpid=0;
bool stop=false;
void Handler(int sig) {
    if(deletenum) {
        for(int i=0;i<deletenum;i++) {
            printf("Process %lld finished\n", todelete[i]);
            kill(todelete[i],SIGKILL);
        }
        deletenum=0;
    }
    if(foregroundpid) {
        stop=true;
        printf("Process %d finished\n",foregroundpid);
        kill(foregroundpid, SIGKILL);
    }



}
void process(int tokencounter,int singleand,char** tokens) {
    char* temp[tokencounter];
    for(int j=0;j<tokencounter;j++)
        temp[j]=tokens[j];
    temp[tokencounter-singleand]=NULL;
    if(temp[0]!=NULL&&temp[0][0]=='c'&&temp[0][1]=='d') {
        int ret=chdir(temp[1]);
        if(ret==-1) {
            perror("chdir");

        }
        return;
    }

    int pid =fork();
    if(pid == 0) {
        if(singleand)
            signal(SIGINT,SIG_IGN);
        int ret=execvp(temp[0], temp);
        if(ret==-1) {
            perror("execvp");
            exit(1);
        }
        exit(0);
    }
    else  if(pid > 0) {
        if(!singleand) {
            foregroundpid=pid;
            wait(NULL);
            foregroundpid=0;
        }

    }
    else {
        printf("Error forking\n");
        exit(0);
    }
}
void process_tripleand( char **tokens) {
    int prev=0;
    int tokenscounter=0;
    for(int i=0;tokens[i]!=NULL;i++) {
        tokenscounter++;
        if(stop)
            return;
        if(tokens[i][0]=='&') {

            char* temp[i-prev+1];
            for(int j=prev;j<i;j++)
                temp[j-prev]=tokens[j];
            temp[i-prev]=NULL;
            if(temp[0]!=NULL&&temp[0][0]=='c'&&temp[0][1]=='d') {
                int ret=chdir(temp[1]);
                if(ret==-1) {
                    perror("chdir");

                }
                prev=i+1;
                continue;
            }
            int pid =fork();
            todelete[deletenum++]=pid;
            if(pid == 0) {
                int ret=execvp(temp[0], temp);
                if(ret==-1) {
                    perror("execvp");
                    exit(1);
                }
                exit(0);
            }

            prev=i+1;
        }
    }
    if(stop)
        return;


    char* temp[tokenscounter-prev+1];
    for(int j=prev;j<tokenscounter;j++)
        temp[j-prev]=tokens[j];
    temp[tokenscounter-prev]=NULL;
    if(temp[0]!=NULL&&temp[0][0]=='c'&&temp[0][1]=='d') {
        int ret=chdir(temp[1]);
        if(ret==-1) {
            perror("chdir");

        }
        free_mem(tokens);
        return;
    }
    int pid =fork();
    todelete[deletenum++]=pid;
    if(pid == 0) {

        int ret=execvp(temp[0], temp);
        if(ret==-1) {
            perror("execvp");
            exit(1);
        }
        exit(0);
    }

    while ((waitpid(-1, NULL, 0)) >0) {

    }
}
void process_doubleand( char **tokens) {
    //printf("testtoo\n");
    int prev=0;
    int tokenscounter=0;
    for(int i=0;tokens[i]!=NULL;i++) {
        tokenscounter++;
        if(stop)
            return;
        if(tokens[i][0]=='&') {

            char* temp[i-prev+1];
            for(int j=prev;j<i;j++)
                temp[j-prev]=tokens[j];
            temp[i-prev]=NULL;
            if(temp[0]!=NULL&&temp[0][0]=='c'&&temp[0][1]=='d') {
                int ret=chdir(temp[1]);
                if(ret==-1) {
                    perror("chdir");

                }
                prev=i+1;
                continue;
            }
            int pid =fork();
            if(pid == 0) {

                int ret=execvp(temp[0], temp);
                if(ret==-1) {
                    perror("execvp");
                    exit(1);
                }
                exit(0);
            }
            else  if(pid > 0) {

                    foregroundpid=pid;
                    wait(NULL);

            }
            prev=i+1;
        }
    }
    if(stop)
        return;


        char* temp[tokenscounter-prev+1];
        for(int j=prev;j<tokenscounter;j++)
            temp[j-prev]=tokens[j];
        temp[tokenscounter-prev]=NULL;
        if(temp[0]!=NULL&&temp[0][0]=='c'&&temp[0][1]=='d') {
            int ret=chdir(temp[1]);
            if(ret==-1) {
                perror("chdir");

            }
            free_mem(tokens);
            return;
        }
        int pid =fork();
        if(pid == 0) {

            int ret=execvp(temp[0], temp);
            if(ret==-1) {
                perror("execvp");
                exit(1);
            }
            exit(0);
        }
        else  if(pid > 0) {

            foregroundpid=pid;
            wait(NULL);


    }
}
long long PIDS[1000];
int cnt=0;

int main(int argc, char* argv[]) {
    signal(SIGINT,Handler);
    char  line[MAX_INPUT_SIZE];
    char  **tokens;
    int i;

    while(1) {
        foregroundpid = 0;
        /* BEGIN: TAKING INPUT */
        bzero(line, sizeof(line));
        printf("$ ");
        scanf("%[^\n]", line);
        getchar();

        //printf("Command entered: %s (remove this debug output later)\n", line);
        /* END: TAKING INPUT */

        line[strlen(line)] = '\n'; //terminate with new line
        tokens = tokenize(line);

        //do whatever you want with the commands, here we just print them

        // for(i=0;tokens[i]!=NULL;i++){
        //     printf("found token %s (remove this debug output later)\n", tokens[i]);
        // }
        if (tokens[0] == NULL) {
            free_mem(tokens);
            continue;
        }
        PIDS[cnt++]=getpid();

        if(tokens[0][0]=='e'&&tokens[0][1]=='x'&&tokens[0][2]=='i'&&tokens[0][3]=='t') {
            for(i=cnt-1;i>=0;i--) {
                kill(PIDS[i],SIGKILL);
            }
            cnt=0;
        }
        reap_background_processes();
        bool singleand=false;
        bool doubleand=false;
        bool tripleand=false;
        int tokencounter=0;
        for(i=0;tokens[i]!=NULL;i++){
            if(stop)
                break;
            tokencounter++;
            if(tokens[i][0]=='&'&&tokens[i][1]==NULL) {
                singleand=true,tokens[i]=NULL;
                break;
            }
            else if(tokens[i][0]=='&'&& tokens[i][1]!=NULL&&tokens[i][1]=='&'&& tokens[i][2]!=NULL&&tokens[i][2]=='&') {
                tripleand=true;
            }
            else if(tokens[i][0]=='&'&& tokens[i][1]!=NULL&&tokens[i][1]=='&') {
                doubleand=true;
            }

        }
        if(doubleand)
            process_doubleand(tokens);
        else if(tripleand) {
            process_tripleand(tokens);

        }
        else {
            process( tokencounter,singleand,tokens);
        }
        // Freeing the allocated memory
        free_mem(tokens);
        stop=false;

    }
    return 0;
}