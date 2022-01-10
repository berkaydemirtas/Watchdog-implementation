/**
 * @file process.cpp
 *  Compile: g++ process.cpp -o process
 * @brief this files contains the code for process. Processes just prints expected things when created and receving signal except SIGTERM.
 * It's main function takes 2 arguments.
 * @param  ProcessOutput  holds the path of the process processOutput file
 * @param ID holds the ID of the process(P1, P2 etc.)
 *
 *   @author Berkay Demirtaş , 2017400234 (CMPE student in Bogazici University)
 *   @date January 2021
    */
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <signal.h>
#include <stdlib.h>


using namespace std;

using namespace std;
string processOutput;
string id;

void sighandler(int);

/**
 * @brief Takes arguments, connects all signals to signal handler and sleep.
 */

int main (int argc, char *argv[]) {
    signal(SIGHUP, sighandler);
    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    signal(SIGILL, sighandler);
    signal(SIGTRAP, sighandler);
    signal(SIGFPE, sighandler);
    signal(SIGSEGV, sighandler);
    signal(SIGXCPU, sighandler);

    processOutput=argv[1];
    id=argv[2];
    ofstream outfile;
    outfile.open(processOutput,ios_base::app);
    outfile << "P"+id+" is waiting for a signal "<< endl;
    outfile.close();

    while(1) {
        sleep(1);
    }
}

/**
 * @brief If the give signum is 15 then program exit(), else just prints the signum.
 */
void sighandler(int signum) {

    signal(SIGHUP, sighandler);
    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    signal(SIGILL, sighandler);
    signal(SIGTRAP, sighandler);
    signal(SIGFPE, sighandler);
    signal(SIGSEGV, sighandler);
    signal(SIGXCPU, sighandler);
    ofstream outfile;
    //   sleep(1);
    if(signum!=15) {
        outfile.open(processOutput,ios_base::app);
        outfile << "P" + id + " recieved signal " << to_string(signum) << endl;
        outfile.close();
    }
    if(signum==15) {
        outfile.open(processOutput,ios_base::app);
        outfile << "P" + id + " recieved signal " << to_string(signum) <<", terminating gracefully"<< endl;
        outfile.close();
        exit(1);
    }
}
