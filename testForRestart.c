#include<stdio.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<ucontext.h>

ucontext_t context,*cp=&context;

char filename[200];
char fileAddr[50]="cat /proc/";
char buffer[200];
FILE *fp1,*ckptFp;
char *stk="]kcats[";
char *vsys="]llacsysv[";
unsigned long long addr1[2];
void *stack_ptr,*stack_ptr2;
	
int permission(char *str);

void intToStr(unsigned long long data,char *str);

void *getMapFileName(char *cmd1);

int readLine(FILE *ptr,char *str);

void hexToDec(char *str,unsigned long long addr[2]);
void restoreStack();

void print(char *str);

int main(int argc,char *argv[])
{
	
	if(argc<2)
	{
		printf("File name was not passed.\nTerminating..........");
		return 1;
	}
	
	strcpy(filename,argv[1]);
	printf("argc:%d\nFilename:%s\n",argc,filename);	
	//while(1);
	stack_ptr=mmap((void*)0x5300000,0x1000,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED,-1,0);
	if(stack_ptr== MAP_FAILED)
	{
          perror ("mmap");
          
          return 1;
    }
    else
    {
		stack_ptr=(void *)0x5300fff;
		printf("Mapping successful\nChanging Stack pointer.\n");
		asm volatile("mov %0,%%rsp ;" : : "g" (stack_ptr) : "memory");
		restoreStack();
	}
	
	
}

void restoreStack()
{
	getMapFileName(fileAddr);
	
	printf("\nCmd to read Map file:%s\n",fileAddr);
	fp1=popen(fileAddr,"r");
	if(fp1==NULL)
	printf("Unable to execute cat command\n");
	
	int len,c=0;
	
	while(readLine(fp1,buffer)!=EOF)
	{
		printf("%s\n",buffer);
		len=strlen(buffer);
		c=0;
		for(int i=len-1;c<7;i--)
		{
			if(stk[c++]!=buffer[i])
			break;
		}
		if(c==7)
		{
			printf("\nStack found\n");
			hexToDec(buffer,addr1);
			printf("stackAddr: %llu - %llu\n",addr1[0],addr1[1]);
			if(munmap((void *)addr1[0],addr1[1]-addr1[0])!=0)
			printf("Unmap failed\n");
			else
			printf("Unmapped old stack..................\n");
			
			break;
		}
	}
		
	
	if(fclose(fp1)!=0)
	printf("unable to close file\n");
	
	ckptFp=fopen(filename,"rb");
	
	if(ckptFp==NULL)
	{
			printf("Error opening file.\n");
	}
	else
	{
		printf("File opened successfully.\n");
		int strLen;
		void *startAddr;


		
		while(fread(&strLen,sizeof(strLen),1,ckptFp))
		{
			
			if(strLen==100000)
			{
				
				break;
			}
			if(fread(buffer,sizeof(char),strLen,ckptFp)==0)
			printf("Unable to read from file. fread() failed\n");
			
			printf("String read is :%s\n",buffer);
			hexToDec(buffer,addr1);
			
			startAddr=mmap((void *)addr1[0],addr1[1]-addr1[0],PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED,-1,0);
			if(startAddr==MAP_FAILED)
			printf("mmap failed to map memory\n");
			else
			{
				if(fread(startAddr,(addr1[1]-addr1[0]),1,ckptFp)==0)
				printf("Unable to read from file. fread() failed\n");
				
				printf("fPtr: %llu \n",ftell(ckptFp));
				if(mprotect(startAddr,addr1[1]-addr1[0],permission(buffer))!=0)
				printf("mprotect() failed \n");
				
			}
		}
		
		printf("Reading context\n");
		if(fread(cp,sizeof(ucontext_t),1,ckptFp)==0)
		printf("Unable to read from file. fread() failed\n");
				
		//fread(cp,sizeof(ucontext_t),1,ckptFp);

	 
	}
	
	printf("Read all Data....\nResuming old process... :)\n");
	
	if(fclose(ckptFp)!=0)
	{
		printf("fclose() failed\n");
				
	}
	printf("Program resumes here: \n");
	//sleep(2);
	
	if(setcontext(cp)==-1)
	{
		printf("setcontext() unsuccesful\n");
	}
	//	while(1);
}

int permission(char *str)
{
	int ptr=0;
	int perm=0;
	while(str[ptr++]!=' ');
	
	for(int i=0;i<3;i++)
	{
		switch(str[ptr+i])
		{
			case 'r':perm|=PROT_READ;
			break;	
			case 'w':perm|=PROT_WRITE;
			break;
			case 'x':perm|=PROT_EXEC;
			break;
		}
	}
	return perm;
	
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





void *getMapFileName(char *cmd1)
{
		char ch[20];
		char cmd2[]="/maps";
		int t;
		
		t=getpid();
		printf("PID = %d ",t);
		intToStr(t,ch);
		int j;
		for(j=10;ch[j-10]!='\0';j++)
		{
			
			cmd1[j]=ch[j-10];
		//	printf("cmd1[%d] :(%c) ",j,cmd1[j]);
		}
		for(int i=j;i<j+sizeof(cmd2);i++)
		{
			cmd1[i]=cmd2[i-j];
		//	printf("cmd1[%d] :(%c) ",i,cmd1[i]);
		}
		//printf("\ncmd1, ch[20], cmd2 : %s %s %s\n",cmd1,ch,cmd2);
	
	
}
