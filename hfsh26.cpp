//*********************************************************
//
// Griffin Kuchar
// Operating Systems
// Writing A Modern Unix Shell: hfsh26
//
//*********************************************************


//*********************************************************
//
// Includes and Defines
//
//*********************************************************
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sys/wait.h>
#include <fcntl.h>

#define STR_EXIT "exit"
#define PROMPT "hfsh26> "


//*********************************************************
//
// Extern Declarations
//
//*********************************************************
// Buffer state. This is used to parse string in memory...
// Leave this alone.
extern "C"{
    extern char **gettoks();
    typedef struct yy_buffer_state * YY_BUFFER_STATE;
    extern YY_BUFFER_STATE yy_scan_string(const char * str);
    extern YY_BUFFER_STATE yy_scan_buffer(char *, size_t);
    extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
    extern void yy_switch_to_buffer(YY_BUFFER_STATE buffer);
}

//*********************************************************
//
// Global variables
//
//*********************************************************
std::vector<std::string> searchPath{"/bin"};

//*********************************************************
//
// Function Prototypes
//
//*********************************************************
void invokeIntMode();
void invokeBatMode(std::string fileName);
void handleError();
void handleCd(char **toks);
int getLength(char **arr);

void processCmd(char **toks);
char** getToksArguments();

//*********************************************************
//
// Helper/Auxiliary Functions
//
//*********************************************************
int getLength(char **arr) { // return length of NULL terminating array
    if (arr == NULL) return 0;
    
    int len = 0;
    while (arr[len] != NULL) {
        len++;
    }
    return len;
}

void handleError() { // standard error message and procedure
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
}

char** getToksArguments(char* input) {
    /* local variables */
    char **toks;
    char linestr[256];
    YY_BUFFER_STATE buffer;

    /* initialize local variables */
    toks = NULL;

    // get arguments
    buffer = yy_scan_string(input);
    yy_switch_to_buffer(buffer);
    toks = gettoks();  // This gives you char**
    yy_delete_buffer(buffer);

    return toks;
}

void handleExit(char **toks) {
    if (getLength(toks) != 1) { // error if exit is invoked with any arguments
        handleError();
        return;
    }

    exit(0);
}

void handleCd(char **toks) {
    int argc = getLength(toks);
    if (argc != 2 || chdir(toks[1]) == -1) { // error if cd is invoked with more than 1 arg or if sys call fails
        handleError();
    }
}

void handlePath(char **toks) {
    searchPath.clear(); // clear old search path
    int argc = getLength(toks);
    int i;
    for(i = 1; i < argc; i++) {
       searchPath.push_back(toks[i]); // add all args to new search path
    }
}

int checkRedir(char **toks) { // returns output file index in toks if valid, -1 if invalid, and 0 if no ">" symbol in toks
    int i;
    int rval = -1;
    int argc = getLength(toks);

    for (i = 1; i < argc; i++) {
        if(!strcmp(toks[i], ">")) {
            rval = i;
            break;
        }
    }

    if (rval != -1 && (toks[rval + 1] == NULL || toks[rval + 2] != NULL)) {
        rval = -2;
    }

    return rval + 1;
}

void handleJob(char **toks) {
    int argc = getLength(toks);
    int i;
    std::string executable;
    int redir = checkRedir(toks); // check if for redirection in command
    if (redir == -1) { // redirection error
        handleError();
        return;
    }

    for (i = 0; i < searchPath.size(); i++) {
        executable = searchPath[i] + "/" + toks[0]; // build full executable from search path and command
        if (access(executable.c_str(), X_OK) == 0) { // if executable found
            pid_t pid = fork(); // create child process for valid executable
            if (pid < 0) { // fork() error handling
                handleError();
                return;
            }
            else if (pid == 0) {
                // if valid redirection
                if (redir > 0) {
                    int fd = open(toks[redir], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) {  // open() error handling
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    dup2(fd, STDERR_FILENO); 
                    close(fd); // redirect output/error to new file

                    toks[redir - 1] = NULL; // truncate ">" and output file from arguments
                }

                // child: execute executable
                execv(executable.c_str(), toks);

                exit(1); // error if exec() returns
            }
            else {
                wait(NULL); // parent: wait for child
            }

            return;
        }
    }
    handleError();
}

void processCmd(char **toks) {
    if(toks[0] != NULL){
        // handle built-in commands
        if(!strcmp(toks[0], STR_EXIT)) { // "exit"
            handleExit(toks);
        }
        else if(!strcmp(toks[0], "cd")) {
            handleCd(toks);
        }
        else if(!strcmp(toks[0], "path")) {
            handlePath(toks);
        }
        else { // handle non-built-in command
            handleJob(toks);
        }
    }
}

void invokeIntMode() {

    /* local variables */
    char linestr[256];

    printf(PROMPT); // print shell prompt for CLI
    /* main loop */
    while(fgets(linestr, 256, stdin)){ // Get entire argument line
        // make sure line has a '\n' at the end of it
        if(!strstr(linestr, "\n"))
            strcat(linestr, "\n");

        char **toks = getToksArguments(linestr); // format line into suitable toks array

        processCmd(toks); // Process command in form of "toks" array

        printf(PROMPT);
    }
}

void invokeBatMode(std::string fileName) {
    std::string line; // Stores the current command line

    // Open the file for reading
    std::ifstream myReadFile(fileName);

    // If opening file was successful
    if (myReadFile.is_open()) {
        // Read the file line by line
        while (std::getline(myReadFile, line)) {
            line += '\n';  // Always add newline since getline removes it

            // Convert c++ string into C char array for uniform command processing
            char charLine[256];
            strcpy(charLine, line.c_str());

            char **toks = getToksArguments(charLine); // format command line into suitable toks array
            processCmd(toks); // process the command
        }
        // Close the file
        myReadFile.close(); 
    }
    else {
        // Bad batch file
        handleError();
        exit(1);
    }
}

//*********************************************************
//
// Main Function
//
//*********************************************************
int main(int argc, char *argv[])
{
    if (argc == 1) { // no args for interactive mode, 1 file arg for batch mode, all else are errors
        invokeIntMode();
    }
    else if (argc == 2) {
        invokeBatMode(argv[1]);
    }
    else {
        handleError();  
        exit(1);
    }

    int retval;
    retval = 0;
    /* return to calling environment */
    return( retval );
}