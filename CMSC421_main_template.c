/*
project: 01
author: Quinten Ballard
email: uw94057
student id: uw94057@umbc.edu
due date: 3/2/25 11:59:59 pm
description: a simple linux shell designed to perform basic linux commands
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include "utils.h"

/*
In this project, you are going to implement a number of functions to 
create a simple linux shell interface to perform basic linux commands
*/

/* 
DEFINE THE FUNCTION PROTOTYPES
user_prompt_loop()
get_user_command()
parse_command()
execute_command()
*/

//Given Function Prototypes
void user_prompt_loop();
char* get_user_command();
char** parse_command(const char *userInput);
void execute_command(char** parsedUserInput);

//Helper Function Prototpyes
void handle_proc_command(char **parsedUserInput);
void log_command(const char* userInput);
void retrieve_commands();

int main(int argc, char **argv)
{
    /*
    Write the main function that checks the number of argument passed to ensure 
    no command-line arguments are passed; if the number of argument is greater 
    than 1 then exit the shell with a message to stderr and return value of 1
    otherwise call the user_prompt_loop() function to get user input repeatedly 
    until the user enters the "exit" command.
    */

    /*
    ENTER YOUR CODE HERE
    */

  if (argc > 1){
    fprintf(stderr, "Error");
    return 1;
  }

  user_prompt_loop();
  return 0;
  
}

/*
user_prompt_loop():
Get the user input using a loop until the user exits, prompting the user for a command.
Gets command and sends it to a parser, then compares the first element to the two
different commands ("/proc", and "exit"). If it's none of the commands, 
send it to the execute_command() function. If the user decides to exit, then exit 0 or exit 
with the user given value. 
*/

void user_prompt_loop()
{
    // initialize variables

    /*
    loop:
        1. prompt the user to type command by printing >>
        2. get the user input using get_user_command() function 
        3. parse the user input using parse_command() function 
        Example: 
            user input: "ls -la"
            parsed output: ["ls", "-la", NULL]
        4. compare the first element of the parsed output to "/proc"and "exit"
        5. if the first element is "/proc" then you have the open the /proc file system 
           to read from it
            i) concat the full command:
                Ex: user input >>/proc /process_id_no/status
                    concated output: /proc/process_id_no/status
            ii) read from the file line by line. you may user fopen() and getline() functions
            iii) display the following information according to the user input from /proc
                a) Get the cpu information if the input is /proc/cpuinfo
                - Cpu Mhz
                - Cache size
                - Cpu cores
                - Address sizes
                b) Get the number of currently running processes from /proc/loadavg
                c) Get how many seconds your box has been up, and how many seconds it has been idle from /proc/uptime
                d) Get the following information from /proc/process_id_no/status
                - the vm size of the virtual memory allocated the vbox 
                - the most memory used vmpeak 
                - the process state
                - the parent pid
                - the number of threads
                - number of voluntary context switches
                - number of involuntary context switches
                e) display the list of environment variables from /proc/process_id_no/environ
                f) display the performance information if the user input is /proc/process_id_no/sched
        6. if the first element is "exit" the use the exit() function to terminate the program
        7. otherwise pass the parsed command to execute_command() function 
        8. free the allocated memory using the free() function
    */

    /*
    Functions you may need: 
        get_user_command(), parse_command(), execute_command(), strcmp(), strcat(), 
        strlen(), strncmp(), fopen(), fclose(), getline(), isdigit(), atoi(), fgetc(), 
        or any other useful functions
    */

    /*
    ENTER YOUR CODE HERE
    */

  //Declared Variables
  char* userInput = NULL;
  char** parsedUserInput = NULL;

  //loop user prompt
  while (1) {
    userInput = get_user_command(); //retrieve user input

    //handle false userinput
    if (!userInput) {
      printf("Exiting Shell\n");
      break;
    }

    log_command(userInput);
    
    parsedUserInput = parse_command(userInput); // parse userinput
    free(userInput); //free userinput
    
    if (!parsedUserInput) continue;
    
    //handle exit command and deallocation
    if (strcmp(parsedUserInput[0], "exit") == 0) {
      int exit_code = 0;
      if (parsedUserInput[1]) exit_code = atoi(parsedUserInput[1]);
      
      for (int i = 0; parsedUserInput[i]; i++) {
	free(parsedUserInput[i]);
      }
      free(parsedUserInput);
      exit(exit_code);
    }

    //handle history command
    if (strcmp(parsedUserInput[0], "history") == 0) retrieve_commands();
    //handle proc command
    else if (strcmp(parsedUserInput[0], "/proc") == 0) handle_proc_command(parsedUserInput);
    else execute_command(parsedUserInput);
    
    //execute_command(parsedUserInput);

    //deallocation
    for (int i = 0; parsedUserInput[i]; i++) {
      free(parsedUserInput[i]);
    }
    free(parsedUserInput);
  }
}

/*
get_user_command():
Take input of arbitrary size from the user and return to the user_prompt_loop()
*/

char* get_user_command()
{
  /*
    Functions you may need: 
    malloc(), realloc(), getline(), fgetc(), or any other similar functions
  */
  
  /*
    ENTER YOUR CODE HERE
  */
  
  //Declare Variables
  char* input = NULL;
  size_t size = 0;
  ssize_t length;

  printf(">> ");
  length = getline(&input, &size, stdin); //retrieve input

  //deallocate and return NULL if getline returns error
  if (length == -1){
    free(input);
    return NULL;
  }


  //removes newline
  if(input[length - 1] == '\n'){
    input[length - 1] = '\0';
  }
  
  
  return input;
}

/*
parse_command():
Take command grabbed from the user and parse appropriately.
Example: 
    user input: "ls -la"
    parsed output: ["ls", "-la", NULL]
Example: 
    user input: "echo     hello                     world  "
    parsed output: ["echo", "hello", "world", NULL]
*/

char** parse_command(const char *userInput)
{
  /*
    Functions you may need:
    malloc(), realloc(), free(), strlen(), first_unquoted_space(), unescape()
  */
  
  /*
    ENTER YOUR CODE HERE
  */

  //Handles empty user input
  if (userInput == NULL) {
    perror("Invalid Argument: NULL input");
    return NULL;
  }
  
  // Ignore leading spaces
  while (*userInput && isspace(*userInput)) {
    userInput++;
  }

  //handles invalid userinput
  if (*userInput == '\0') {
    perror("Invalid Argument: Empty input");
    return NULL;
  }
  
  // Unescape the input
  char *unescapedInput = unescape(userInput, stderr);
  if (!unescapedInput) {
    perror("Failed to retrieve escaped sequence");
    return NULL;
  }
  
  // Count spaces to determine the number of arguments
  int spacesCounted = count_spaces(unescapedInput);
  char **parsedUserInput = malloc((spacesCounted + 2) * sizeof(char *));
  if (!parsedUserInput) {
    perror("Failed to allocate space");
    free(unescapedInput);
    return NULL;
  }
  
  //Declare variables
  int i = 0;
  char *saveptr;

  // store first word as token
  char *token = strtok_r(unescapedInput, " ", &saveptr);

  //Loop through tokens
  while (token) {
    parsedUserInput[i] = strdup(token);
    if (!parsedUserInput[i]) {
      perror("Memory allocation failed");
      
      // Free all previously allocated memory
      for (int j = 0; j < i; j++) {
	free(parsedUserInput[j]);
      }
      free(parsedUserInput);
      free(unescapedInput);
      return NULL;
    }
    i++; //Increment
    token = strtok_r(NULL, " ", &saveptr); // Store next token
  }
  
  parsedUserInput[i] = NULL; // Null-terminate for execvp
  free(unescapedInput);
  return parsedUserInput;
}

/*
execute_command():
Execute the parsed command if the commands are neither /proc nor exit;
fork a process and execute the parsed command inside the child process
*/

void execute_command(char** parsedUserInput)
{
    /*
    Functions you may need: 
        fork(), execvp(), waitpid(), and any other useful function
    */

    /*
    ENTER YOUR CODE HERE
    */

  //Handles invalid input
  if (!parsedUserInput || !parsedUserInput[0]) return;

  pid_t pid = fork(); //Create process
  if (pid == -1) {
    perror("Failed to Fork");
    return;
  }

  
  if (pid == 0){
    if (execvp(parsedUserInput[0], parsedUserInput) == -1){ //if execvp returns -1 execution failed
      perror("Execution failed");

      //Deallocation
      for (int i = 0; parsedUserInput[i]; i++){
	free(parsedUserInput[i]);
      }

      free(parsedUserInput);
      
      exit(EXIT_FAILURE);
      free(parsedUserInput);
    }
  }
  else waitpid(pid, NULL, 0);
  
}

//Helper Functions

// Function to handle /proc queries
void handle_proc_command(char **parsedUserInput) {
  // Handle invalid input
  if (!parsedUserInput[1]) {
      printf("Usage: /proc <filename> or /proc <pid> <subfile>\n");
      return;
  }

  char filepath[256];

  // Construct the correct `/proc` file path
  if (parsedUserInput[2]) {
      snprintf(filepath, sizeof(filepath) - 1, "/proc/%s/%s", parsedUserInput[1], parsedUserInput[2]);
  } else {
      snprintf(filepath, sizeof(filepath) - 1, "/proc/%s", parsedUserInput[1]);
  }
  filepath[sizeof(filepath) - 1] = '\0';  // Ensure null termination

  FILE *file = fopen(filepath, "r"); // Open the `/proc` file
  if (!file) {
      perror("Error opening /proc file");
      return;
  }

  char *line = NULL;
  size_t len = 0;

  // Handle different `/proc` queries
  if (strcmp(parsedUserInput[1], "cpuinfo") == 0) {
      // Extract CPU information
      while (getline(&line, &len, file) != -1) {
          if (strstr(line, "model name") || strstr(line, "cpu MHz") ||
              strstr(line, "cache size") || strstr(line, "cpu cores")) {
              printf("%s", line);
          }
          free(line);
          line = NULL;
      }
  } 
  else if (strcmp(parsedUserInput[1], "loadavg") == 0) {
      // Display system load average
      if (getline(&line, &len, file) != -1) {
          printf("System Load: %s\n", line);
          free(line);
      }
  } 
  else if (parsedUserInput[2] && strcmp(parsedUserInput[2], "status") == 0) {
      // Retrieve specific process status details
      while (getline(&line, &len, file) != -1) {
          if (strstr(line, "VmSize") || strstr(line, "VmPeak") ||
              strstr(line, "State") || strstr(line, "PPid") ||
              strstr(line, "Threads")) {
              printf("%s", line);
          }
          free(line);
          line = NULL;
      }
  } 
  else {
      // Print the entire file for general cases
      while (getline(&line, &len, file) != -1) {
          printf("%s", line);
          free(line);
          line = NULL;
      }
  }

  fclose(file);
}

void log_command(const char* userInput) {
  FILE *file = fopen(".421sh", "a"); // Open the history file in append mode

  if (!file) {
      perror("Error: Couldn't open .421sh file.\n");
      return;
  }

  // Remove newline from userInput before storing
  char cleanInput[strlen(userInput) + 1];
  strcpy(cleanInput, userInput);
  if (cleanInput[strlen(cleanInput) - 1] == '\n') {
      cleanInput[strlen(cleanInput) - 1] = '\0';
  }

  fprintf(file, "%s\n", cleanInput); // Write to the file
  fclose(file); // Close the file
}

void retrieve_commands() {
  FILE *file = fopen(".421sh", "r"); // Open history file in read mode

  if (!file) {
      perror("Error: Couldn't open .421sh file.\n");
      return;
  }

  char* array[10] = {NULL}; // Stores up to 10 commands
  char* line = NULL;
  size_t len = 0;
  int count = 0;

  // Read each line from the file
  while (getline(&line, &len, file) != -1) {
      // Ensure a maximum of 10 commands are stored
      if (count < 10) {
          array[count] = strdup(line); // Store command
      } else {
          free(array[0]); // Remove the oldest command
          for (int i = 0; i < 9; i++) {
              array[i] = array[i + 1]; // Shift all elements left
          }
          array[9] = strdup(line); // Store the newest command
      }
      count++;
  }

  fclose(file);

  // Print last 10 commands in reverse order (newest first)
  printf("Command History:\n");
  for (int i = count - 1; i >= 0 && i >= count - 10; i--) {
      printf("%d: %s", count - i, array[i]);
      free(array[i]); // Free dynamically allocated memory
  }
  free(line); // Free last allocated line
}
