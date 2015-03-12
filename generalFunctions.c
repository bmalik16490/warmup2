/* 
* Author: Bhavya Malik
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include<sys/stat.h>
#include<errno.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "generalFunctions.h"


/*
* Displays correct usage of the command line
*/
void DisplayErrorMessage()
{
	fprintf(stdout,"\nCorrect usage of command line:  ./warmup2 [-n num] [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-t tsfile]\n");

}


/*
* gets number of packets to arrive from tsfile
*/
int GetNoOfPacketsFromFile(char *filename)
{
	struct stat sstat;
	char buff[100], field2[100];
	int i=0;
	float val1, val2;
	char *ptr=NULL;
	memset(field2,'\0',100);
	
	if(!(globalFilePtr=fopen(filename,"r")))
	{
		if(stat(filename, &sstat)==-1)
		{
			perror("Error");
			return 0;			
		}
				
	}
	if(stat(filename, &sstat)==0)
	{
		if(sstat.st_mode & S_IFDIR)
		{
			fprintf(stderr, "Error: Given Path is of a Directory.\n");
			return 0;
			
		}
		else if(sstat.st_mode & S_IFREG) //its a file
		{	
			if(!(globalFilePtr=fopen(filename,"r")))
			{
				perror("Error");
				return 0;						
				
			}
			if(sstat.st_size ==0)
			{
				fprintf(stderr,"Error: Input File is empty.\n");
				return 0;
				
			}
		}
	}
	
	if(fgets(buff,sizeof(buff),globalFilePtr)!=NULL)
	{	//fprintf(stdout,"here***************\n");
		if(buff[0]==' ' || buff[0]== '\t')
		{
			fprintf(stderr, "Error:leading space or tab... exiting\n");
			return 0;
		}
		if(buff[0]== '\n')
		{
			
			fprintf(stderr, "Error: no value for number of packets to arrive... exiting\n");
			return 0;

		}
		while(buff[i]!='\n')
		{
			if(!isdigit(buff[i]))
			{
				fprintf(stderr, "Error: wrong value for n. It should be a positive integer with max value of 2147483647\n");
				return 0;
			}
				
				//fprintf(stdout, "i: %d\n", i);
				field2[i]=buff[i];	
				i++;
		}
		field2[i]='\0';
		if(field2[strlen(field2)-1] == ' ' || field2[strlen(field2)-1]== '\t')
		{
			fprintf(stderr, "Error:trailing space or tab\n");
			return 0;
		}
		val1=strtol(field2, &ptr, 10);
		val2=strtol(ptr, NULL, 10);
		if(!(val2==0 && errno!=ERANGE && val1>=0)) // doesnot overflow and is a valid positive integer
		{
			fprintf(stderr, "Error: num should be a valid positive integer with max value of 2147483647\n");
			return 0;

		}
		else
		{	clp->num=(int)val1;
			return 1;

		}
		
	}
	
	return 0;
}

/*
* Reads inptu from tsfile
*/
int readFromFile()
{

	
	char buff[100];
	int oneByL=0, oneByMU=0,  i=0, j=0, l=0;
	char field[100],field2[100], field3[100];
	int stCount=0;
	long int val1,val2;

	char *ptr=NULL;
	memset(buff,'\0',100);
	memset(field,'\0',100);
	memset(field2,'\0',100);
	memset(field3,'\0',100);
		
	if(fgets(buff,sizeof(buff),globalFilePtr)!=NULL)
	{	i=0, j=0, stCount=0,  l=0;
		while(buff[i]!='\n')
		{
			if(buff[0]==' ' || buff[0]== '\t')
			{
				fprintf(stderr, "Error:leading space or tab... exiting\n");
				return 0;
			}
			if(buff[i]==' ' || buff[i]=='\t')
			{	l++; 
			}
			field[i]=buff[i];
			i++;
			
		}
		field[i]='\0';
		if(field[strlen(field)-1] == ' ' || field[strlen(field)-1]== '\t' )
		{
			fprintf(stderr, "Error:trailing space or tab... exiting\n");
			return 0;
		}
		for(i=0;i<strlen(field);i++)
		{
			if(field[i]!=' ' && field[i]!='\t')
			{
				if(!isdigit(field[i]))
				{
					fprintf(stderr, "Error: a char found in tsfile.. Exiting\n");
					return 0;
				}
				field3[j]=field[i];
				j++;	
			}
			 
			else
			{ field3[j]='\0';
			   if((field[i]==' ' || field[i]=='\t')  && (field[i-1]!=' ' && field[i-1]!='\t'))
			   {
				val1=strtol(field3, &ptr, 10);
				val2=strtol(ptr, NULL, 10);
				if(!(val2==0 && errno!=ERANGE && val1>=0)) // doesnot overflow and is a valid positive integer
				{
					fprintf(stderr, "Error: the field value should be a positive integer.\n");
					return 0;

				}
		         	if(stCount==0)
				{	
					oneByL=val1; 
					
					clp->l=1.0/ (((float)(oneByL))/1000.0);
					
				}
				else if(stCount==1)
				{	
										
					clp->p=val1; 
					
				}
			
				j=0;	
				stCount++;
			   }
			  	
			}
		
		}
		field3[j]='\0';
		val1=strtol(field3, &ptr, 10);
		val2=strtol(ptr, NULL, 10);
		if(!(val2==0&& errno!=ERANGE && val1>=0)) // doesnot overflow and is a valid positive integer
		{
			fprintf(stderr, "Error: the field value should be a positive integer.\n");
			return 0;

		}
		
		oneByMU=val1;//in ms
		clp->mu=1.0/(((float)(oneByMU))/1000.0);
				
		if(stCount!=2)
		{
			fprintf(stderr, "Error: Wrong number of fields in a line. There should be exactly 3 fields.\n");
			return 0;
		}
		
		
		lineCount++;
	}
	else
	{
		fprintf(stderr, "Error: less number of lines when compared with the number of packets specified in the tracefile.\n");
			return 0;

	}
		if(lineCount > clp->num)
		{
			fprintf(stderr, "Error: wrong number of lines when compared with the number of packets specified in the tracefile.\n");
			return 0;
		}
		
	return 1;
}

/*
* Prints timestamp in the correct format
*/
void getTimeInMs(long long int t)
{ char resultBuff[13];
     int i=0,j=0;
    char bd[12];
   
    memset(resultBuff,'0',13);
    memset(bd,'\0',12);
   
    snprintf(bd,12,"%lld",t);
    i=7;
    for(j=strlen(bd)-4;j>=0;j--)
    {	resultBuff[i]=bd[j];
	i--;
    }
    resultBuff[8]='.';
     i=11;
      for(j=strlen(bd)-1;i>8;j--)
    {	resultBuff[i]=bd[j];
	i--;
    }
     
    resultBuff[12]='\0';
  
	fprintf(stdout, "%s", resultBuff);
	
}

/*
* Validates Command line arguments
*/
void GetCommandLineArgs(int argc, char** argv)
{
	int i=1;
	int p=0;
	float val1,val2;
	char *ptr=NULL;
   	


	while(i<argc)
	{
		if(strcmp(argv[i],"-lambda")==0 )
		{ 
			if(i+1<argc)
		  	{ 
				if( strcmp(argv[i+1],"-mu")!=0 && strcmp(argv[i+1],"-B") !=0 &&  strcmp(argv[i+1],"-r") !=0&& strcmp(argv[i+1],"-P") !=0 && strcmp(argv[i+1],"-n") !=0 && strcmp(argv[i+1],"-t") !=0)
				 {
					val1=strtof(argv[i+1], &ptr);
					val2=strtof(ptr, NULL);
					if(val2==0.0 && errno!=ERANGE && val1>=0.0) // doesnot overflow and is a valid positive real number
					{
						if(val1 < 0.1) clp->l=0.1;
						else clp->l=val1;
						i++;i++;continue;
	
					}
					else
					{
						fprintf(stderr, "Error: value of lambda should be a positive real number\n");
						exit(1);
					
					}
					
				}
				else
				{
			
					fprintf(stderr, "Error: value of lambda is missing.\n");
					DisplayErrorMessage();
					exit(1);
				}
			}
			else
			{
			
				fprintf(stderr, "Error: value of lambda is missing.\n");
				DisplayErrorMessage();
				exit(1);
			}				
		}
			
		else if(strcmp(argv[i],"-mu")==0 )
		{ 
			
			if(i+1<argc)
			{  
				if( strcmp(argv[i+1],"-lambda")!=0 && strcmp(argv[i+1],"-B") !=0 &&  strcmp(argv[i+1],"-r") !=0&& strcmp(argv[i+1],"-P") !=0 && strcmp(argv[i+1],"-n") !=0 && strcmp(argv[i+1],"-t") !=0)
				 {
					val1=strtof(argv[i+1], &ptr);
					val2=strtof(ptr, NULL);
					
					if(val2==0.0 && errno!=ERANGE && val1>=0.0) // doesnot overflow and is a valid number
					{
						if(val1 < 0.1) clp->mu=0.1;
						else clp->mu=val1;
						i=i+2;continue;
					}
					else
					{
						fprintf(stderr, "Error: value of mu should be a positive real number\n");
						exit(1);
					} 
				}
				else
				{
					fprintf(stderr, "Error: value of mu is missing.\n");
					DisplayErrorMessage();
					exit(1);
				}
					
			}
		
			else
			{
				fprintf(stderr, "Error: value of mu is missing.\n");
				DisplayErrorMessage();
				exit(1);
			}
			
		}
		else if(strcmp(argv[i],"-r")==0 )
		{ 
			
			if(i+1<argc)
			{  
				if((strcmp(argv[i+1],"-lambda")!=0 && strcmp(argv[i+1],"-mu") !=0 &&  strcmp(argv[i+1],"-B") !=0&& strcmp(argv[i+1],"-P") !=0 && strcmp(argv[i+1],"-n") !=0 && strcmp(argv[i+1],"-t") !=0))
				{
					val1=strtof(argv[i+1], &ptr);
					val2=strtof(ptr, NULL);
					if(val2==0.0 && errno!=ERANGE && val1>=0.0) // doesnot overflow and is a valid number
					{
						if(val1<0.1) clp->r=0.1;
						else clp->r=val1;
						i=i+2;continue;

					}
					else
					{
						fprintf(stderr, "Error: value of r should be a positive real number\n");
					
						exit(1);
				
					}
			
				}
		
				else
				{
					fprintf(stderr, "Error: value of r is missing.\n");
					DisplayErrorMessage();
					exit(1);
				}
			
			}
			else
			{
				fprintf(stderr, "Error: value of r is missing.\n");
					DisplayErrorMessage();
				exit(1);
			}
		}

		else if(strcmp(argv[i],"-B")==0 )
		{ 
			
			if(i+1<argc)
			{  
				if((strcmp(argv[i+1],"-lambda")!=0 && strcmp(argv[i+1],"-mu") !=0 &&  strcmp(argv[i+1],"-r") !=0&& strcmp(argv[i+1],"-P") !=0 && strcmp(argv[i+1],"-n") !=0 && strcmp(argv[i+1],"-t") !=0))
				{
					p=0;
					while(argv[i+1][p]!='\0')
					{if(!isdigit(argv[i+1][p]))
					{
						fprintf(stderr, "Error: wrong value for B. It should be a positive integer with max value of 2147483647\n");
						exit(1);
					}
					p++;
					}
					val1=strtol(argv[i+1], &ptr, 10);
					val2=strtol(ptr, NULL, 10);
					if(val2==0 && errno!=ERANGE && val1>=0) // doesnot overflow and is a valid positive integer
					{	clp->b=val1;  i=i+2;continue;}
					else
					{
						fprintf(stderr, "Error: value of B should be a positive integer\n");
						exit(1);
						
					}
			
				}
		
				else
				{
					fprintf(stderr, "Error: value of B is missing.\n");
				
						DisplayErrorMessage();
					exit(1);
				}
			
			}
			else
			{
				fprintf(stderr, "Error: value of B is missing.\n");
					DisplayErrorMessage();
				exit(1);
			}
		}

		else if(strcmp(argv[i],"-P")==0 )
		{ 
			
			if(i+1<argc)
			{  
				if((strcmp(argv[i+1],"-lambda")!=0 && strcmp(argv[i+1],"-mu") !=0 &&  strcmp(argv[i+1],"-r") !=0&& strcmp(argv[i+1],"-B") !=0 && strcmp(argv[i+1],"-n") !=0 && strcmp(argv[i+1],"-t") !=0))
				{
					p=0;
					while(argv[i+1][p]!='\0')
					{if(!isdigit(argv[i+1][p]))
					{
						fprintf(stderr, "Error: wrong value for P. It should be a positive integer with max value of 2147483647\n");
						exit(1);
					}
					p++;
					}
					val1=strtol(argv[i+1], &ptr,10);
					val2=strtol(ptr, NULL,10);
					if(val2==0 && errno!=ERANGE  && val1>=0) // doesnot overflow and is a valid number
					{	clp->p=val1; i=i+2;continue; }
					else
					{
						fprintf(stderr, "Error: value of P should be a positive integer. It should be a positive integer with max value of 2147483647\n");
						exit(1);
					}
				}
		
				else
				{
					fprintf(stderr, "Error: value of P is missing.\n");
						DisplayErrorMessage();
					exit(1);
				}
			
			}
			else
			{
				fprintf(stderr, "Error: value of P is missing.\n");
				DisplayErrorMessage();
				exit(1);
			}
		}

		else if(strcmp(argv[i],"-n")==0 )
		{ 
			
			if(i+1<argc)
			{  
				if((strcmp(argv[i+1],"-lambda")!=0 && strcmp(argv[i+1],"-mu") !=0 &&  strcmp(argv[i+1],"-r") !=0&& strcmp(argv[i+1],"-B") !=0 && strcmp(argv[i+1],"-P") !=0 && strcmp(argv[i+1],"-t") !=0))
				{	p=0;
					while(argv[i+1][p]!='\0')
					{if(!isdigit(argv[i+1][p]))
					{
						fprintf(stderr, "Error: wrong value for n. It should be a positive integer with max value of 2147483647\n");
						exit(1);
					}
					p++;
					}
					val1=strtol(argv[i+1], &ptr,10);
					val2=strtol(ptr, NULL,10);
					fprintf(stdout, "%f\n", val2);
					if(val2==0 && errno!=ERANGE  && val1>=0 ) // doesnot overflow and is a valid number
					{ clp->num=val1; i=i+2;continue; }
					else
					{	fprintf(stderr, "Error: value of n should be a positive integer. It should be a positive integer with max value of 2147483647\n");
						exit(1);
					}
				}
				else
				{
					fprintf(stderr, "Error: value of n is missing.\n");
					DisplayErrorMessage();
					exit(1);
				}
			
			}
			else
			{
				fprintf(stderr, "Error: value of n is missing.\n");
				DisplayErrorMessage();
				exit(1);
			}
		}

					
		else if(strcmp(argv[i],"-t")==0 )
		{ 
			
			if(i+1<argc)
			{ 
				if((strcmp(argv[i+1],"-lambda")!=0 && strcmp(argv[i+1],"-mu") !=0 &&  strcmp(argv[i+1],"-r") !=0 && strcmp(argv[i+1],"-B") !=0 && strcmp(argv[i+1],"-P") !=0 && strcmp(argv[i+1],"-n") !=0))
				{
					clp->t=(char*)malloc(sizeof(char)*( strlen(argv[i+1])));
					strncpy(clp->t,argv[i+1], strlen(argv[i+1]));
					i=i+2;continue; 
				}
				else
				{
					fprintf(stderr, "Error: value of t missing\n");
					DisplayErrorMessage();
					exit(1);
				}
			}	
			else
			{
				fprintf(stderr, "Error: value of t is missing.\n");
				DisplayErrorMessage();
				exit(1);
			}
			
		}
			else
			{
				fprintf(stdout,"Wrong command line parameter.\n");
				DisplayErrorMessage();
				exit(1);
			}
	
	}
	
   
}

