/*! \mainpage Cmpe322 watchdog project
 * I am Berkay and 21 years old. I study Computer Engineering in Bogazici University.
 *
 * This projects enables us to create simple processes that are
 * watched by watchdog. I used pipes and signal handling in this project for the first time. So it was
 * very improving for me. I think one of the most challenging tasks in that projects is using sleep(). Because
 * of the scheduling mechanism of the computers, outputs can change time to time. Another challenging task was debugging.
 * During debugging, I learned many unix command. (kill, ps , ps aux etc.). At the end of the project, I have better understanding
 * about computer scheduling, using pipes and signal handling.
 */

/**
 * @file watchdog.cpp
 * * Compile: g++ watchdog.cpp -o watchdog
* ./watchdog processNum processOut WatchdogOut
 * @brief this files contains the code for watchdog. Watchdog creates expected number of processes and check if
 * any of the processes die. If any of them is dead, then create them according to rules. This program takes some
 * argumants from command line.
 * @param  numberOfProcesses  holds the path of the process watchdog file
 * @param processOutput holds the path of the process output file
   @param watchdogOutput holds the path of the watchdog output file

    This was a very improving project for me. I used pipe and signal handling for the first time.
    Detailed description follows here.
    @author Berkay Demirtaş , 2017400234 (CMPE student in Bogazici University)
    @date January 2021
 */
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string>
#include <fstream>
#include <map>
#include <cstdlib> // for exit function
#include <wait.h>

using namespace std;

struct timespec delta = {0 /*secs*/, 300000000 /*nanosecs*/}; //0.3 sec
char arr2[30];
string watchOut;
string processOutput;
int processNum;
void signal_handler(int signum);
void killAllProcesses();
void createAllProcesses();
map<int, int> processIDs;
map<int, int> PIDs;

/**
 *
 *
 */

/**
 * @brief main method reads input from command line, clears output files , create pipe and with the createAllProcesses() function creates
 * all processes. There is a while(1) loop in this function and it is used to handle the case any of the processes terminate. In this case,
 * if the dead process is "head" process , all processes are terminated by calling killAllProcesses() function and createAllProcesses(). If it
 * is not the head , then only dead proccess created again without calling any function. In addition to that, this process can handle SIGTERM signal
 * with signal_handler function.
 * @var watchOut It is a global variable and holds the path of the process watchdog file
 * @var processOutput It is a global variable and holds the path of the process output file
 * @var processNum holds number of processes
 *  @var processIDs holds child process (pid(key)-numOfProcess(value))
 *  @var PIDs holds child process (pid(value)-numOfProcess(key))
 *  @var processNum holds number of processes
 *  @var arr[2] is a char array and used to fill pipe.
 * the watchdog process waits until any of the child processes die. If the dead process is P1,
 * then kills all processes and recreate them. If it is not, then just creates the dead process again.
 * If the process killed by watchdog, then it does not make any of them.
 * @var stopped- the pid of child process that exit()
 * @var dead - pid of created process.
 */
int main(int argc, char *argv[]) {



    signal(SIGTERM,signal_handler);
    processNum = stoi(argv[1]) ;
    processOutput=argv[2];
    string watchdogOutput = argv[3];
    watchOut=watchdogOutput;

    ofstream ofs;
    ofs.open(processOutput, std::ofstream::out | std::ofstream::trunc);
    ofs.close();

    ofstream ofs2;
    ofs.open(watchdogOutput, std::ofstream::out | std::ofstream::trunc);
    ofs.close();
/**
 * @brief main method reads input from command line, clears output files , create pipe and with the createAllProcesses() function creates
 * all processes. There is a while(1) loop in this function and it is used to handle the case any of the processes terminate. In this case,
 * if the dead process is "head" process , all processes are terminated by calling killAllProcesses() function and createAllProcesses(). If it
 * is not the head , then only dead proccess created again without calling any function. In addition to that, this process can handle SIGTERM signal
 * with signal_handler function.
 * @var watchOut It is a global variable and holds the path of the process watchdog file
 * @var processOutput It is a global variable and holds the path of the process output file
 * @var processNum holds number of processes
 *  @var processIDs holds child process (pid(key)-numOfProcess(value))
 *  @var PIDs holds child process (pid(value)-numOfProcess(key))
 *  @var processNum holds number of processes
 *  @var arr[2] is a char array and used to fill pipe.
 * the watchdog process waits until any of the child processes die. If the dead process is P1,
 * then kills all processes and recreate them. If it is not, then just creates the dead process again.
 * If the process killed by watchdog, then it does not make any of them.
 * @var stopped- the pid of child process that exit()
 * @var dead - pid of created process.
 */

    int unnamedPipe;
    char *myfifo = (char *) "/tmp/myfifo";
    mkfifo(myfifo, 0666);
    unnamedPipe = open(myfifo, O_WRONLY);
    string p0 = to_string(getpid());
    string s1 = "P0 " + p0;
    strcpy(arr2, s1.c_str());
    write(unnamedPipe, arr2, 30);
    createAllProcesses();

    while(1) {
        int stopped = wait(NULL);
        int a=processIDs.count(stopped);
        if(a==0)
            continue;
        if (processIDs[stopped] != 1) { //not head
            int dead = fork();
            if(dead==0) {
                processIDs[getpid()]=processIDs[stopped];
                string processID= to_string(processIDs[getpid()]);
                char a[30];
                char b[30];
                strcpy(a,processID.c_str());
                strcpy(b,processOutput.c_str());
                execl("./process","process" ,argv[2],a,NULL);
            }
            else {
                PIDs[processIDs[stopped]]=dead;
                ofstream outfile;
                outfile.open(watchdogOutput, ios_base::app);
                string st=to_string(processIDs[stopped]);
                outfile <<"P"+ st+" is killed" << endl;
                outfile.close();
                outfile.open(watchdogOutput, ios_base::app);
                outfile <<"Restarting P"+ st << endl;
                outfile.close();
                usleep(500);
                int unnamedPipe;
                char *myfifo = (char *) "/tmp/myfifo";
                mkfifo(myfifo, 0666);
                unnamedPipe = open(myfifo, O_WRONLY);
                p0 = to_string(dead);
                string process = "P" + to_string(processIDs[stopped]);
                string Px = process + " " + p0;
                strcpy(arr2, Px.c_str());
                write(unnamedPipe, arr2, 30);
                string x = process + " is started and it has a pid of " + to_string(dead);
                outfile.open(watchdogOutput, ios_base::app);
                outfile << x << endl;
                outfile.close();
                processIDs.erase(stopped);
            }
        } if(processIDs[stopped]==1){
            ofstream outfile;
            outfile.open(watchOut, ios_base::app);
            outfile<<"P1 is killed, all processes must be killed"<<endl;
            outfile.close();
            processIDs.erase(stopped);
            killAllProcesses();
            outfile.open(watchOut, ios_base::app);
            outfile<<"Restarting all processes"<<endl;
            outfile.close();
            createAllProcesses();
        }
    }
}

/**
 * @brief a simple signal handler. If the given signal is SIGTERM then kills all children and
 * exit() watchdog
 */

void signal_handler(int signum) {
    sleep(1);
    if (signum == 15) {
        killAllProcesses();
        ofstream outfile;
        outfile.open(watchOut, ios_base::app);
        outfile << "Watchdog is terminating gracefully";
        outfile.close();
        exit(0);
    }
}

/**
 * @brief It contains a for loop that turns processNum times and creates a child.
 * @param childPid is the pid of created child. If this value is 0 it is a child,
 * else it is a watchdog. If it is a child then it exec() process.cpp. If it is watchdog
 * then it opens pipe and put the informations to the pipe about the new created child.
 */
void createAllProcesses(){
    for (int i = 0; i < processNum; i++) // loop will run n times (n=5)
    {
        ofstream outfile;
        outfile.open(watchOut, ios_base::app);
        int childPid = fork();
        if (childPid == 0) {
            string processID= to_string(i+1);
            char a[30];
            char b[30];
            strcpy(a,processID.c_str());
            strcpy(b,processOutput.c_str());
            execl("./process","process", b,a);
        } else {
            processIDs[childPid]=i+1;
            PIDs[i+1]=childPid;
            string p0 = to_string(childPid);
            string process = "P" + to_string(i + 1);
            string Px = process + " " + p0;
            strcpy(arr2, Px.c_str());
            int unnamedPipe;
            char *myfifo = (char *) "/tmp/myfifo";
            mkfifo(myfifo, 0666);
            unnamedPipe = open(myfifo, O_WRONLY);
            write(unnamedPipe, arr2, 30);
            string x = process + " is started and it has a pid of " + to_string(childPid);
            outfile << x << endl;
            sleep(1);
            outfile.close();
        }
    }
}

/**
 * @brief kills all processes start from P1 and clears the maps.
 */
void killAllProcesses(){
    for (int i=1; i<=processNum; i++){
        processIDs.erase(PIDs[i]);
        kill(PIDs[i] , SIGTERM);
        nanosleep(&delta, &delta);
    }
}

