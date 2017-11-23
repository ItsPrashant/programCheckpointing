# programCheckpointing
It contains program to be checkpointed and program to resume or restart it.
libckpt.c is the file which contains signal handler for SIGINT. 
myProg.c is a simple counting program.
testForRestart has been updated and it will be used to restart or resume the myProg.
We will just have to type "make check " in terminal and the process begins.
NOTE: Extra information will be printed on the screen for debugging purpose.
