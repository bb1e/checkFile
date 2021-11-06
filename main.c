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


//functions prototypes
void help();
int isSupportedFile(const char *fileName);
void checkFile(const char *fileName, int *v);
void signals(int sig, siginfo_t *siginfo, void *context);

char fileExec[255]; //global variable to save file name needed for signal message


int main(int argc, char *argv[]) {

    (void)argc;
    (void)argv; 

    struct gengetopt_args_info args;
	if (cmdline_parser(argc,argv,&args) != 0){
	exit(1);
	}


    //signals
	struct sigaction act;

    //define signals answer
    act.sa_sigaction = signals;
    //mask without signals -- dont block signals
    sigemptyset(&act.sa_mask);

    act.sa_flags = SA_SIGINFO;  //info about signal
    act.sa_flags |= SA_RESTART; //retrieves blocking calls

    //get signal SIGUSR1
    if (sigaction(SIGUSR1, &act, NULL) < 0){
        ERROR(1, "sigaction - SIGUSR1");
	}
    if (sigaction(SIGQUIT, &act, NULL) < 0){
        ERROR(1, "sigaction - SIGQUIT");
    }


    //array to save summary counters
    int summary[4] = {0};

    if (args.file_given){

        signal(SIGUSR1, SIG_IGN); //ignore SIGUSR1

        printf("\n[INFO] analyzing file/s\n");

        for(unsigned int i = 0; i < args.file_given; i++){

            if(args.file_arg[i] == NULL || (strcmp(args.file_arg[i], "..") == 0) || (strcmp(args.file_arg[i], ".") == 0)){
            
                printf("[INFO] '%s' is not a file\n", args.file_arg[i]);

            } else if(strchr(args.file_arg[i],'.') == NULL){

                // file that doesn't have the extension in the name
                printf("[INFO] '%s' is not valid\n", args.file_arg[i]);

            } else{

                FILE *file = fopen(args.file_arg[i], "r");
                if (file == NULL) 
                {
                    printf("[Error] cannot open file '%s' - No such file or directory\n", args.file_arg[i]);
                    continue;

                }else{

                    checkFile(args.file_arg[i], summary);

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
                printf("[Error] cannot open file '%s' - No such file or directory\n\n", args.batch_arg);
                exit(1);
            }

            // read the batch file given line by line and analyse each file
            char *line;
            size_t len = 0;
            while (getline (&line, &len, file) != -1){

                line[strcspn(line, "\n")] = 0; //end line with \0 instead of \n

                if(line == NULL || (strcmp(line, "..") == 0) || (strcmp(line, ".") == 0)){
            
                    printf("[INFO] '%s' is not a file\n", line);

                } else if(strchr(line,'.') == NULL){

                    // file that doesn't have the extension in the name
                    printf("[INFO] '%s' is not valid\n", line);

                } else{

                    FILE *f = fopen(line, "r");
                    if (f == NULL) 
                    {
                        printf("[Error] cannot open file '%s' - No such file or directory\n", line);
                        summary[0]++;

                    }else{

                        checkFile(line, summary);
                        summary[1]++;
                        fclose(f);

                    }
                    
                }
            }

        fclose(file);
        printf("[SUMMARY] files analyzed: %d; files OK: %d; mismatch: %d; errors: %d\n", summary[1], summary[2], summary[3], summary[0]);
        printf("\n");
                  
	} else if (args.dir_given){

        signal(SIGUSR1, SIG_IGN); //ignore SIGUSR1 

        printf("\n[INFO] analyzing files of directory '%s'\n", args.dir_arg);

        struct dirent *dp;

        DIR *dir = opendir(args.dir_arg);
        if (!dir){ 
            printf("[ERROR] cannot open dir '%s' - No such file or directory\n\n", args.dir_arg); 
            exit(1);
        }
        
        while ((dp = readdir(dir)) != NULL){

            char* fileName = dp->d_name;
            if(fileName == NULL || (strcmp(fileName, "..") == 0) || (strcmp(fileName, ".") == 0)){
            
                printf("[INFO] '%s' is not a file\n", fileName);

             } else if(strchr(fileName,'.') == NULL){

                // file that doesn't have the extension in the name
                 printf("[INFO] '%s' is not valid\n", fileName);

             } else{

                FILE *f = fopen(fileName, "r");
                if (f == NULL) 
                {
                    printf("[Error] cannot open file '%s' - permission denied\n", fileName);
                    summary[0]++;

                } else{

                    checkFile(fileName, summary);
                    summary[1]++;

                }
                fclose(f);
             }
        }

        closedir(dir);
        printf("[SUMMARY] files analyzed: %d; files OK: %d; mismatch: %d; errors: %d\n", summary[1], summary[2], summary[3], summary[0]);
        printf("\n");
    
        
    } else if (args.help_given){

        signal(SIGUSR1, SIG_IGN); //ignore SIGUSR1

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

int isSupportedFile(const char *ext){

    //verify extension
    if((strcmp(ext, "pdf") != 0) && (strcmp(ext, "gif") != 0) && (strcmp(ext, "jpeg") != 0)
               && (strcmp(ext, "png") != 0) && (strcmp(ext, "mp4") != 0) && (strcmp(ext, "zip") != 0)
               && (strcmp(ext, "html") != 0)){

        return -1;

    }else {

        return 0;
    }

}



void checkFile(const char *fileName, int *v){

    strcpy(fileExec, fileName); //copy fileName to fileExec to sow the file name in signal message

    //create temporary file to save the output of execl() 
    FILE* auxFile = tmpfile();
    //tmpfile() creates a temporary file that is deleted when fclose() 
    int fd = fileno(auxFile); //get the file descriptor from an open stream


    pid_t pid = fork();
    if (pid < 0){

        ERROR(1, "[ERROR] failed to fork()");
        v[0]++;

    } else if(pid == 0){
        //child

        dup2(fd, 1); //redirect the output (stdout) to temporary file
        int execReturn = execl("/bin/file", "file", fileName, "--mime-type", "-b", NULL);
        //--mime-type -> print only the specified elements
        // -b -> do not prepend filenames to output lines
        if(execReturn == -1){
            ERROR(1, "[ERROR] failed to execl()");
            v[0]++;
        }

    } else{
        //parent

        wait(NULL); //wait for any child to terminate

        //get file extention
        const char ch = '.';
        char *ext;
        ext = strrchr(fileName, ch);
        ext++;
        char* fileExt = ext; //file extention after '.'

        // read til EOF and count nº of bytes
        //here we get the output of execl() and save it in string to use it to extract the real file extention
        fseek(auxFile, 0, SEEK_END);
        // ftell() returns the current file position
        long fsize = ftell(auxFile)-1;
        fseek(auxFile, 0, SEEK_SET);
        //dynamic memory to save the string we want 
        char *str = malloc(fsize*sizeof(char)); 
        fread(str, 1, fsize, auxFile);
        str[fsize] = 0; //Set the last '\n' byte to \0
        //this string contains the real file extension
        fclose(auxFile);

        //get real file extention type
        const char ch1 = '/';
        char *type;
        type = strrchr(str, ch1);
        type++;
        char* trueExt = type; //file extention after '/'

        //jpg apears as jpeg in exec the others still the same
        char* fullExt = fileExt;
        if(strcmp(fileExt, "jpg")==0){
            fullExt = "jpeg";
        }

        if(strcmp(type, "x-empty") == 0){
                
            printf("[INFO] '%s': is an empty file\n", fileName);

        }else if(isSupportedFile(trueExt) == -1){

            printf("[INFO] '%s': type '%s' is not supported by checkFile\n", fileName, trueExt);

        } else{

            if(strcmp(trueExt, fullExt)==0){

                printf("[OK] '%s': extension '%s' matches file type '%s'\n", fileName, fileExt, trueExt);
                v[2]++;

            }else{

                 printf("[MISMATCH] '%s': extension is '%s' but file type is '%s'\n", fileName, fileExt, trueExt);
                 v[3]++;
            }
        }
    }
}


void signals(int sig, siginfo_t *siginfo, void *context) {
    (void)context;
    (void)siginfo;
    //copy errno global variable
    int aux = errno;

    if (sig == SIGUSR1) {
       time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        printf("%d.%02d.%02d_%02dh%02d:%02d  --- %s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, fileExec);
    }
    if(sig== SIGQUIT){
		 printf("Captured SIGQUIT signal (sent by PID: %d ). Use SIGINT to terminate application.\n", getpid());
		}
	
    //restores value of the global variable errno
    errno = aux;
  
}
