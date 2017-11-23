#include<stdio.h>
#include<unistd.h>
int main()
{
	int i=0;
	for(;;)
	{
		printf("%d ",i++);
		fflush(stdout);
		sleep(2);
		
	}
	
	return 0;
}
