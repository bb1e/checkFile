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

#include "debug.h"
#include "memory.h"
#include "args.h"

int main(int argc, char *argv[]) {

    (void)argc;
    (void)argv; 


    struct gengetopt_args_info args;
	if (cmdline_parser(argc,argv,&args) != 0){
	exit(1);
	}

    printf("%d\n", args.grp1_group_counter);
    //printf("%s", cmdline_parser_file_save);

    int option = 0;
    int fd;

    if (args.file_given){

		option = 1;

	} else if (args.batch_given){

		option = 2;
                  
	} else if (args.dir_given){
        option = 3;
    }

    printf("%d\n", option);

    switch (option){
        case 0: //help manual

            printf("op %d\n", option);

            break;
        case 1: //file op

            printf("op %d\n", option);

            break;
        case 2: //batch op


            fd = open (args.batch_arg, O_RDONLY);
		        if(fd == -1)
		        {
			        ERROR(1,"Can't open file '%s'\n", args.batch_arg);

		        }

            //ler linha a linha e guardar num malloc e dps correr num for para fazer fork exec file

            int close_fd = close(fd);
		        if (close_fd == -1)
		        {
			        ERROR(1, "Error closing file '%s'\n", args.batch_arg);
		        }

            /*FILE *f = NULL;
            char fname = args.batch_arg;
            f = fopen(fname, "r");
            if (f == NULL){
                ERROR(2, "error opening file");
            }*/
 
/*
            char *lineptr = NULL;
            size_t n = 0;
            ssize_t res;

            FILE *fptr = NULL;
            fptr = fopen(args.batch_arg, "r"); //nao funciona?????????? fazer da outra maneira

            if (fptr == NULL) {
                ERROR(1,"Error opening the file");
            }

            while ((res = getline(&lineptr, &n, fptr)) != -1) {
                printf("Encontrada linha com o tamanho: %zu :\n", res);
                printf("%s", lineptr);
            }

            free(lineptr);
            int fclose(FILE *fptr); */
            break;
        case 3: //dir op

            printf("op %d\n", option);

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

    //DIR
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
