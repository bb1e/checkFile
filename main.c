/**
 * @file main.c
 * @date 2021-10-01
 * @author Bruna Leal 2201732
 * @author Tomás Pereira 2201785
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <stdint.h>
#include <dirent.h>
#include <signal.h>
#include <ctype.h>
#include <time.h>
#include <libgen.h>

#include "debug.h"
#include "memory.h"
#include "args.h"


#define MAX 100

void trata_sinal(int signal);
void help();


int main(int argc, char *argv[]) {

    char buf[MAX];
    struct sigaction act;
    ssize_t n;

    (void)argc;
    (void)argv; 

    act.sa_handler = trata_sinal;

    sigemptyset(&act.sa_mask);

    act.sa_flags = SA_RESTART;


    if (sigaction(SIGUSR1, &act, NULL) < 0)
    {
        ERROR(1, "sigaction - SIGUSR1");
    }

    printf("PID do processo: %d\n", getpid());
    printf("Mata o processo!!!:\n");

    n = read(0, buf, MAX);
    if (n < 0){
        ERROR(2, "Erro de entrada.");
    buf[n] = '\0';
    }
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        printf("%d.%02d.%02d_%02dh%02d:%02d  --- %d\%c", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,getpid(), argc);

    struct gengetopt_args_info args;
	if (cmdline_parser(argc,argv,&args) != 0){
	exit(1);
	}

    //printf("%d\n", args.grp1_group_counter);
    //printf("%s", cmdline_parser_file_save);

    int option = 0;

    if (args.file_given){

		option = 1;
        printf("%s \n", args.file_arg);

	} else if (args.batch_given){

		option = 2;
                  
	} else if (args.dir_given){

        option = 3;
        
    } else if (args.help_given){

        option = 4;

    }



    switch (option){
        case 1: //file op

            printf("op %d\n", option);

            const char *fileName = args.file_arg;
            FILE *file = fopen(fileName, "r");
            if (file == NULL) 
            {
                ERROR(1, "Error opening file '%s'\n", args.batch_arg);
            }

            fclose(file);

            break;
        case 2: //batch op

            //const char *fileName = args.batch_arg;
            fileName = args.batch_arg;
            FILE *f = fopen(fileName, "r");
            if (f == NULL) 
            {
                ERROR(1, "Error opening file '%s'\n", args.batch_arg);
            }

            char *line = NULL;
            size_t len = 0;
 
            while(getline(&line, &len, f) != -1) {
                printf("line length: %zd\n", strlen(line));
            }

            //ler linha a linha e guardar num malloc e dps correr num for para fazer fork exec file

            
            free(line);
            fclose(f);
 
            break;
        case 3: //dir op

            printf("op %d\n", option);

            DIR *opendir(const char *name);

            struct dirent *readdir(DIR *dirp);
            int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result);
            int closedir(DIR *dirp);

            break;
        case 4: //help op

            help();

            break;
        default: 

            printf("error");

            break;
    }

    // FILE
    /*
    correr os argumentos tds num for
    fork exec file -> strtok
    */

    // BATCH
    /*
    fopen file
    meter td num vetor dinamico
    correr os elementos num for
    fork exec file -> strtok
    */

    // DIR
    /*
    percorrer o diretorio sem os subdiretorios
    verificar tds os seus ficheiros
    fork exec file -> strtok
    */

    return 0;
}

//funcao para fork exec file
void callFile(){

    //execl( caminho_do_executavel , nome_do_comando, comando1, comando2, NULL )
    ///execl("/bin/ls", "ls", "-la", NULL);
    //printf("File extension: %s \n",)

}

void help() {
    printf("CheckFile will check if the file extension is the real file type or not\n");
    printf("Command format: ./checkFile -op [argument]\n");
    printf("Program options:\n-f -> one or more files\n-b -> file with one or more file names/paths\n-d -> directory name/path\n-h -> help manual\n");
    printf("Author 1: Bruna Bastos LeaL -- 2201732\n");
    printf("Author 2: Tomás Vilhena Pereira -- 2201785\n");
    printf("File extensions compatible with the program: \nPDF, GIF, JPG, PNG, MP4, ZIP, HTML\n");
    exit(0);
}


void trata_sinal(int signal) {
    int aux;
    aux = errno;

    /* Restaura valor da variavel global errno */
    errno = aux;
}