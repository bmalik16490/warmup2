
*******
*BUILD*
*******
Comments: to build the files type:
		make
	
	   to clean type:
		make clean 
	
	   to run the executable type:
		./warmup2	
		    
  		    or

		pass command line arguments:
		./warmup2 [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]

		where lambda, mu and r are positive real values and P, B and num are positive integers with maximum value of 2147483647 and tsfile is the filename from where input is read.

*********
*GRADING*
*********
Basic running of the code: 100/100 pts

missing/incomplete required section(s) in README file: none
	submitted binary file: none
	cannot compile: n/a
	segmentation faults: no comments
	compiler warnings: none
	make clean: cleans all .o files and executable
	separate compilation: the files have been separately compiled
	using busy wait: no
	handling of commandline arguments: all cases have been handled
	1) -n: should be a positive integer of a given max value or else throws error
	2) -lambda: should be a positive real value else throws error
	3) -mu: should be a positive real value else throws error
	4) -r: should be a positive real value else throws error	5) -B : should be a positive integer of a given max value or else throws error
	6) -P: should be a positive integer of a given max value or else throws error
	trace output: 
	1) regular packets: proper
	2) dropped packets: proper 1 line
	3) removed packets: proper 1 line
	4) token arrival : proper shows dropped and not dropped tokens
	statistics output: 
	1) inter arrival time : proper
	2) service time:proper
	3)number of customers in Q1:proper
	4) number of customers in Q2: almost proper
	5) number of customers in at server: proper
	6) time in system:proper
	7) standard deviation for time in system:proper
	8) drop porbability: proper
	output bad format: none
	output wrong precision for statistics: none
	large service time test: no comments
	large inter-arrival time test:  no comments
	tiny inter-arrival time test: no comments
	large total number of customers test: passes for all positive integers till 2147483647
	larger total number of customers with high arrival rate test: no comments
	dropped token test: no comments
	cannot handle <CNTRL+C> at all: it can handle
	can handle <CNTRL+C> but statistics are way off: no comments
	not using condition variables or do some kind of busy wait: not busy waiting
	synchronization check: used mutex
	deadlocks: none found so far
	bad commandline: prints error for all malformed command lines

	


******
*BUGS*
******

Comments: No bugs

*******
*Other*
*******
Comments on design decisions: I have created three threads one for Packet Arrival, one for Token Deposit and one for Server. I have also created another thread for handling CNTRL+C signal. I am cancelling this thread in main() so that it does not become a Zombie.

I have used thread cancellation to cancel token deposit thread and the packet arrival thread when CNTRL+C is pressed.
	
Also, since the emulation parameters print number of packets to arrive, if the input tsfile is specified and the first line is malformed, the program exits.

Comments on deviation from spec: None



 


