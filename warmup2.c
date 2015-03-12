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
#include<unistd.h>
#include "my402list.h"
#include "generalFunctions.h"


unsigned long long int lineCount;

unsigned long long int packetsArrivedFromFile;
pthread_t packetThread;
pthread_t tokenThread;
pthread_t serverThread;
pthread_t signalHandlingThread;
sigset_t set;
int flag;
FILE *globalFilePtr;
unsigned long long int totalPacketsArrived;
unsigned long long int totalTokensArrived;
unsigned long long int droppedPackets;
unsigned long long int droppedTokens;
unsigned long long int tokensInBucket;
unsigned long long int droppedPackets;
pthread_mutex_t mutex;
int ccNotSignaled;
int noPacketFlag;
int  noPacketsFlag;
struct timeval emulationStartTime;
struct timeval emulationEndTime;
pthread_cond_t q2NotEmpty;
My402List *q1;
My402List *q2;
CommandLineParam* clp;
double avgPacketInterArrivalTime;
double avgPacketServiceTime;
double totalTimeInQ1;
double totalTimeInQ2;
double totalTimeAtS;
double totalTimeInSystem;
double squareOfTTS;
unsigned long long int packetsServiced;
double e1, e2;   
double tokenDropProb;
double packetDropProb;
  
void* tokenDepositMethod(void *x)
{ 
	struct timeval t1;
    	struct timeval t2;
	struct timeval q2ATime;
	struct timeval q1DTime;
	My402ListElem* first;
	Packet* firstPacketCopy;	
	long int sTime;	
   	long long int diff; 
	
	
	for(tokensInBucket=0; ccNotSignaled==1 ; )
	{
		gettimeofday(&t2, NULL);
		
		if(totalPacketsArrived == clp->num && My402ListEmpty(q1))
		{		pthread_mutex_lock(&mutex);
				//fprintf(stdout,"######\n");
				pthread_cond_signal(&q2NotEmpty);
				//fprintf(stdout,"After ######\n");
				pthread_mutex_unlock(&mutex); 
				break;	//stop generating tokens and signal the 
		}

		if(totalTokensArrived==0) //for first token
			t1=emulationStartTime;
 		diff=(t1.tv_usec + 1000000 * t1.tv_sec)- (t2.tv_usec + 1000000 * t2.tv_sec);
		
		if(clp->r< 0.1)
		clp->r = 0.1;
			sTime=(1000000/clp->r)+diff;
		
		if(sTime<0)
			sTime=0;
		
		//sleep
		usleep(sTime);
	
		//thread wakes up
		gettimeofday(&t1, NULL);
			
		
	//acquire lock
	//fprintf(stdout, "before locking mutex in token\n");
	pthread_mutex_lock(&mutex);
	//fprintf(stdout, "after locking mutex in token\n");
				
		if((tokensInBucket) < clp->b && totalPacketsArrived <= clp->num)
		{	
			if(totalPacketsArrived == clp->num && My402ListEmpty(q1))
			{		pthread_mutex_unlock(&mutex); 
				break;	//stop generating tokens and signal the 
			}			
			//add token to bucket
			totalTokensArrived++;
			diff=((t1.tv_usec + 1000000 * t1.tv_sec)- (emulationStartTime.tv_usec + 1000000 * emulationStartTime.tv_sec));
		        getTimeInMs(diff);
		
			tokensInBucket++;
			fprintf(stdout,"ms: token t%lld arrives, token bucket has now %lld tokens\n",  totalTokensArrived, tokensInBucket);
		}
		else
		{
			//drop token
			if(totalPacketsArrived < clp->num)
			{ 
				totalTokensArrived++;
				droppedTokens++;
			diff=((t1.tv_usec + 1000000 * t1.tv_sec)- (emulationStartTime.tv_usec + 1000000 * emulationStartTime.tv_sec));
			getTimeInMs(diff);
			 fprintf(stdout, "ms: token t%lld arrived, dropped\n", totalTokensArrived) ;
	               }
							
		
		}	
			//checks if it can move first packet from Q1 to Q2
			//get first packet from Q1
					
					
		if(My402ListLength(q1)>0)
		{	
			if(tokensInBucket >= ((Packet*)My402ListFirst(q1)->obj)->p)						
			{	
				first=(My402ListElem*)malloc(sizeof(My402ListElem));
				firstPacketCopy=(Packet*)malloc(sizeof(Packet));
				first=My402ListFirst(q1);
				firstPacketCopy=(Packet*)(first->obj);
							
							//unlink
				My402ListUnlink(q1, first);
				tokensInBucket= tokensInBucket - firstPacketCopy->p;		

				gettimeofday(&q1DTime, NULL);
				firstPacketCopy->q1DepartTime=q1DTime;
				diff=((q1DTime.tv_usec + 1000000 * q1DTime.tv_sec)- (emulationStartTime.tv_usec + 1000000 * emulationStartTime.tv_sec));
				//decrement tokensInBucket count by number of tokens taken by packet
				getTimeInMs(diff);
				fprintf(stdout, "ms: p%lld leaves Q1, time in Q1 = %.3fms, token bucket now has %lld token\n",  firstPacketCopy->packetNum, ((float)(((firstPacketCopy->q1DepartTime).tv_usec + 1000000 * (firstPacketCopy->q1DepartTime).tv_sec) - ((firstPacketCopy->q1ArrivalTime).tv_usec + 1000000 * (firstPacketCopy->q1ArrivalTime).tv_sec)))/1000.0, tokensInBucket );
				//packet enter Q2 so append it
				My402ListAppend(q2, (void*)(firstPacketCopy));
				//if Q2 is not empty signal
				if (My402ListLength(q2) == 1)
		                {	gettimeofday(&q2ATime, NULL);
				firstPacketCopy->q2ArrivalTime=q2ATime;
				
				diff=(((firstPacketCopy->q2ArrivalTime).tv_usec + 1000000 * (firstPacketCopy->q2ArrivalTime).tv_sec)- (emulationStartTime.tv_usec + 1000000 * emulationStartTime.tv_sec));
				getTimeInMs(diff);
			
				fprintf(stdout, "ms: p%lld enters Q2\n", firstPacketCopy->packetNum);
				
                			pthread_cond_signal(&q2NotEmpty); //to wake server
					fprintf(stdout,"hi\n");
                        	}	
else
{fprintf(stdout,"hi2\n");

gettimeofday(&q2ATime, NULL);
				firstPacketCopy->q2ArrivalTime=q2ATime;
				
				diff=(((firstPacketCopy->q2ArrivalTime).tv_usec + 1000000 * (firstPacketCopy->q2ArrivalTime).tv_sec)- (emulationStartTime.tv_usec + 1000000 * emulationStartTime.tv_sec));
				getTimeInMs(diff);
			
				fprintf(stdout, "ms: p%lld enters Q2\n", firstPacketCopy->packetNum);
				
}
			}		
		}
		fprintf(stdout,"hi3\n");
	//fprintf(stdout, "after break out of while loop\n");			
	pthread_mutex_unlock(&mutex);	
	//fprintf(stdout, "after unlocking mutex in token\n");
		fprintf(stdout,"h4\n");
	}
	//totalTokensArrived--;
	
return NULL;
	
}


void *packetArrivalMethod(void *x)
{
	
	Packet *pkt=NULL;
	struct timeval t1;
    	struct timeval t2;
	struct timeval t3;
	struct timeval q1Time;
	struct timeval q2ATime;
	struct timeval q1DTime;
	long int sTime;	
   	long long int diff;
	My402ListElem* first;
	Packet* second;
	int result;
		for(totalPacketsArrived=0; totalPacketsArrived < clp->num  && ccNotSignaled==1;)
		{
			
			gettimeofday(&t2, NULL);
			if(totalPacketsArrived==0)
				t1=emulationStartTime;
			
			if(clp->t!=NULL)
			{
				//read fom file
				result= readFromFile();
				
				if(result==0)
				{
					exit(1);
				}

			}

			diff=(t1.tv_usec + 1000000 * t1.tv_sec)- (t2.tv_usec + 1000000 * t2.tv_sec);
			
			if(clp->l<0.1 && clp->t==NULL)
				clp->l=0.1;
			
			//sleep for 1/lambda +t1-t2

			sTime=(1000000 /clp->l)+diff;
			if(totalPacketsArrived==0 && clp->t!=NULL)
				sTime=(1000000/clp->l);
			if(sTime <0)
				sTime=0;
			
			usleep(sTime);

 			//thread wakes up
			gettimeofday(&t3, NULL); //t3
			totalPacketsArrived++;
			pkt=(Packet*)malloc(sizeof(Packet)); //creates a packet
			memset(pkt, 0, sizeof (pkt));//initialize all packet values to 0
			
			pkt->packetNum=totalPacketsArrived;
			pkt->p=clp->p;
			pkt->sysArrivalTime=t3;
			//set packet's actual inter arrival time
			pkt->interArrivalTime= ((float)((t3.tv_usec + 1000000 * t3.tv_sec) - (t1.tv_usec + 1000000 * t1.tv_sec)))/1000.0; //in millisec
			avgPacketInterArrivalTime+=((float)((t3.tv_usec + 1000000 * t3.tv_sec) - (t1.tv_usec + 1000000 * t1.tv_sec)))/1000.0;
			t1=t3;
			pkt->serviceRate= clp->mu;

			//now check if tokens required by the packet are <= bucket capacity or not
			diff=((t3.tv_usec + 1000000 * t3.tv_sec)- (emulationStartTime.tv_usec + 1000000 * emulationStartTime.tv_sec));
			if(pkt->p > clp->b)
			{
				//drop packet
				
				getTimeInMs(diff),			
				fprintf(stdout, "ms: p%lld arrives, needs %lld tokens, inter-arrival time = %.3fms, dropped\n", pkt->packetNum, pkt->p, pkt->interArrivalTime);
				droppedPackets++;
                		
			}
			else
			{

				//acquire lock
				//fprintf(stdout, "before  locking mutex in packet\n");
				 pthread_mutex_lock(&mutex);
        			//fprintf(stdout, "after locking mutex in packet\n");
					//add packet to Q1
					getTimeInMs(diff);
					fprintf(stdout, "ms: p%lld arrives, needs %lld tokens, inter-arrival time = %.3fms\n",  pkt->packetNum, pkt->p, pkt->interArrivalTime);		
					//append pkt to q1
					gettimeofday(&q1Time, NULL);
					pkt->q1ArrivalTime=q1Time;	
					
					My402ListAppend(q1, (void*)pkt);
					diff=((q1Time.tv_usec + 1000000 * q1Time.tv_sec)- (emulationStartTime.tv_usec + 1000000 * emulationStartTime.tv_sec));
					getTimeInMs(diff);
					fprintf(stdout, "ms: p%lld enters Q1\n",totalPacketsArrived);
					//get first packet from Q1
					
					if(My402ListLength(q1) >0 )
					{
						first=(My402ListElem*)malloc(sizeof(My402ListElem));
						first=My402ListFirst(q1);
						second=(Packet*)malloc(sizeof(Packet));
						second=(Packet*)(first->obj);
						if( tokensInBucket >= second->p)
						{	
							//dequeue packet from q1
							My402ListUnlink(q1, first);
							gettimeofday(&q1DTime, NULL);
							second->q1DepartTime=q1DTime;

							tokensInBucket= tokensInBucket - second->p;		

							
							diff=((q1DTime.tv_usec + 1000000 * q1DTime.tv_sec)- (emulationStartTime.tv_usec + 1000000 * emulationStartTime.tv_sec));
							//decrement tokensInBucket count by number of tokens taken by packet
							getTimeInMs(diff);
							fprintf(stdout, "ms: p%lld leaves Q1, time in Q1 = %.3fms, token bucket now has %lld token\n",  second->packetNum, ((float)(((second->q1DepartTime).tv_usec + 1000000 * (second->q1DepartTime).tv_sec) - ((second->q1ArrivalTime).tv_usec + 1000000 * (second->q1ArrivalTime).tv_sec)))/1000.0, tokensInBucket );
							gettimeofday(&q2ATime, NULL);
							second->q2ArrivalTime=q2ATime;
							diff=((q2ATime.tv_usec + 1000000 * q2ATime.tv_sec)- (emulationStartTime.tv_usec + 1000000 * emulationStartTime.tv_sec));
							//packet enter Q2 so append it
							My402ListAppend(q2, (void*)(second));
							getTimeInMs(diff);
							fprintf(stdout, "ms: p%lld enters Q2\n",  second->packetNum);
							
							//if Q2 is not empty broadcast
							if (My402ListLength(q2) == 1)
					                {	//fprintf(stdout,"in packet thread signal\n");
                        					pthread_cond_signal(&q2NotEmpty); //to wake server
								//fprintf(stdout,"in packet thread after signal\n");
                        				}	
			
							
						


						}
						
					}
					//fprintf(stdout, "before unlocking mutex in packet\n");

				pthread_mutex_unlock(&mutex);
				//fprintf(stdout, "after unlocking mutex in packet\n");
				
			}

		}

	
	return NULL;
}							


void * serverMethod(void * args)
{
    
    struct timeval t1;
    struct timeval t2;
    long long int diff;
   
    Packet *first = NULL;
     
	
    while (ccNotSignaled==1)
    {	fprintf(stdout,"yo\n");	
	pthread_mutex_lock(&mutex); 
	fprintf(stdout,"yo1\n");
		if(My402ListEmpty(q2))
			pthread_cond_wait(&q2NotEmpty, &mutex);
	fprintf(stdout,"yo2\n");
	   gettimeofday(&t1, NULL);
	       first = (Packet*) malloc(sizeof (Packet));
		first=(Packet*)(My402ListFirst(q2))->obj;               
		  
                    first->q2DepartTime = t1;
                    My402ListUnlink(q2, My402ListFirst(q2));
               
                    
		   diff=((t1.tv_usec + 1000000 * t1.tv_sec)- (emulationStartTime.tv_usec + 1000000 * emulationStartTime.tv_sec));
		    getTimeInMs(diff);
			fprintf(stdout,"ms: p%lld leaves Q2, time in Q2 = %.3fms\n",  first->packetNum, ((float)(((first->q2DepartTime).tv_usec + 1000000 * ( first->q2DepartTime).tv_sec) - (( first->q2ArrivalTime).tv_usec + 1000000 * ( first->q2ArrivalTime).tv_sec)))/1000.0);
			
		 
			//fprintf(stdout, "before unlocking mutex\n");
                   pthread_mutex_unlock(&mutex);
			//fprintf(stdout, "after unlocking mutex\n");
                   
		   gettimeofday(&t1, NULL);
		  first->sArrivalTime=t1;
		   diff=((t1.tv_usec + 1000000 * t1.tv_sec)- (emulationStartTime.tv_usec + 1000000 * emulationStartTime.tv_sec));
		    getTimeInMs(diff);
			
		   
		   fprintf(stdout, "ms: p%lld begin service at S, requesting %.7gms of service\n"
                                 , first->packetNum
                                , (1000/first->serviceRate));
                      

			//check for sleep time if 1/mu >10 sec use 10sec
			if(first->serviceRate < 0.1 && (clp->t==NULL) )
				 first->serviceRate = 0.1;
                   usleep(1000000 / first->serviceRate);
                        
                   gettimeofday(&t2, NULL);
                      
                   first->systemDepartureTime = t2;
		  diff=((t2.tv_usec + 1000000 * t2.tv_sec)- (emulationStartTime.tv_usec + 1000000 * emulationStartTime.tv_sec));
	           getTimeInMs(diff);
		    fprintf(stdout,"ms: p%lld departs from S, service time = %.3fms, time in system = %.3fms\n"
                              , first->packetNum, ((float)((( first->systemDepartureTime).tv_usec + 1000000 * ( first->systemDepartureTime).tv_sec) - (( first->sArrivalTime).tv_usec + 1000000 * ( first->sArrivalTime).tv_sec)))/1000.0, 
((float)((( first->systemDepartureTime).tv_usec + 1000000 * (first->systemDepartureTime).tv_sec) - ((first->sysArrivalTime).tv_usec + 1000000 * (first->sysArrivalTime).tv_sec)))/1000.0); 
		   packetsServiced++;
		     
		      diff=(((first->q1DepartTime).tv_usec + 1000000 * ( first->q1DepartTime).tv_sec)- (( first->q1ArrivalTime).tv_usec + 1000000 * (first->q1ArrivalTime).tv_sec));
                      totalTimeInQ1 += ((float)diff)/1000.0;
                        
                        diff=(((first->q2DepartTime).tv_usec + 1000000 * (first->q2DepartTime).tv_sec)- (( first->q2ArrivalTime).tv_usec + 1000000 * (first->q2ArrivalTime).tv_sec));
                      totalTimeInQ2 += ((float)diff)/1000.0;  
			
			diff=(((first->systemDepartureTime).tv_usec + 1000000 * (first->systemDepartureTime).tv_sec)- ((first->sArrivalTime).tv_usec + 1000000 * (first->sArrivalTime).tv_sec));
                      totalTimeAtS += ((float)diff)/1000.0;

			diff=((( first->systemDepartureTime).tv_usec + 1000000 * (first->systemDepartureTime).tv_sec)- ((first->sysArrivalTime).tv_usec + 1000000 * (first->sysArrivalTime).tv_sec));
                      totalTimeInSystem  += ((float)diff)/1000.0;
			
		       squareOfTTS += pow((double)(((float)diff)/1000.0), 2);
		        
	
	if ((totalPacketsArrived >= clp->num && My402ListLength(q2) == 0 && My402ListLength(q1) ==0 )|| ccNotSignaled==0)
           break;

     }
   
	
    return NULL;
}

void *signalHandler(void* x)
{
	//fprintf(stdout,"in handler");
	int sig;
		sigwait(&set, &sig);
		pthread_mutex_lock(&mutex); //lock global mutex
		ccNotSignaled=0;
				
		
	
			//cancel threads
		pthread_cancel(packetThread);
		pthread_cancel(tokenThread);
		
		//signal to server thread Q2 is not empty
		pthread_cond_signal(&q2NotEmpty);
		
		//unlock mutex
		pthread_mutex_unlock(&mutex);
		
		
		
	return NULL;



}
int main(int argc, char** argv)
{	
globalFilePtr =NULL;
int result, i=0,j=0;
int q1l=0, q2l=0;
signalHandlingThread=0;
packetThread=0;
tokenThread=0;
serverThread=0;
My402ListElem* first= NULL;
Packet* second=NULL;
struct timeval t;
long long int diffNew;
clp=(CommandLineParam*)malloc(sizeof(CommandLineParam));
long long int diff;
double sd=0.0, v=0.0, e1=0.0, e2=0.0;

droppedPackets=0;
packetsServiced=0;
totalPacketsArrived=0;
totalTokensArrived=0;
droppedPackets=0;
droppedTokens=0;
tokensInBucket=0;

clp->num=20;
clp->r=1.5;
clp->t=NULL;
clp->p=3;
clp->mu=0.35;
clp->b=10;
clp->l=0.5;
noPacketsFlag=1;
pthread_mutex_init(&mutex, NULL);
pthread_cond_init(&q2NotEmpty, NULL);
ccNotSignaled=1;
noPacketFlag=1;
q1=(My402List*)malloc(sizeof(My402List));
q2=(My402List*)malloc(sizeof(My402List));
flag=1;
avgPacketInterArrivalTime=0.0;
avgPacketServiceTime=0.0;
totalTimeInQ1=0.0;
totalTimeInQ2=0.0;
totalTimeAtS=0.0;
totalTimeInSystem=0.0;
squareOfTTS=0.0;
e1=0.0, e2=0.0;
tokenDropProb=0.0;
packetDropProb=0.0;
lineCount=0;
packetsArrivedFromFile=0;


 
if(argc >1)
{
//fprintf(stdout, "getting command line args\n");
GetCommandLineArgs(argc,argv);

}

if(clp->t!=NULL)
{
result=GetNoOfPacketsFromFile(clp->t);

if(result==0)
	exit(1);

}	

fprintf(stdout,"Emulation Parameters: \n");
fprintf(stdout,"\t number to arrive = %lld\n", clp->num);
if(clp->t == NULL)
{
	fprintf(stdout,"\t lambda = %.6g\n", clp->l);
	fprintf(stdout,"\t mu = %.6g\n", clp->mu);
}

fprintf(stdout,"\t r = %.6g\n", clp->r);
fprintf(stdout,"\t B = %lld\n", clp->b);

if(clp->t == NULL)
{ 
	fprintf(stdout,"\t P = %lld\n", clp->p);
}

if(clp->t != NULL)
{	fprintf(stdout,"\t tsfile = %s\n", clp->t);
}

//set signal details
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	pthread_sigmask(SIG_BLOCK, &set, NULL);
	pthread_create(&signalHandlingThread, NULL, signalHandler, NULL); //create thread for cntrl c handling
	

	//get time of day
	gettimeofday(&emulationStartTime, NULL);
    	
	fprintf(stdout,"00000000.000ms: emulation begins\n");
	//initialise Q1
	if(!My402ListInit(q1))
	{	
		fprintf(stderr, "Error: Cannot initialize Q1.\n");
		exit(1);					
	}	

	//initialise Q2
	if(!My402ListInit(q2))
	{	
		fprintf(stderr,"Error: Cannot initialize Q2.\n");
		exit(1);					
	}	

	//create three threads
	pthread_create(&tokenThread, NULL, tokenDepositMethod, NULL);
	pthread_create(&packetThread, NULL, packetArrivalMethod, NULL);
	pthread_create(&serverThread, NULL, serverMethod, NULL);
			
	//join		
	pthread_join(tokenThread, NULL);	
	pthread_join(packetThread, NULL);
	pthread_join(serverThread, NULL);

	q1l=My402ListLength(q1);
	q2l=My402ListLength(q2);
	
	for(i=0;i<q1l;i++)
 	{	first=(My402ListElem*)malloc(sizeof(My402ListElem));
		first=My402ListFirst(q1);
		second=(Packet*)malloc(sizeof(Packet));
		second=(Packet*)(first->obj);
		gettimeofday(&t,NULL);
		My402ListUnlink(q1,first);
		diffNew=((t.tv_usec + 1000000 * t.tv_sec)- (emulationStartTime.tv_usec + 1000000 * emulationStartTime.tv_sec));
		
		getTimeInMs(diffNew);
		fprintf(stdout, "ms: p%lld removed from q1\n",second->packetNum); 
		
	}
	for(j=0;j<q2l;j++)
 	{	first=(My402ListElem*)malloc(sizeof(My402ListElem));
		first=My402ListFirst(q2);
		second=(Packet*)malloc(sizeof(Packet));
		second=(Packet*)(first->obj);
		gettimeofday(&t,NULL);
		My402ListUnlink(q2,first);
		
		diffNew=((t.tv_usec + 1000000 * t.tv_sec)- (emulationStartTime.tv_usec + 1000000 * emulationStartTime.tv_sec));
		getTimeInMs(diffNew);
		fprintf(stdout, "ms: p%lld removed from q2\n",second->packetNum); 
			
	}
	pthread_cancel(signalHandlingThread);
	//print statisctics
	 gettimeofday(&emulationEndTime, NULL);
	 diff=((emulationEndTime.tv_usec + 1000000 * emulationEndTime.tv_sec)- (emulationStartTime.tv_usec + 1000000 * emulationStartTime.tv_sec));
getTimeInMs(diff); 
	fprintf(stdout, "ms: emulation ends\n");

   	 fprintf(stdout, "Statistics:\n");
    	if (totalPacketsArrived == 0)
        	fprintf(stdout, "\taverage packet inter-arrival time = N/A as no packets arrived.\n");
    	else
        	fprintf(stdout, "\taverage packet inter-arrival time = %.6gs\n", (avgPacketInterArrivalTime/totalPacketsArrived)/1000);
    	if (packetsServiced == 0)
        	fprintf(stdout, "\taverage packet service time = N/A as no packets were serviced.\n");
    	else
        	fprintf(stdout, "\taverage packet service time = %.6gs\n", (totalTimeAtS/packetsServiced)/1000);
	
    	fprintf(stdout, "\n\taverage number of packets in Q1 = %.6g\n", (totalTimeInQ1*1000)/diff );
    	fprintf(stdout, "\taverage number of packets in Q2 = %.6g\n", (totalTimeInQ2*1000)/diff);
    	fprintf(stdout, "\taverage number of packets at S = %.6g\n", (totalTimeAtS*1000)/diff);
    	if (packetsServiced == 0)
        	fprintf(stdout, "\n\taverage time a packet spent in system = N/A as no packets were serviced.\n");
    	else
        	fprintf(stdout, "\n\taverage time a packet spent in system = %.6gs\n", (totalTimeInSystem/packetsServiced)/1000);
    	if (packetsServiced == 0)
        	fprintf(stdout, "\tstandard deviation for time spent in system = N/A as no packets were serviced.\n");
    	else
        {	
		e1=pow((totalTimeInSystem/(1000*packetsServiced)),2);//sec2
		e2=(squareOfTTS/(1000000*packetsServiced)) ;//sec2
		v= e2 - e1;
		sd=sqrt(v);
		
		if(v<0.0)
			fprintf(stdout, "\tstandard deviation for time spent in system = 0\n");
		else
			fprintf(stdout, "\tstandard deviation for time spent in system = %.6g\n", sd);
    	
	}
	if (totalTokensArrived == 0)
    	    fprintf(stdout, "\n\ttoken drop probability = N/A as no tokens arrived.\n");
    	else
    	 { 
		
		   fprintf(stdout, "\n\ttoken drop probability = %.6g\n", (double) droppedTokens/totalTokensArrived);
	}	
    	if (totalPacketsArrived == 0)
    	    fprintf(stdout, "\tpacket drop probability = N/A as no packets arrived.\n");
    	else
    	    fprintf(stdout, "\tpacket drop probability = %.6g\n", (double) droppedPackets/totalPacketsArrived);


	//close file pointer
	if(globalFilePtr!=NULL)
		fclose(globalFilePtr);
	
	//free Q1 & Q2 & clp
	free(clp);
	free(q1);
	free(q2);	
return 0;
}


