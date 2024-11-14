#include <bits/stdc++.h>
#include <fstream>
#include <deque>
#include <algorithm>  
#include <iomanip>
using namespace std;
ofstream outputfile("output.txt");

class Process{
    public:
    string ID;
    int AT;
    int Burst;
    int CT;
    int TAT;
    int WT;
    int Priority;
    int OriginalBurst;
    Process(string ID, int AT, int Burst, int Priority){
        this->ID = ID;
        this->AT = AT;
        this->OriginalBurst = Burst;
        this->Burst = Burst;
        this->Priority = Priority;
    }
};

void Display() {
    ifstream inputfile("output.txt");
    string line;
    while (getline(inputfile, line)) {
        cout << line << endl;
    }
    inputfile.close();
}

string extractValue(const string& line) {
    int colonPos = line.find(':');
    if (colonPos != string::npos && colonPos + 2 < line.size()) {
        return line.substr(colonPos + 2); 
    }
    return ""; 
}

bool lambdaForIDsort(Process p1, Process p2){
    if (p1.ID[1] < p2.ID[1]) return 1;
    else return 0;
}
bool lambdaForFCFS(Process p1, Process p2){
    if ((p1.AT < p2.AT) || (p1.AT == p2.AT && p1.ID[1] < p2.ID[1])) return 1;
    else return 0;
}
bool lambdaForSPN(Process p1, Process p2){
    if ((p1.Burst < p2.Burst) || (p1.Burst == p2.Burst && p1.AT < p2.AT) || (p1.Burst == p2.Burst && p1.AT == p2.AT && p1.ID[1] < p2.ID[1])){
        return 1;
    } else {
        return 0;
    }
}
bool lambdaForPrioritySort(Process p1, Process p2){
    if ((p1.Priority < p2.Priority )|| (p1.Priority == p2.Priority && p1.AT < p2.AT) || (p1.Priority == p2.Priority && p1.AT == p2.AT && p1.ID[1] < p2.ID[1])){
        return 1;
    } else {
        return 0;
    }
}
bool lambdaForPRR(Process p1, Process p2){
    if((p1.AT < p2.AT) || (p1.AT == p2.AT && p1.ID[1] < p2.ID[1])) return 1;
    else return 0;
}

void displayTable(vector<Process> processes){
    outputfile << "\n" << "Process Turnaround Time Waiting Time" << endl;
    sort(processes.begin(), processes.end(), lambdaForIDsort);
    for (int i = 0; i < processes.size(); i++){
        if(processes[i].TAT < 10){
            outputfile << processes[i].ID << "      " << processes[i].TAT << "               " << processes[i].WT << endl;
        } else {
            outputfile << processes[i].ID << "      " << processes[i].TAT << "              " << processes[i].WT << endl;
        }
    }
    outputfile << "\n";
}

vector<float> FCFS(vector<Process> processes, int Disp){
    outputfile << "FCFS:" << "\n";
    sort(processes.begin(), processes.end(), lambdaForFCFS);
    float averageTAT = 0, averageWT = 0;
    float currentTime  = processes[0].AT + Disp;
    float acctualCurrentTime = processes[0].AT;
    for (int i =0; i< processes.size(); i++){
        if (processes[i].AT > acctualCurrentTime){
            currentTime = processes[i].AT + Disp;
            acctualCurrentTime = processes[i].AT;
        }
        outputfile << "T" << currentTime << ": " << processes[i].ID << "(" << processes[i].Priority << ")" << "\n";

        processes[i].CT = currentTime + processes[i].Burst;
        currentTime = processes[i].CT + Disp;
        processes[i].TAT = processes[i].CT - processes[i].AT;
        processes[i].WT = processes[i].TAT - processes[i].Burst;
        acctualCurrentTime = processes[i].CT;

        
    }
    for (int i = 0; i < processes.size(); i++){
        averageTAT += processes[i].TAT;
        averageWT += processes[i].WT;
    }
    displayTable(processes);
    averageTAT /= processes.size();
    averageWT /= processes.size();

    vector<float> resulT;
    resulT.push_back(averageTAT);
    resulT.push_back(averageWT);
    return resulT;
}

vector<float> SPN(vector<Process> processes, int Disp){
    outputfile << "SPN:" << "\n";
    sort(processes.begin(), processes.end(), lambdaForSPN);
    int minAT = INT_MAX;
    for (int i = 0; i < processes.size(); i++){
        minAT = min(minAT, processes[i].AT);
    }
    int currentTime = minAT + Disp;
    int actualCurrentTime = currentTime;
    float averageTAT = 0, averageWT = 0;

    while (true){
        int check;                             
        for (int i = 0; i < processes.size(); i++){
            check = 0;
            if (processes[i].AT <= actualCurrentTime){
                check = 1;
                outputfile << "T" << currentTime << ": " << processes[i].ID << "(" << processes[i].Priority << ")" << "\n";
                processes[i].CT = currentTime + processes[i].Burst;
                actualCurrentTime = processes[i].CT;
                currentTime = processes[i].CT + Disp;
                processes[i].TAT = processes[i].CT - processes[i].AT;
                processes[i].WT = processes[i].TAT - processes[i].Burst;
                processes[i].AT = INT_MAX;
                break;
            }
        }
        if (check == 0) {
            int l = 0;
            sort(processes.begin(), processes.end(), lambdaForFCFS);
            for (int i = 0; i < processes.size(); i++){
                if (processes[i].AT != INT_MAX){
                    l = 1;
                    currentTime = processes[i].AT + Disp;
                    actualCurrentTime = processes[i].AT;
                    sort(processes.begin(), processes.end(), lambdaForSPN);
                    break;
                }
            }
            if (l == 0)
                break;
        }
    }
    for (int i = 0; i < processes.size(); i++){
        averageTAT += processes[i].TAT;
        averageWT += processes[i].WT;
    }
    displayTable(processes);
    averageTAT /= processes.size();
    averageWT /= processes.size();
    vector<float> result;
    result.push_back(averageTAT);
    result.push_back(averageWT);
    return result;
}

vector<float> PP(vector<Process> processes, int Disp){
    outputfile << "PP:" << "\n";
    sort(processes.begin(), processes.end(), lambdaForPrioritySort);
    int currentTime = Disp;
    int actualCurrentTime = 0; //current - Disp
    float averageTAT = 0, averageWT = 0;

    int minAT = INT_MAX;
    for (int i = 0; i < processes.size(); i++){
        minAT = min(minAT, processes[i].AT);
    }
    if (currentTime < minAT) currentTime = minAT+Disp;
    actualCurrentTime = currentTime - Disp;
    int index = -1;

    while (true){
        int check = 0;
        for (int i = 0; i < processes.size(); i++){
            if (processes[i].AT <= actualCurrentTime){
                if(index!=i){
                    outputfile << "T" << currentTime << ": " << processes[i].ID << "(" << processes[i].Priority << ")" << "\n";
                } else if (index == i){
                    currentTime -= Disp;
                }
                index = i;
                check = 1;
                processes[i].Burst -= 1;
                if (processes[i].Burst == 0){
                    processes[i].CT = currentTime + 1;
                    processes[i].TAT = processes[i].CT - processes[i].AT;
                    processes[i].WT = processes[i].TAT - processes[i].OriginalBurst;
                    processes[i].AT = INT_MAX;
                }
                actualCurrentTime = currentTime + 1;
                currentTime = actualCurrentTime + Disp;
                break;
            }
        }
        if (check == 0) {
            int l = 0;
            sort(processes.begin(), processes.end(), lambdaForPRR); // sorted on based of arrival time
            for (int i = 0; i < processes.size(); i++){
                if (processes[i].AT != INT_MAX){
                    l = 1;
                    currentTime = processes[i].AT + Disp;
                    actualCurrentTime = processes[i].AT;
                    sort(processes.begin(), processes.end(), lambdaForPrioritySort);
                    break;
                }
            }
            if (l == 0)
                break;
        };
    }
    for (int i = 0; i < processes.size(); i++){
        averageTAT += processes[i].TAT;
        averageWT += processes[i].WT;
    }
     
    displayTable(processes);

    averageTAT /= processes.size();
    averageWT /= processes.size();
    vector<float> result;
    result.push_back(averageTAT);
    result.push_back(averageWT);
    return result;
}

vector<float> PRR(vector<Process> processes, int dispatch_time) {
    outputfile << "PRR:\n";
    
    sort(processes.begin(), processes.end(), lambdaForPRR);

    int total_turnaround = 0, total_waiting = 0;
    vector<Process*> ready_queue;
    int current_time = processes[0].AT + dispatch_time;
    int actual_time = processes[0].AT + dispatch_time;
    int current_process_index = -1;
    int processed_count = 0;


    // Push all processes that have arrived at time 0 into the queue
    for (int i = 0; i < processes.size(); ++i) {
        if (processes[i].AT <= processes[0].AT) {
            ready_queue.push_back(&processes[i]);
            processed_count++;
        }
    }

    while (!ready_queue.empty()) {
        Process* active_process = ready_queue.front();
        ready_queue.erase(ready_queue.begin());

        int time_slice = (active_process->Priority <= 2) ? 4 : 2;
        int execution_time = min(time_slice, active_process->Burst);

        if (current_process_index != active_process->ID[1] - '0') {
            outputfile << "T" << current_time << ": " << active_process->ID << "(" << active_process->Priority << ")\n";
        }
        current_process_index = active_process->ID[1] - '0';

        active_process->Burst -= execution_time;
        current_time += execution_time;

        // Adding new processes to the ready queue
        for (int i = processed_count; i < processes.size(); ++i) {
            if (processes[i].AT <= current_time) {
                ready_queue.push_back(&processes[i]);
                processed_count++;
            }
        }

        if (active_process->Burst <= 0) {
            active_process->CT = current_time;
            active_process->TAT = active_process->CT - active_process->AT;
            active_process->WT = active_process->TAT - active_process->OriginalBurst;
            total_turnaround += active_process->TAT;
            total_waiting += active_process->WT;
            active_process->AT = INT_MAX;  // INT_MAX ,Becauses process is completed
        } else {
            ready_queue.push_back(active_process);
        }

        actual_time = current_time;
        current_time += dispatch_time;
        if(ready_queue.empty() && processed_count < processes.size()) {
            for (int i = processed_count; i < processes.size(); ++i) {
                if (processes[i].AT != INT_MAX) {
                    ready_queue.push_back(&processes[i]);
                    processed_count++;
                    if (processes[i].AT > actual_time) {
                        current_time = processes[i].AT + dispatch_time;
                        actual_time = processes[i].AT;
                    }
                    break;
                }
            }

        }
    }

    displayTable(processes);
    vector<float> result;

    result.push_back((float)total_turnaround / processes.size());
    result.push_back((float)total_waiting / processes.size());
    return result;
}

void Summary(vector<float> fcfs, vector<float> spn, vector<float> pp, vector<float> prr){
    outputfile << "Summary" << "\n";
    outputfile << "Algorithm       Average Turnaround Time   Average Waiting Time" << "\n";
    if(fcfs[0] < 10){
        outputfile << "FCFS            " << fixed << setprecision(2) << fcfs[0] << "                      " << fixed << setprecision(2) << fcfs[1] << "\n";

    }else {

        outputfile << "FCFS            " << fixed << setprecision(2) << fcfs[0]<< "                     " << fixed << setprecision(2) << fcfs[1] << "\n";
    }
    if(spn[0] < 10){
        outputfile << "SPN             " << fixed << setprecision(2) << spn[0] << "                      " << fixed << setprecision(2) << spn[1] << "\n";
    } else {
        outputfile << "SPN             " << fixed << setprecision(2) << spn[0] << "                     " << fixed << setprecision(2) << spn[1] << "\n";
    }
    if(pp[0] < 10){
        outputfile << "PP              " << fixed << setprecision(2) << pp[0] << "                      " << fixed << setprecision(2) << pp[1] << "\n";
    } else {
        outputfile << "PP              " << fixed << setprecision(2) << pp[0] << "                     " << fixed << setprecision(2) << pp[1] << "\n";
    }
    if(prr[0] < 10){
        outputfile << "PRR             " << fixed << setprecision(2) << prr[0] << "                      " << fixed << setprecision(2) << prr[1] << "\n";
    } else {
        outputfile << "PRR             " << fixed << setprecision(2) << prr[0] << "                     " << fixed << setprecision(2) << prr[1] << "\n";
    }

}

int main(){
    string fileName;
    cout << "Enter the file name(carefully): ";
    cin >> fileName;
    fileName = fileName + ".txt";
    ifstream myfile(fileName);
    string line;
    vector<string> pid;
    vector<int> arrival;
    vector<int> burst;
    vector<int> priority;
    int Disp = 0;

    while (getline(myfile, line)) {
        if (line.find("DISP:") != string::npos) {
            Disp = stoi(extractValue(line)); 
            break;
        }
    }

    while (getline(myfile, line)) {
        if (line.find("ID:") != string::npos) {
            pid.push_back(extractValue(line));

            getline(myfile, line);
            arrival.push_back(stoi(extractValue(line))); 

            getline(myfile, line);
            burst.push_back(stoi(extractValue(line)));

            getline(myfile, line); 
            priority.push_back(stoi(extractValue(line)));
        }
    }
    myfile.close();

    vector<Process> processes;
    for(int i = 0; i < pid.size(); i++) {
        Process p(pid[i], arrival[i], burst[i], priority[i]);
        processes.push_back(p);
    }

    vector<float> fcfs = FCFS(processes, Disp);
    vector<float> spn = SPN(processes, Disp);
    vector<float> pp = PP(processes, Disp);
    vector<float> prr = PRR(processes, Disp);

    Summary(fcfs, spn, pp, prr);

      
    outputfile.close();
    Display();

    return 0;
}