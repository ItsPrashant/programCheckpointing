#include<string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <unistd.h>
//#include "libckpt.h"
#include<sys/mman.h>
 
ucontext_t context, *cp = &context;

int flag=0;

void intToStr(unsigned long long data,char *str)
{
	int i=0;
	while(data!=0)
	{	
	//	printf("%llu ",data);
		str[i++]=data%10+48;
		data/=10;
	}
	str[i]='\0';
	char temp;
	
	for(int j=i-1;j>=i/2;j--)
	{
		temp=str[j];
		str[j]=str[i-j-1];
		str[i-j-1]=temp;	
	}
	
	printf("%s ",str);
}

void hexToDec(char *str,unsigned long long addr[2])
{
	
	int i=0;
	addr[0]=0;
	addr[1]=0;
	
	while(str[i]!='-')
	{	//printf("%c",str[i]);
		addr[0]=addr[0]<<4;
		if(str[i]>=48&&str[i]<=57)
		addr[0]+=(str[i]-48);
		else
		if(str[i]>=97&&str[i]<=102)
		addr[0]+=(str[i]-87);
		else
		{
			//printf("%c   ",str[i]);
			fflush(stdout);
			perror("Error in converting hex to dec");
			//exit(1);
		}
		i++;
		
	}
	i++;
	while(str[i]!=' ')
	{	
		//printf("%c",str[i]);
		addr[1]=addr[1]<<4;
		if(str[i]>=48&&str[i]<=57)
		addr[1]+=(str[i]-48);
		else
		if(str[i]>=97&&str[i]<=102)
		addr[1]+=(str[i]-87);
		else
		{
			//printf("%c   ",str[i]);
			fflush(stdout);
			perror("Error in converting hex to dec");
			//exit(1);
		}
		i++;	
	}
	
	
}

char permission(char *str)
{
	int ptr=0;
	while(str[ptr++]!=' ');
	
	return str[ptr];
}
int readLine(FILE *ptr,char *str)
{
	char ch;
	ch=fgetc(ptr);
	int c=0;
	while(ch!=EOF&&ch!='\n')
	{
	//	printf("%c",ch);
		str[c++]=ch;
		ch=fgetc(ptr);
	}
	str[c]='\0';

	
	return ch;
}

int checkMemoryType(char *str,int size)
{
	if(str[size-1]==']'&&str[size-2]=='r'&&str[size-3]=='a'&&str[size-4]=='v'&&str[size-5]=='v'&&str[size-6]=='[')
	return 0;
	if(str[size-1]==']'&&str[size-2]=='l'&&str[size-3]=='l'&&str[size-4]=='a'&&str[size-5]=='c'&&str[size-6]=='s'&&str[size-7]=='y'&&str[size-8]=='s'&&str[size-9]=='v'&&str[size-10]=='[')
	return 0;
	if(str[size-1]==']'&&str[size-2]=='o'&&str[size-3]=='s'&&str[size-4]=='d'&&str[size-5]=='v'&&str[size-6]=='[')
	return 0;
	
	return 1;
}

void myHandler(int sig)
{
	FILE *fp,*ckptFile;
	char ch[20];
	int t;
	char buffer[200];
	int count=0;
	//unsigned long long *ptr;
	void *ptr;		
	char cmd1[20]="cat /proc/";
	char cmd2[]="/maps";
	unsigned long long addr1[2];		
	int j,i;
	int strSize;

		
	if(flag==0)
	{
		flag=1;
		t=getpid();
		printf("PID = %d ",t);
		intToStr(t,ch);
		for(j=10;ch[j-10]!='\0';j++)
		{
			
			cmd1[j]=ch[j-10];
		//	printf("cmd1[%d] :(%c) ",j,cmd1[j]);
		}
		for(i=j;i<j+sizeof(cmd2);i++)
		{
			cmd1[i]=cmd2[i-j];
		//	printf("cmd1[%d] :(%c) ",i,cmd1[i]);
		}

		//printf("\ncmd1, ch[20], cmd2 : %s %s %s\n",cmd1,ch,cmd2);
	
		fp=popen(cmd1,"r");
		ckptFile=fopen("myckpt.bin","wb");

		while(readLine(fp,buffer)!=EOF)
		{	
			strSize=strlen(buffer)+1;		
				
			printf("\n%s and str size:%d \n",buffer,strSize);
			
			hexToDec(buffer,addr1);									//fetch starting and ending addresses from received string
			
			printf("%d. start= %llu end= %llu\n",count,addr1[0],addr1[1]);	
			//printf("\npermission is:%c\n",permission(buffer));
			
			if(permission(buffer)=='r'&&checkMemoryType(buffer,strSize-1))
			{
				fwrite(&strSize,sizeof(int),1,ckptFile);							//write sizeof received string to ckpt file
				fwrite(buffer,sizeof(char),strSize,ckptFile);						//write received string to ckpt file 
				
				printf("Permission granted to read!!\n");
				printf("Total memory to be written: %llu\n",(addr1[1]-addr1[0]));
				
				ptr=(void *)addr1[0];
				
				fwrite(ptr,addr1[1]-addr1[0],1,ckptFile);
										
														
					
			}
			 
		}
		strSize=100000;
		fwrite(&strSize,sizeof(int),1,ckptFile);
		getcontext(cp);
		printf("In restoreStack()\n");	

		fwrite(cp,sizeof(ucontext_t),1,ckptFile);						//write current context to file					
		fclose(fp);		//close chkpt file
		
	}
	else
	{
		printf("hello!!\n");
		flag=0;
	}


}

int main()
{
	//while(1)
	//{
		printf("Welcome to memory read!!\n");
		
		myHandler(1);
		//myHandler(1);
		
		printf("Hello from main\n");
		//setcontext(cp);
	//}
}
