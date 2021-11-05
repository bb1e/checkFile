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



void help();


int main(int argc, char *argv[]) {

    (void)argc;
    (void)argv; 

    struct gengetopt_args_info args;
	if (cmdline_parser(argc,argv,&args) != 0){
	exit(1);
	}

    int option = 0;

    if (args.file_given){

		option = 1;

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
            
            for(unsigned int i = 0; i < args.file_given; i++){
  
                FILE *file = fopen(args.file_arg[i], "r");
                if (file == NULL) 
                {
                    ERROR(1, "Error opening file '%s'\n", args.file_arg[i]);
                }

                const char ch = '.';
                char *ret;
                ret = strrchr(args.file_arg[i], ch);
                printf("String after |%c| is - |%s|\n", ch, ret);

                if((strcmp(ret, ".pdf") != 0) && (strcmp(ret, ".gif") != 0) && (strcmp(ret, ".jpg") != 0)
                    && (strcmp(ret, ".png") != 0) && (strcmp(ret, ".mp4") != 0) && (strcmp(ret, ".zip") != 0)
                    && (strcmp(ret, ".html") != 0)){

                    printf("Invalid file type\n");
                }else {
                    printf("Valid file type\n");
                    //callFile
                }


                fclose(file);

            }

            break;
        case 2: //batch op

            printf("op %d\n", option);

            char *line;
            size_t len = 0;
            FILE *f = fopen(args.batch_arg, "r");
            if (f == NULL) 
            {
                ERROR(1, "Error opening file '%s'\n", args.batch_arg);
            }


            while (getline (&line, &len, f) != -1){
                printf("%s", line);
                const char ch = '.';
                char *ret;
                ret = strrchr(line, ch);
                printf("String after |%c| is - |%s|\n", ch, ret);

                if((strcmp(ret, ".pdf") != 0) && (strcmp(ret, ".gif") != 0) && (strcmp(ret, ".jpg") != 0)
                    && (strcmp(ret, ".png") != 0) && (strcmp(ret, ".mp4") != 0) && (strcmp(ret, ".zip") != 0)
                    && (strcmp(ret, ".html") != 0)){

                    printf("Invalid file type\n");
                }else {
                    printf("Valid file type\n");
                    //callFile
                }
            }         

            
            free(line);
            fclose(f);
 
            break;
        case 3: //dir op

            printf("op %d\n", option);
            

            /*printf("op %d\n", option);

            DIR *opendir(const char *name);

            struct dirent *readdir(DIR *dirp);
            int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result);

            int closedir(DIR *dirp);*/

            struct dirent *dp;

            DIR *dir = opendir(args.dir_arg);
            if (!dir){ 
                ERROR(1,"Unable to open director"); 
            }

            while ((dp = readdir(dir)) != NULL){
                printf("%s\n", dp->d_name);
                const char ch = '.';
                char *ret;
                ret = strrchr(dp->d_name, ch);

                if((strcmp(ret, ".pdf") != 0) && (strcmp(ret, ".gif") != 0) && (strcmp(ret, ".jpg") != 0)
                    && (strcmp(ret, ".png") != 0) && (strcmp(ret, ".mp4") != 0) && (strcmp(ret, ".zip") != 0)
                    && (strcmp(ret, ".html") != 0)){

                    printf("Invalid file type\n");
                }else {
                    printf("Valid file type\n");
                    //callFile
                }
            }

            closedir(dir);



            /*

            while((df = readdir(dir))!=NULL){

                if{
                    continue;
                } //if it isn't a regular file, continue to the next record

                char* fileName = df->d_name; //file name

                //join dirPath and fileName together in a separate string
                char* filePath = malloc((strlen(dirPath)+strlen(fileName)+1)*sizeof(char));
                strcpy(filePath, dirPath);
                strcat(filePath, fileName);

                if(canOpenFile(filePath)){
                    verifyFile(filePath);
                }

            free(filePath);
            }

            */



            break;
        case 4: //help op

            help();

            break;
        default: 

            printf("error");

            break;
    }

    return 0;
}

//funcao para fork exec file
void callFile(){

    //execl( caminho_do_executavel , nome_do_comando, comando1, comando2, NULL )
    //execl("/bin/ls", "ls", "-la", NULL);
    //printf("File extension: %s \n",)

    //execlp("/bin/ls", "ls", "-la", NULL);

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


