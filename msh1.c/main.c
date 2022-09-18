/*
    Name: Shivam Patel
    ID: 1001707265
*/
// The MIT License (MIT)
//
// Copyright (c) 2016 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports four arguments

pid_t pid;
// signal handler for ctrl+
static void handle_signal (int sig )
{
  printf ("Caught signal %d\n", sig );
  kill(pid, SIGKILL);
}

int main()
{

  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );
  char history[15][MAX_COMMAND_SIZE];
  int history_index = 0;
  int i;
  int pid_index = 0;
  pid_t showpids[15];
  char cmd[20];
  int z;

  struct sigaction act;
  memset (&act, '\0', sizeof(act));
  act.sa_handler = &handle_signal;
  if (sigaction(SIGINT , &act, NULL) < 0)
  {
    perror ("sigaction: ");
    return 1;
  }


  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );

    // we can access (!number) command from history with maximum of 15 possible
    while (command_string[0] == '!')
      {
          strcpy(cmd,command_string);
          z = atoi(strtok(&cmd[1]," "));
          if(history_index < z)
          {
              printf("%c: Invalid option \n",command_string[0]);
              printf("Try 'ls --help' for more information \n");
              continue;
          }
          strcpy(command_string,history[z]);
      }
    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;
                                                           
    char *working_string  = strdup( command_string );

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr = working_string;

    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_string, WHITESPACE ) ) != NULL) &&
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your shell functionality
    int index  = 0;
    int pid_index = 0;
    pid_t child_pid;
    int status;
    int cdir;
    char source[30];
    // if users something we compare it and go accordingly
    if (token[0]!=NULL)
    {
        // if we type exit or quit its exit the shell
        if ((strcmp(token[0],"exit")==0)||(strcmp(token[0],"quit")==0))
        {
          exit(EXIT_SUCCESS);
        }
        // if we type cd it would change the directory using chdir function
        else if (strcmp(token[0],"cd")==0)
        {
          cdir = chdir(token[1]);
          if(cdir != 0)
          {
            printf("Directory does not exist: %s\n",token[1]);
          }
        }
        // we can type history in our shell and it would return last 15 commands used
        else if(strcmp(token[0],"history")==0)
        {
          for(i = 0; i < history_index; i++)
            printf("%d: %s", i, history[i]);
        }
        // we can type showpids and it would list the process value in the shell
        else if(strcmp(token[0],"showpids")==0)
        {
          for(i = 0;i < pid_index; i++)
          {
            printf("%d: %i\n", i, showpids[i]);
          }
        }
    }
    // fork the process
    pid = fork();
    child_pid = pid;
    // here if it is 0, meaning child process it would enter the if condition
    if (child_pid == 0)
    {
        strcpy(source,"./");
        strcat(source,token[0]);
        execl(source, token[0], token[1], token[2], token[3], token[4],  NULL);
            
        strcpy(source,"/usr/local/bin/");
        strcat(source,token[0]);
        execl(source, token[0], token[1], token[2], token[3], token[4], NULL);
                
        strcpy(source,"/usr/bin/");
        strcat(source,token[0]);
        execl(source, token[0], token[1], token[2], token[3], token[4], NULL);
                
        strcpy(source,"/bin/");
        strcat(source,token[0]);
        execl(source, token[0], token[1], token[2], token[3], token[4],  NULL);

        printf("%s : Command not found.\n\n",token[0]);
        exit(0);
        
    }
    // parent waits
    wait(&status);
    // place the values to the history array
    if(strcmp(command_string,"")!= 0)
    {
        if(history_index == 15)
        {
            for(i = 1; i < 16; i++)
            {
                strcpy(history[i-1],history[i]);
            }
        }
        // if there is just one command we copy from command_string to the history array
        else
        strcpy(history[history_index],command_string);
        history_index++;
    }
    free( head_ptr );
  }
  return 0;
  // e2520ca2-76f3-11ec-90d6-0242ac120003
}
