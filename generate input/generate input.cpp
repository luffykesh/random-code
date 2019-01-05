#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <csignal>

#define FORE_BOLD "\e[1m"
#define FORE_RED "\e[31m"
#define FORE_GREEN "\e[32m"
#define FORE_YELLOW "\e[33m"
#define FORE_BLUE "\e[34m"
#define FORE_MAGENTA "\e[35m"
#define FORE_CYAN "\e[36m"
#define RESET "\e[0m"

#define BACK_GREEN "\e[102m"
#define BACK_GREY "\e[100m"

#define PROGRESS_BAR_MAX_STEP (100)
#define PRINT_BUFFER (1000)

using namespace std;

void initconsole();
void update_progress(int p);
void bufferprint(char * s);
void intprint(const long int offset, const long int count,FILE * out);
void cprint(const long int offset, const long int count,FILE * out);
void lprint(const long int offset, const long int count, FILE *out);
void printhelp();
void sighandler(int s);
void cleanup();

FILE *out = NULL ;

int main(int argc, char * argv[])
{
	out = stdout;
	char * fname = NULL;
	long int ncount=250;
	long int ccount=0,lcount=0;
	bool lflag=false,cflag=false,nflag=false;
	long int startnumber = 0;
	if(argc==1)
	{
		printhelp();
		return 1;
	}
	for(long int i = 1 ; i< argc; ++i )
	{
		if(strcmp(argv[i],"-h")==0 || strcmp(argv[i],"--help")==0)
		{
			printhelp();
			return 1;
		}
		else if(strcmp(argv[i],"-o")==0 || strcmp(argv[i],"--out")==0)
		{
			++i;
			fname = argv[i];
		}
		else if(strcmp(argv[i],"-n")==0)
		{
			++i;
			ncount=strtol(argv[i],NULL,10);
			if(ncount==0) ncount = 250;
			nflag=true;
			cflag=lflag=false;
		}
		else if(strcmp(argv[i],"-c")==0)
		{
			++i;
			ccount=strtol(argv[i],NULL,10);
			cflag=true;
			nflag=lflag=false;
		}
		else if(strcmp(argv[i],"-l")==0)
		{
			++i;
			lcount=strtol(argv[i],NULL,10);
			lflag=true;
			nflag=cflag=false;
		}
		else if(strcmp(argv[i],"-s")==0 || strcmp(argv[i],"--offset")==0)
		{
			++i;
			startnumber =strtol(argv[i],NULL,10);
		}
		else
		{
			printf(FORE_RED "Invalid Option %s\n" RESET,argv[i]);
			printhelp();
			return -2;
		}
	}
	signal(SIGINT,sighandler);
	signal(SIGABRT,sighandler);
	if(fname!=NULL)
		out = fopen(fname,"w");
	if(out!=stdout)
		initconsole();
	if(out==NULL)
	{
		fprintf(stdout,"%s\n","Cannot open file.Abort!");
		return -1;
	}
	if(nflag)
		intprint(startnumber,ncount,out);
	else if(cflag)
		cprint(startnumber,ccount,out);
	else if(lflag)
		lprint(startnumber,lcount,out);
	cleanup();
	cout<<endl;
}
void initconsole()
{
	printf("[%*c]0%%",PROGRESS_BAR_MAX_STEP,' ');
}
void update_progress(int p)
{
	static int prevprogress=0;
	int back;
	if(prevprogress==p)
	{
		return;
	}
	int i=0;
	// printf("\nprevprogress:%d p:%d\n", prevprogress,p);

	//'%'
	back = 1;
	//1-100
	if(prevprogress<10) ++back;
	else if(prevprogress<100) back+=2;
	else back+=3;
	//] + empty bar length
	back+=1+(PROGRESS_BAR_MAX_STEP-prevprogress);
	// printf("\nback=%d\n",back);

	printf("\e[%dD\e[K",back);

	printf(BACK_GREY);
	for(i=prevprogress;i<p;++i)
		printf(" ");
	printf(RESET);

	for(;i<PROGRESS_BAR_MAX_STEP;++i)
		printf(" ");
	printf("]");
	printf(FORE_BOLD"%d%%"RESET,p);
	fflush(stdout);
	prevprogress=p;
}

void bufferprint(char * s)
{
	static char buf[PRINT_BUFFER+1];
	if(s==NULL && strlen(buf)!=0)
	{
		fprintf(out,"%s",buf);
		memset(buf,0,sizeof(buf));
		return;
	}
	if(strlen(s)+strlen(buf)<sizeof(buf))
		strcat(buf,s);
	else
	{
		fprintf(out,"%s%s",buf,s);
		memset(buf,0,sizeof(buf));
	}
}

void intprint(const long int offset, const long int count,FILE * out)
{
	// cout<<"intprint\n";
	char pdata[22];
	long int i = 0 ;
	while(i<count)
	{
		if(i!=count-1)
			snprintf(pdata,sizeof(pdata),"%ld ",offset+i);
		else
			snprintf(pdata,sizeof(pdata),"%ld",offset+i);
		bufferprint(pdata);
		if(out!=stdout)
			update_progress((i*PROGRESS_BAR_MAX_STEP)/count);
		++i;
	}
	update_progress((i*PROGRESS_BAR_MAX_STEP)/count);
}

void cprint(const long int offset,const long int count,FILE * out)
{
	// cout<<"cprint\n";
	long int size=0;
	char * data;
	char pdata[22];
	long int i = offset;
	int n;
	while(size<count)
	{
		asprintf(&data,"%lu ",i);
		size+=snprintf(pdata,(count-size)+1,"%s",data);
		if(strlen(pdata)!=strlen(data))
			size=count;
		bufferprint(pdata);
		++i;
		if(out!=stdout)
			update_progress((size*PROGRESS_BAR_MAX_STEP)/count);
	}
	update_progress((size*PROGRESS_BAR_MAX_STEP)/count);
	free(data);
}

void lprint(const long int offset,const long int count, FILE *out)
{
	// cout<<"\nlprint\n"<<count;

	char pdata[22];
	long int i=0;
	while(i<count)
	{
		snprintf(pdata,sizeof(pdata),"%ld\n",offset+i);
		bufferprint(pdata);
		if(out!=stdout)
			update_progress((i*PROGRESS_BAR_MAX_STEP)/count);
		++i;
	}
	update_progress((i*PROGRESS_BAR_MAX_STEP)/count);
}
void printhelp()
{
	printf(FORE_BOLD"Command%10cDescription\n"RESET,' ');
	printf("%-17s%s\n","-h/--help","print this help");
	printf("%-17s%s\n","-c","print given no of characters");
	printf("%-17s%s\n","-l","print given no of lines");
	printf("%-17s%s\n","-n","print till n");
	printf("%-17s%s\n","-o/--out","output file name");
	printf("%-17s%s\n","-s/--offset","count start offset (default is 0)");
}

void sighandler(int s)
{
	cleanup();
	printf(FORE_RED"\nSignal(%d)\n"RESET,s);
	exit(0);
}
void cleanup()
{
	bufferprint(NULL);
	if(out!=stdout)
		fclose(out);
}