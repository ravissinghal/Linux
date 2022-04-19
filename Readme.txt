
In this version, the parent process creates the first child process.
Then the first child creates the pipe and the second child process.
Then the second child creates the pipe1 and the third child process.

Start with a parent process.

               parent
          +---------------+
          |               |
  stdin-->> 0           1 >>----> stdout
          |               |
          |             2 >>----> stderr
          |               |
          |               |
          +---------------+

Step 1:
The parent calls fork() (and then waits on the child_1 process).

                   parent
              +---------------+
              |               |
          +-->> 0           1 >>-----------+
          |   |               |            |
          |   |             2 >>-> stderr  |
          |   |               |   (shared) |
          |   |               |            |
          |   +---------------+            |
          |                                |
  stdin --+                                +---> stdout
 (shared) |                                |    (shared)
          |   +---------------+            |
          |   |               |            |
          +-->> 0           1 >>-----------+
              |               |
              |             2 >>-> stderr
              |               |   (shared)
              |               |
              +---------------+
                   child_1

Step 2:
The child calls pipe() which creates two new file descriptors(3,4) in the child.

                   parent
              +---------------+
              |               |
          +-->> 0           1 >>------------+
          |   |               |             |
          |   |             2 >>--> stderr  |
          |   |               |    (shared) |
          |   +---------------+             |
  stdin --+                                 +---> stdout
 (shared) |                                 |    (shared)
          |        child_1                  |
          |   +---------------+             |
          |   |               |             |
          +-->> 0           1 >>------------+
              |               |
              |             2 >>--> stderr
              |               |    (shared)
              |  4         3  |
              +--\/-------/\--+
                 |         |
                 |  pipe   |
                 +-0=====0-+

Step 3:
child_1 calls fork() to create child_2 with which it shares the pipe.


                   parent
              +---------------+
              |               |
          +-->> 0           1 >>-----------+
          |   |               |            |
          |   |             2 >>-> stderr  |
          |   |               |  (shared)  |
          |   +---------------+            |
  stdin --+                                +--> stdout
 (shared) |                                |   (shared)
          |        child_1                 |
          |   +---------------+            |
          |   |               |            |
          +-->> 0           1 >>-----------+
          |   |               |            |
          |   |             2 >>-> stderr  |
          |   |               |  (shared)  |
          |   |  4         3  |            |
          |   +--\/-------/\--+            |
          |      |         |               |
          |      |  pipe   |               |
          |      +-0=====0-+               |
          |      |         |               |
          |      |         |               |
          |   +--/\-------\/--+            |
          |   |  4         3  |            |
          |   |               |            |
          +-->> 0           1 >>-----------+
              |               |
              |             2 >>--> stderr
              |               |    (shared)
              +---------------+
                   child_2

Step 4:
child_1 now calls close(4), close(0), dup(3), close(3).
child_1's standard input is now redirected to the pipe's output.

                   parent
              +---------------+
              |               |
          +-->> 0           1 >>-----------+
          |   |               |            |
          |   |             2 >>-> stderr  |
          |   |               |   (shared) |
          |   +---------------+            |
  stdin --+                                +--> stdout
 (shared) |                                |   (shared)
          |        child_1                 |
          |   +---------------+            |
          |   |               |            |
          |   |             1 >>-----------+
          |   |               |            |
          |   |             2 >>-> stderr  |
          |   |               |   (shared) |
          |   |           0   |            |
          |   +-----------/\--+            |
          |                |               |
          |         pipe   |               |
          |      +-0=====0-+               |
          |      |         |               |
          |      |         |               |
          |   +--/\-------\/--+            |
          |   |  4         3  |            |
          |   |               |            |
          +-->> 0           1 >>-----------+
              |               |
              |             2 >>--> stderr
              |               |    (shared)
              +---------------+
                   child_2

Step 5:
The child_2 calls pipe() which creates two new file descriptors(5,6) in the child_2.


                   parent
              +---------------+
              |               |
          +-->> 0           1 >>-----------+
          |   |               |            |
          |   |             2 >>-> stderr  |
          |   |               |   (shared) |
          |   +---------------+            |
  stdin --+                                +--> stdout
 (shared) |                                |   (shared)
          |        child_1                 |
          |   +---------------+            |
          |   |               |            |
          |   |             1 >>-----------+
          |   |               |            |
          |   |             2 >>-> stderr  |
          |   |               |   (shared) |
          |   |           0   |            |
          |   +-----------/\--+            |
          |                |               |
          |         pipe   |               |
          |      +-0=====0-+               |
          |      |         |               |
          |      |         |               |
          |   +--/\-------\/--+            |
          |   |  4         3  |            |
          |   |               |            |
          +-->> 0           1 >>-----------+
              |               |		  
              |             2 >>--> stderr
              |    child_2    |   (shared) 
              +--\/-------/\--+ 	   
                 6	   5		   
                 |         |
                 |  pipe1  |
                 +-0=====0-+


Step 6:
child_2 calls fork() to create child_3 with which it shares the pipe1.

                   parent
              +---------------+
              |               |
          +-->> 0           1 >>-----------+
          |   |               |            |
          |   |             2 >>-> stderr  |
          |   |               |   (shared) |
          |   +---------------+            |
  stdin --+                                +--> stdout
 (shared) |                                |   (shared)
          |        child_1                 |
          |   +---------------+            |
          |   |               |            |
          |   |             1 >>-----------+
          |   |               |            |
          |   |             2 >>-> stderr  |
          |   |               |   (shared) |
          |   |           0   |            |
          |   +-----------/\--+            |
          |                |               |
          |         pipe   |               |
          |      +-0=====0-+               |
          |      |         |               |
          |      |         |               |
          |   +--/\-------\/--+            |
          |   |  4         3  |            |
          |   |               |            |
          +-->> 0           1 >>-----------+
          |   |               |		   |
          |   |             2 >>--> stderr |
          |   |    child_2    |   (shared) |
          |   +--\/-------/\--+ 	   |
          |      6	   5		   |
 	  |      |         |               |
          |      |  pipe1  |               |
          |      +-0=====0-+               |
          |      |         |               |
          |      |         |               |
          |   +--/\-------\/--+            |
          |   |  4         3  |            |
          |   |               |            |
          +-->> 0           1 >>-----------+
              |               |
              |             2 >>--> stderr
              |               |    (shared)
              +---------------+
                   child_3



Step 7:
child_2 now calls close(3), close(1), dup(4), close(4).
child_2's standard output is now directed to the pipe's input.

                   parent
              +---------------+
              |               |
          +-->> 0           1 >>-----------+
          |   |               |            |
          |   |             2 >>-> stderr  |
          |   |               |   (shared) |
          |   +---------------+            |
  stdin --+                                +--> stdout
 (shared) |                                |   (shared)
          |        child_1                 |
          |   +---------------+            |
          |   |               |            |
          |   |             1 >>-----------+
          |   |               |            |
          |   |             2 >>-> stderr  |
          |   |               |   (shared) |
          |   |           0   |            |
          |   +-----------/\--+            |
          |                |               |
          |         pipe   |               |
          |      +-0=====0-+               |
          |      |                         |
          |      |                         |
          |   +--/\-----------+            |
          |   |   1           |            |
          |   |               |            |
          +-->> 0             |		   |
          |   |               |		   |
          |   |             2 >>--> stderr |
          |   |    child_2    |   (shared) |
          |   +--\/-------/\--+ 	   |
          |      6	   5		   |
 	  |      |         |               |
          |      |  pipe1  |               |
          |      +-0=====0-+               |
          |      |         |               |
          |      |         |               |
          |   +--/\-------\/--+            |
          |   |  4         3  |            |
          |   |               |            |
          +-->> 0           1 >>-----------+
              |               |
              |             2 >>--> stderr
              |               |    (shared)
              +---------------+
                   child_3


Step 8:
child_2 now calls close(6), close(0), dup(5), close(5).
child_2's standard input is now redirected to the pipe1's output.

                   parent
              +---------------+
              |               |
          +-->> 0           1 >>-----------+
          |   |               |            |
          |   |             2 >>-> stderr  |
          |   |               |   (shared) |
          |   +---------------+            |
  stdin --+                                +--> stdout
 (shared) |                                |   (shared)
          |        child_1                 |
          |   +---------------+            |
          |   |               |            |
          |   |             1 >>-----------+
          |   |               |            |
          |   |             2 >>-> stderr  |
          |   |               |   (shared) |
          |   |           0   |            |
          |   +-----------/\--+            |
          |                |               |
          |         pipe   |               |
          |      +-0=====0-+               |
          |      |                         |
          |      |                         |
          |   +--/\-----------+            |
          |   |   1           |            |
          |   |               |            |
          |   |               |		   |
          |   |               |		   |
          |   |             2 >>--> stderr |
          |   | child_2   0   |   (shared) |
          |   +----------\/---+ 	   |
          |      	  |		   |
 	  |               |                |
          |        pipe1  |                |
          |     +-0=====0-+                |
          |     |         |                |
          |     |         |                |
          |  +--/\-------\/--+             |
          |  |  4         3  |             |
          |  |               |             |
          +->> 0            1 >>-----------+
             |               |
             |             2 >>--> stderr
             |               |    (shared)
             +---------------+
                  child_3


Step 9:
child_3 now calls close(3), close(1), dup(4), close(4).
child_2's standard input is now redirected to the pipe1's output.

                   parent
              +---------------+
              |               |
          +-->> 0           1 >>-----------+
          |   |               |            |
          |   |             2 >>-> stderr  |
          |   |               |   (shared) |
          |   +---------------+            |
  stdin --+                                +--> stdout
 (shared) |                                |   (shared)
          |        child_1                 |
          |   +---------------+            |
          |   |               |            |
          |   |             1 >>-----------+
          |   |               |            
          |   |             2 >>-> stderr  
          |   |               |   (shared) 
          |   |           0   |            
          |   +-----------/\--+            
          |                |               
          |         pipe   |               
          |      +-0=====0-+               
          |      |                         
          |      |                         
          |   +--/\-----------+            
          |   |   1           |            
          |   |               |            
          |   |               |		   
          |   |               |		   
          |   |             2 >>--> stderr 
          |   | child_2   0   |   (shared) 
          |   +----------\/---+ 	   
          |      	  |		   
 	  |               |                
          |        pipe1  |                
          |     +-0=====0-+                
          |     |                         
          |     |                        
          |  +--/\-----------+             
          |  |  1            |             
          |  |               |             
          +->> 0             |
             |               |
             |             2 >>--> stderr
             |               |    (shared)
             +---------------+
                  child_3


Now child_1, child_2 and child_3 are configured as a pipeline.
Here is a simplified picture of the above pipeline.
(Recall that the parent is waiting on the child_1 process.)

                        parent
                    +------------+
                    |            |
          +-------->> 0        1 >>-------------------------------------------------+
          |         |            |                    	 			    |
          |         |          2 >>----> stderr    	 			    |
          |         |            |      (shared)         			    |
  stdin --+         +------------+                       			    +--> stdout
 (shared) |                                              			    |   (shared)
          |                                              			    |
          |       child_3                 child_2        	    child_1	    |
          |   +------------+           +------------+    	  +------------+    |
          |   |            |   pipe1   |            |   pipe      |	       |    |
          +-->> 0        1 >>-0======0->>0        1 >>-0======0->>0          1 >>---+
              |            |           |            |             |            |
              |          2 >>->stderr  |          2 >>---> stderr |          2 >>---> stderr
              |            | (shared)  |            |     (shared)|	       |     (shared)
              +------------+           +------------+             +------------+ 


Step 10: 

All the three child processes now call exec() to run programs which were passed through command-line. 

For example:
> double | remove_vowels | twiddle

                        parent
                    +------------+
                    |            |
          +-------->> 0        1 >>-------------------------------------------------+
          |         |            |                    	 			    |
          |         |          2 >>----> stderr    	 			    |
          |         |            |      (shared)         			    |
  stdin --+         +------------+                       			    +--> stdout
 (shared) |                                              			    |   (shared)
          |                                              			    |
          |       double                remove_vowels        	    twiddle	    |
          |   +------------+           +------------+    	  +------------+    |
          |   |            |   pipe    |            |   pipe1     |	       |    |
          +-->> 0        1 >>-0======0->>0        1 >>-0======0->>0          1 >>---+
              |            |           |            |             |            |
              |          2 >>->stderr  |          2 >>---> stderr |          2 >>---> stderr
              |            | (shared)  |            |     (shared)|	       |     (shared)
              +------------+           +------------+             +------------+ 


>> 
Input:- arteio
Output:- rrtt