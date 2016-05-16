/* 
 * File:   project1.c
 * Author: Claude Abounegm
 */

#include "project1.h"

int main(int argc, char** argv)
{
    // a circular array to keep the commands history data in it
    circular_array history;
    // we want to limit history to only 10 items, so here we intitialize
    // an array this big.
    circ_alloc(&history, HISTORY_LIMIT);

    // the args that the user will pass
    char* args[ARGS_SIZE];

    // flag to check if the parent should stop looping
    int should_run = 1;

    // specifies whether command being executed, was requested by the user
    // or by the program. Default is FALSE, as user inputs commands.
    char internal_exec = 0;

    // if the command ends with an ampersand (&), then we should not wait
    // for the child to terminate.
    char isConcurrent = 0;

    // the current raw command, as inputted by the user or by the application.
    char* rawCommand = NULL;

    while (should_run) // START: WHILE LOOP
    {
        fflush(stdout);

        // we check if we are executing an internal requested command
        if (!internal_exec)
        {
            isConcurrent = 0;
            printf("osh>");
            rawCommand = readLine();
        }
        else
        {
            // Note: we do not reset isConcurrent here, as this command
            // is valid: "!! &". It means that it is going to execute the
            // last command concurrently.
            printf("%s\n", rawCommand);
            internal_exec = 0;
        }

        // we keep a copy of the command's pointer, as tempStr might
        // be replaced by some internal commands as seen below.
        char* history_elem = rawCommand;

        // if there was an error parsing the input, fail.
        if (parseInputArgs(rawCommand, args, &isConcurrent))
            printf("The syntax of the command is incorrect.\n");

            // if array is not empty, process it
        else if (*args) // START: COMMAND EXECUTION
        {
            //printArgsArray(args);

            // the commands below are not added to history, purposefully.
            // To allow a command to be added to history, add its 
            // functionality to the "external commands" clause.

            if (!strcmp(args[0], EXIT_COMMAND)) // exit
                should_run = 0;

            else if (args[0][0] == '!') // command is either !N or !! 
            {
                int index = -1;

                if (args[0][1] == '!') // EXECUTE_LAST_COMMAND: !!
                {
                    if (history.count > 0)
                        index = (history.additions - 1);
                    else
                        printf("No commands in history.\n");
                }
                else // EXECUTE_N_COMMAND: !N
                {
                    int temp_index;
                    sscanf(args[0], EXECUTE_N_COMMAND, &temp_index);
                    if ((temp_index > 0)
                            && (history.additions < (temp_index + HISTORY_LIMIT))
                            && (temp_index <= history.additions))
                        index = (temp_index - 1);
                    else
                        printf("No such command in history.\n");
                }

                // we check if the index is valid, then a valid command was issued
                // execute it.
                if (index != -1)
                {
                    rawCommand = strdup(circ_getAbsoluteAt(&history, index));
                    internal_exec = 1;
                }
            }

            else if (!strcmp(args[0], HISTORY_COMMAND)) // history
                circ_foreach_reverse(&history, print_history_reverse);

            else // external commands
            {
                // push the command to history
                circ_push(&history, history_elem);

                // we add the implementation of some commands here as they need
                // to show up in history
                if (!strcmp(args[0], CD_COMMAND)) // cd
                {
                    // returns 0 when successful, -1 when fails
                    if (chdir(args[1]) == -1)
                        printf("%s\n", strerror(errno));
                }
                else if (!strcmp(args[0], PWD_COMMAND)) // pwd
                {
                    char cwd[1024];
                    printf("%s\n", (!getcwd(cwd, sizeof (cwd)) ? strerror(errno) : cwd));
                }
                else // external or unsupported command
                {
                    pid_t pid = fork();

                    if (pid == -1)
                    {
                        printf("An error has occurred while spawning a child process.Exiting.\n");
                        return 1;
                    }
                    else if (pid == 0)
                    {
                        fflush(stdout);
                        execvp(args[0], args);
                        printf("%s: %s\n", args[0], strerror(errno));
                        return 1;
                    }
                    else if (!isConcurrent)
                        wait(NULL);
                }

                // if we are executing concurrently, add a new line, just
                // for aesthetic purposes.
                if (!isConcurrent)
                    printf("\n");
            }

            // free the history element as it is not needed anymore
            free(history_elem);
            // free the *args allocated by parseInputArgs())
            free(*args);
        } // END: COMMAND EXECUTION
    } // END: WHILE LOOP

    // free the circular array
    circ_free(&history);

    return 0;
}

char* readLine()
{
    int i = 0; // index
    int c; // placeholder for the current character
    int capacity = 128; // the initial capacity of the string

    // we first allocate memory with an initial capacity
    char* str = malloc(capacity * CHAR_SIZE);

    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (c != '\b')
        {
            // ensure there is enough space for the string
            if (i == (capacity - 1))
                str = realloc(str, (capacity *= 2) * CHAR_SIZE);

            // set the character at the index
            str[i++] = c;
        }
    }
    // null-terminate the string
    str[i++] = '\0';

    // resize the array to fit exactly the string
    str = realloc(str, i * CHAR_SIZE);

    return str;
}

char parseInputArgs(char* rawString, char** args, char* isConcurrent)
{
    int i = 0, j = 0;

    char error = 0; // flag if a parsing error was found and should stop
    char stringModeOn = 0; // 0: false; '"' or '\'': true.

    // allocate enough memory for our new string
    char* str = malloc((strlen(rawString) + 1) * CHAR_SIZE);

    // we define a local variable, as we do not want to depend on the
    // external value of the isConcurrent ptr. This might result in wrong
    // behavior in the while() loop.
    int _isConcurrent = 0;

    // START PARSING
    args[j++] = &str[0]; // same as saying args[]=str;

    // loop while ampersand or error were found, and while rawString[0]
    // is not NUL (\0).
    while (!_isConcurrent && !error && rawString[0])
    {
        if (rawString[0] == '\\') // backslash `\`
        {
            // the next char after the escape character
            char nextChar = rawString[1];

            // we check if the next character is an escape for the opening
            // quote, then we actually print the quotes as is.
            // ex: "Hello \"world\"!", will be parsed as 'Hello "world"!'.
            if (nextChar == stringModeOn)
                str[i++] = rawString++[1];
            else if (stringModeOn)
            { // if we're between quotes, start copy as-is.
                str[i++] = rawString[0];
                str[i++] = rawString++[1];
            }
            // else, the backslash is ignored
        }
        else if (rawString[0] == '"' || rawString[0] == '\'') // " or '
        {
            if (rawString[0] == stringModeOn) // (opening quote == closing quote)
                stringModeOn = 0; // stop literal parsing

            else if (stringModeOn) // "hello 'world'" copied correctly
                str[i++] = rawString[0]; // copy the character as-is

            else // toggle string mode on
                // value as the ASCII code will allow us to keep track
                // of the opening quotation char, and still evaluate as true.
                stringModeOn = rawString[0];
        }
        else if (rawString[0] == '&') // &
        {
            if (stringModeOn) // if it's in string mode, copy as-is
                str[i++] = rawString[0];

            else // this command will be used to execute concurrently
                // parsing will be ignored after this character.
                _isConcurrent = 1;
        }
        else if (rawString[0] == ' ') // space
        {
            if (stringModeOn) // string mode, copy as-is
                str[i++] = rawString[0];

            else if (i != 0 && str[i - 1] != '\0')
            {
                str[i++] = '\0';
                args[j++] = &str[i]; // set the next 
            }
            // else, ignore whitespace
        }
        else
        {
            if (!stringModeOn && strchr(INVALID_COMMAND_CHARS, rawString[0]))
                error = 1;
            else if (isprint(rawString[0]))
                str[i++] = rawString[0];
        }

        rawString++;
    }
    // NUL-terminate the string
    str[i++] = '\0';

    if (error)
    {
        // we will not use str, so free it
        free(str);
        // set the args to be empty 
        args[0] = NULL;

        // return 1 if it fails 
        return 1;
    }

    // if the last string is empty, set it to NULL. Otherwise,
    // set the next one to NULL.
    args[!args[j - 1][0] ? (j - 1) : (j++)] = NULL;

    // set concurrent only if the passed variable was false
    if (!*isConcurrent)
        *isConcurrent = _isConcurrent;

    // success
    return 0;
}

void print_history_reverse(const circular_array* arr, int i, const char* str)
{
    printf("%zu %s\n", ((arr->additions - arr->count) + (i + 1)), str);
}

void printArgsArray(char* args[])
{
    int i;
    for (i = 0; args[i]; ++i)
        printf("%d: %s\n", i, args[i]);
    printf("size: %d\n", i);
}