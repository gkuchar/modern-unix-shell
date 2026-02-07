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
int getLength(char **arr) {
    if (arr == NULL) return 0;
    
    int len = 0;
    while (arr[len] != NULL) {
        len++;
    }
    return len;
}

void handleError() {
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

void handleCd(char **toks) {
    int argc = getLength(toks);
    if (argc != 2 || chdir(toks[1]) == -1) {
        handleError();
    }
}

void processCmd(char **toks) {
    int ii = 0;

    if(toks[0] != NULL){
        // /* simple loop to echo all arguments */
        // for(ii=0; toks[ii] != NULL; ii++){
        //     printf( "Argument %d: %s\n", ii, toks[ii] );
        // }
        
        if(!strcmp(toks[0], STR_EXIT)) {
            exit(0);
        }
        else if(!strcmp(toks[0], "cd")) {
            handleCd(toks);
        }
    }
}

void invokeIntMode() {

    /* local variables */
    char linestr[256];

    printf(PROMPT);
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
            processCmd(toks);
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
    if (argc == 1) {
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