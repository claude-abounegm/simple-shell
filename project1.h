/* 
 * File:   project1.h
 * Author: Claude Abounegm
 */

#ifndef PROJECT1_H
#define PROJECT1_H

#include <stdio.h> // printf, sscanf
#include <sys/unistd.h> // fork, execvp
#include <sys/wait.h> // wait
#include <sys/types.h> // pid_t
#include <stdlib.h> // malloc, realloc, free
#include <string.h> // strdup, strerror
#include <errno.h> // errno
#include <unistd.h> // chdir, getcwd
#include <ctype.h>

#include "circular_array.h"

#define MAX_LINE 80
#define ARGS_SIZE MAX_LINE/2+1
#define HISTORY_LIMIT 10
#define EXIT_COMMAND "exit"

#define HISTORY_COMMAND "history"
#define EXECUTE_N_COMMAND "!%d"
#define EXECUTE_LAST_COMMAND "!!"
#define CD_COMMAND "cd"
#define PWD_COMMAND "pwd"
#define INVALID_COMMAND_CHARS "|;<>(){}"
#define CHAR_SIZE sizeof(char)

char* readLine();
char parseInputArgs(char* rawString, char** args, char* isConcurrent);
void print_history_reverse(const circular_array* arr, int i, const char* str);
void printArgsArray(char* args[]);

#endif /* PROJECT1_H */

