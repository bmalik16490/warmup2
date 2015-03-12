/* 
* Author: Bhavya Malik
*/


extern void initialiseVariables();
extern int GetNoOfPacketsFromFile(char *);
extern int readFromFile();
extern void getTimeInMs(long long int);
extern void GetCommandLineArgs(int , char** );

// Structure to store commandline arguments
typedef struct  cmdLineParam
{

	float l;
	float mu; 
	float r;
	unsigned long long int b;
	unsigned long long int p;
	unsigned long long int num;
	char *t;

} CommandLineParam;

//structure to store packet data
typedef struct packetStruct 
{
        unsigned long long int packetNum;
        unsigned long long int p;
        float interArrivalTime;
        double serviceRate;
        struct timeval q1ArrivalTime;
	struct timeval q1DepartTime;
        struct timeval q2ArrivalTime;
        struct timeval q2DepartTime;
        //struct timeval finalDeptTime;
	struct timeval sysArrivalTime;
	struct timeval sArrivalTime;
	struct timeval systemDepartureTime;

}Packet;

extern FILE *globalFilePtr;
extern CommandLineParam* clp;
extern unsigned long long int lineCount;

