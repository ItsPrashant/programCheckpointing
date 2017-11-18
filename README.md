# programCheckpointing
It contains program to be checkpointed and program to resume or restart it.
testForCkpt.c program makes checkpoint file itself without any use of dynamic library as all functions are contianed in it.
It is just for testing purpose. I have made a .so library too.
testForRestart.c takes name of .bin file as parameter and remaps every segment of memory of testForCkpt process prior checkpointing.
Atlast it switch its context to testForCkpt context where it gets hang. 
I guess the problem is with context switching. When should getcontxt() be called is the main question. 
