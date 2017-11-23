#ifndef _LIBCKPT_H_
#define _LIBCKPT_H_

extern void hexToDec(char *str,unsigned long long adrr[2]);
extern int checkMemoryType(char *str,int size);
extern int readLine(FILE *,char *);

int contextFlag=0;
ucontext_t context, *cp = &context;

extern char permission(char *);
extern void intToStr(unsigned long long data,char *str);

extern void myHandler(int sig);


#endif
