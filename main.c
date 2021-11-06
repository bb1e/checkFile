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
int isRegularFile(const char *fileName);
void checkFile(const char *fileName);


int main(int argc, char *argv[]) {

    (void)argc;
    (void)argv; 

    struct gengetopt_args_info args;
	if (cmdline_parser(argc,argv,&args) != 0){
	exit(1);
	}


    if (args.file_given){

        printf("\n[INFO] analyzing file/s\n");

        for(unsigned int i = 0; i < args.file_given; i++){

            if(args.file_arg[i] == NULL || (strcmp(args.file_arg[i], "..") == 0) || (strcmp(args.file_arg[i], ".") == 0)){
            
                    printf("[INFO] not a file\n");

                } else if(strchr(args.file_arg[i],'.') == NULL){

                    // file that doesn't have the extension in the name
                    printf("[INFO] not valid\n");

                } else{

                    FILE *file = fopen(args.file_arg[i], "r");
                    if (file == NULL) 
                    {
                        printf("[Error] cannot open file '%s' - No such file or directory\n", args.file_arg[i]);

                    }else{

                        checkFile(args.file_arg[i]);

                    }

                    fclose(file);
                }
		}
        printf("\n");

	} else if (args.batch_given){

        printf("\n[INFO] analyzing files listed in '%s'\n", args.batch_arg);

		FILE *file = fopen(args.batch_arg, "r");
            if (file == NULL) 
            {
                printf("[Error] cannot open file '%s' - No such file or directory\n", args.batch_arg);
            }

            char *line;
            size_t len = 0;
            while (getline (&line, &len, file) != -1){

                line[strcspn(line, "\n")] = 0; //end line with \0 instead of \n
                printf("%s\n", line);

                if(line == NULL || (strcmp(line, "..") == 0) || (strcmp(line, ".") == 0)){
            
                    printf("[INFO] not a file\n");

                } else if(strchr(line,'.') == NULL){

                    // file that doesn't have the extension in the name
                    printf("[INFO] not valid\n");

                } else{

                    FILE *f = fopen(line, "r");
                    if (f == NULL) 
                    {
                        printf("[Error] cannot open file '%s' - No such file or directory\n", line);

                    }else{

                        checkFile(line);

                    }
                }
            }

        fclose(file);
        printf("\n");
                  
	} else if (args.dir_given){

        printf("\n[INFO] analyzing files of directory '%s'", args.dir_arg);

        struct dirent *dp;

        DIR *dir = opendir(args.dir_arg);
        if (!dir){ 
            printf("[Error] cannot open dir '%s' - No such file or directory\n", args.dir_arg); 
            exit(1);
        }
        
        while ((dp = readdir(dir)) != NULL){

            char* fileName = dp->d_name;
            printf("%s\n", fileName);
            if(fileName == NULL || (strcmp(fileName, "..") == 0) || (strcmp(fileName, ".") == 0)){
            
                printf("[INFO] '%s' not a file\n", fileName);

             } else if(strchr(fileName,'.') == NULL){

                // file that doesn't have the extension in the name
                 printf("[INFO] not valid\n");

             } else{

                checkFile(fileName);

             }
        }

        closedir(dir);
        printf("\n");
    
        
    } else if (args.help_given){

        help();

    }

    return 0;
}


void help() {
    printf("\n***************************************************************************************\n");
    printf("\nCheckFile will check if the file extension is the real file type or not.\n");
    printf("File extensions supported by program: PDF, GIF, JPG, PNG, MP4, ZIP, HTML\n");
    printf("Command format: ./checkFile -op [argument]\n");
    printf("Program options:\n-f -> one or more files\n-b -> file with one or more file names/paths\n-d -> directory name/path\n-h -> help manual\n\n");
    printf("Author 1: Bruna Bastos LeaL -- 2201732\n");
    printf("Author 2: Tomás Vilhena Pereira -- 2201785\n");
    printf("\n***************************************************************************************\n");
    exit(0);
}

int isRegularFile(const char *ext){

    //verify extension
    if((strcmp(ext, "pdf") != 0) && (strcmp(ext, "gif") != 0) && (strcmp(ext, "jpg") != 0)
               && (strcmp(ext, "png") != 0) && (strcmp(ext, "mp4") != 0) && (strcmp(ext, "zip") != 0)
               && (strcmp(ext, "html") != 0)){

        return -1;

    }else {

        return 0;
    }

}



void checkFile(const char *fileName){

    //create temporary file to save the output of execl() 
    FILE* tmpFile = tmpfile(); 
    int fd = fileno(tmpFile); //get the file descriptor from an open stream


    pid_t pid = fork();
    if (pid < 0){

        ERROR(1, "[ERROR] failed to fork()");

    } else if(pid == 0){

        dup2(fd, 1); //redirect the output (stdout) to temporary file
        int execReturn = execl("/bin/file", "file", fileName, "--mime-type", "-b", NULL);
        if(execReturn == -1){
            ERROR(1, "[ERROR] failed to execl()");
        }

    } else{

        wait(NULL);

        const char ch = '.';
        char *ext;
        ext = strrchr(fileName, ch);
        ext++;
        char* fileExt = ext; //file extention after '.'

        // read til EOF and count nº of bytes
        //here we get the output of execl() and save it in string to use it to extract the real file extention
        fseek(tmpFile, 0, SEEK_END);
        long fsize = ftell(tmpFile)-1;
        fseek(tmpFile, 0, SEEK_SET); 
        char *str = malloc(fsize*sizeof(char)); 
        fread(str, 1, fsize, tmpFile);
        str[fsize] = 0; //Set the last '\n' byte to \0
        //this string contains the real file extension
        fclose(tmpFile);

        const char ch1 = '/';
        char *type;
        type = strrchr(str, ch1);
        type++;
        char* trueExt = type;

        //jpg apears as jpeg in exec the others still the same
        char* fullExt = fileExt;
        if(strcmp(fileExt, "jpg")==0){
            fullExt = "jpeg";
        }

        if(strcmp(type, "x-empty") == 0){
                
            printf("[INFO] '%s': is an empty file\n", fileName);

        }else if(isRegularFile(trueExt) == -1){

            printf("[INFO] '%s': type '%s' is not supported by checkFile\n", fileName, trueExt);

        } else{

            if(strcmp(trueExt, fullExt)==0){

                printf("[OK] '%s': extension '%s' matches file type '%s'\n", fileName, fileExt, trueExt);

            }else{

                 printf("[MISMATCH] '%s': extension is '%s' but file type is '%s'\n", fileName, fileExt, trueExt);
            }
        }
    }
}


