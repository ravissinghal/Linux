/*
   
   The parent process creates the first child process.
   Then the first child creates the pipe and the second child process.
   The second child creates another pipe and the third child process.
   
   I have grouped all the printf statements under conditional compilation.
   
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#define  STDIN_PIPE  0     /* pipe index for the fd to read from */
#define STDOUT_PIPE  1     /* pipe index for the fd to write to  */

/* #define DEBUG */        /*  DEBUG defination*/

int main(int argc, char* argv[])
{
   int fork_rv;
   char *P1,*P2,*P3;
   
   //taking inputs from command line arguments for 3 program to exec
   if(argc == 4) 
   { 
      // get an operand from the command-line
      P1 = argv[1];
      P2 = argv[2];
      P3 = argv[3];
   }
   
   else
   {
      perror("The program takes 3 arguments which is then passed to 3 child process.\nSince you have not passed 3 parameters the program will not be executed.\nPlease run program with 3 parameters");
      exit(1);
   }

   fork_rv = fork();          /* Step 1a: create new process */
   if ( fork_rv == -1 )       /* check for error */
   {
      perror("fork");
   }
   else if ( fork_rv > 0 )    /* parent */
   {
      /* Step 1b: wait on child 1 */
      int wait_rv;
      
      #ifdef DEBUG            /*ifdef directive */
      printf("pid=%d is the parent of child 1 with pid= %d\n", getpid(), fork_rv);
      #endif
      
      wait_rv = wait(NULL);
      
      #ifdef DEBUG
      printf("pid=%d is done waiting for %d. Wait returned: %d\n", getpid(), fork_rv, wait_rv);
      printf("The pipeline is done with its work.\n");
      #endif
   }
   else  /* child 1 */
   {
      int fork_rv1;
      int pipefd[2];    /* the pipe */

      #ifdef DEBUG
      printf("pid=%d is child 1\n", getpid());
      #endif
      
      if ( pipe(pipefd) == -1 )   /* Step 2: create a pipe */
      {
         perror("pipe");
      }
      #ifdef DEBUG
      printf("pid=%d got a pipe! It is file descriptors: { %d %d }\n", getpid(), pipefd[0], pipefd[1]);
      #endif
      
      fork_rv1 = fork();         /* Step 3: create new process */
      if ( fork_rv1 == -1 )      /* check for error */
      {
         perror("fork1");
      }
      else if ( fork_rv1 > 0 )   /* child1 */
      {
         char *arglist[2];
         #ifdef DEBUG
         printf("pid=%d is the parent of child 2 with pid= %d\n", getpid(), fork_rv1);
         #endif

         /* Step 4: child 1 calls close(4), close(0), dup(3), close(3) */
         close(pipefd[STDOUT_PIPE]);             /* pipefd[1] == 4 */
         close(STDIN_FILENO);                    /* STDIN_FILENO == 0 */ 
          
         if ( dup(pipefd[STDIN_PIPE]) == -1 )    /* pipefd[0] == 3 */
         {
            perror("dup child1 error");
         }
         close(pipefd[STDIN_PIPE]);             /* pipefd[0] == 3 */ 
         
         
         /* Step 10 : child 1 exec  */
         arglist[0] = P3;
         arglist[1] = 0 ;
         execvp( P3 , arglist );
         perror("execvp P3");
         exit(1);
         
      }
      else  /* child 2 */
      {
         
         int fork_rv2;
         int pipefd1[2];   /* the pipe */

         #ifdef DEBUG
         printf("pid=%d is child 2\n", getpid());
         #endif
         
         
         if ( pipe(pipefd1) == -1 )          /* Step 5: create a pipe */
         {
            perror("pipe1");
         }
         
         #ifdef DEBUG
         printf("pid=%d got a pipe! It is file descriptors: { %d %d }\n", getpid(), pipefd1[0], pipefd1[1]);
         #endif

         fork_rv2 = fork();                  /* Step 6: create new process */
         if ( fork_rv2 == -1 )               /* check for error */
         {  
            perror("fork2");
         }
         else if ( fork_rv2 > 0 )            /* child2 */
         {
            char *arglist[2];
            
            #ifdef DEBUG
            printf("pid=%d is the parent of child 3 with pid=%d\n", getpid(), fork_rv2);
            #endif
            
            /* Step 7: child 2 calls close(3), close(1), dup(4), close(4) */
            close(pipefd[STDIN_PIPE]);                /* pipefd[0] == 3 */
            close(STDOUT_FILENO);                     /* STDOUT_FILENO == 1 */
            if (dup( pipefd[STDOUT_PIPE] ) == -1)     /* pipefd[1] == 4 */
            {
               perror("dup child2 error");
            }
            close( pipefd[STDOUT_PIPE] );             /* pipefd[1] == 4 */
         

            /* Step 8: child 2 calls close(6), close(0), dup(5), close(5) */
            close(pipefd1[STDOUT_PIPE]);           /* pipefd1[1] == 6 */
            close(STDIN_FILENO);                   /* STDIN_FILENO == 0 */
            if(dup(pipefd1[STDIN_PIPE]) == -1)     /* pipefd1[0] == 5 */
            {
               perror("dup child2 error");
            }
            close(pipefd1[STDIN_PIPE]);             /* pipefd1[0] == 5 */
         
         
            /* Step 10 : child 2 exec */
            arglist[0] = P2;
            arglist[1] = 0;
            execvp( P2 , arglist );
            perror("execvp P2");
            exit(1);
         }
         else  /* child 3 */
         {
            char *arglist[2];
            
            #ifdef DEBUG
            printf("pid=%d is child 3\n", getpid());
            #endif
            
            /* Step 9: child 3 calls close(3), close(1), dup(4), close(4) */
            
            close(pipefd1[STDIN_PIPE]);               /* pipefd[0] == 3 */
            close(STDOUT_FILENO);                     /* STDOUT_FILENO == 1 */
            if(dup( pipefd1[STDOUT_PIPE] ) == -1)     /* pipefd[1] == 4 */
            {
               perror("dup child3 error");
            }
            close( pipefd1[STDOUT_PIPE] );            /* pipefd[1] == 4 */
            
            /* Step 10: child 3 exec */
            arglist[0] = P1;
            arglist[1] = 0;
            execvp( P1 , arglist );
            perror("execvp P1");
            exit(1);
         }
      }
      
         
   
   }
   return 0;
}
