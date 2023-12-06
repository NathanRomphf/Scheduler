/*
/// this is the only file you should modify and submit for grading

#include "scheduler.h"
#include <iostream>

// this is the function you should implement
//
// simulate_rr() implements a Round-Robin scheduling simulator
// input:
//   quantum = time slice
//   max_seq_len = maximum length of the reported executing sequence
//   processes[] = list of process with populated IDs, arrivals, and bursts
// output:
//   seq[] - will contain the compressed execution sequence
//         - idle CPU will be denoted by -1
//         - other entries will be from processes[].id
//         - sequence will be compressed, i.e. no repeated consecutive numbers
//         - sequence will be trimmed to contain only first max_seq_len entries
//   processes[]
//         - adjust finish_time and start_time for each process
//         - do not adjust other fields
//

void startTime(std::vector<Process> &processes, int64_t ct, int64_t pid)
{
    if (processes[pid].start_time == -1) // If the process has not started
    {
        processes[pid].start_time = ct; // Set start time equal to current time
    }
}

void sequence(std::vector<int> &seq, std::vector<Process> &p, int64_t msl)
{
    if ((seq.size() == 0 || seq[seq.size() - 1] != p[0].id) && (seq.size() < msl)) //If the sequence number is not the same as the previous process in the sequence
                                                                                    //and the sequence size is less than its max length
    {
        seq.push_back(p[0].id); //Append the current proceess to the sequence
    }
    return;
}

void moveQueues(std::vector<Process> &p1, std::vector<Process> &p2)
{
    p1.push_back(p2[0]); //Add the element from the second vector to the first
    p2.erase(p2.begin()); //Remove the element that moved to the first queue from the second queue
}

void simulate_rr(
    int64_t quantum,
    int64_t max_seq_len,
    std::vector<Process> &processes,
    std::vector<int> &seq)
{
    seq.clear(); //Clear the current sequence
    std::vector<Process> jq = std::vector<Process>(); //Create a vector of processes representing the job queue
    std::vector<Process> rq = std::vector<Process>(); //Create a vector of processes representing the ready queue
    std::vector<int64_t> remainingTime = std::vector<int64_t>(); //Create an integer vector to store the processes remaining times
    int64_t curr_time = 0; //Set the current time to 0
    //int64_t prevSeqSize = 0;
    //bool manyq = false;
    //int64_t largeQ = quantum * 1000;
    for (auto &p : processes) //For each process
    {
        remainingTime.push_back(p.burst); //Set the remaining time for the process to the burst time
        if (p.arrival_time <= 0) //If the process arrives at time 0
        {
            rq.push_back(p); //Add the process to the ready queue
        }
        else
        {
            jq.push_back(p); //Add the process to the job queue
        }
    }
    while (jq.size() > 0 || rq.size() > 0) //While there exists a process in either queue
    {
        if (jq.size() == 0 || ((jq[0].arrival_time > (curr_time + largeQ * rq.size())) && rq.size() != 0))
        {
            manyq = true;
            for (int i = 0; i < rq.size(); i++)
            {
                startTime(processes, curr_time + i * quantum, rq[i].id);
                if (remainingTime[rq[i].id] <= largeQ)
                {
                    manyq = false;
                }
            }
            if (manyq)
            {
                for (Process p : rq)
                {
                    sequence(seq, rq, max_seq_len);
                    remainingTime[p.id] -= largeQ;
                    curr_time += largeQ;
                }
            }
            manyq = false;
        }
        if (rq.size() > 0 && jq.size() > 0) //If their are processes in both queues
        {
            sequence(seq, rq, max_seq_len); //Attempt to add the first process in the ready queue to the sequence
            startTime(processes, curr_time, rq[0].id); //Check if this process had its start time recorded
            if (remainingTime[rq[0].id] > quantum) //If the remaining bust time is longer than the quantum
            {
                curr_time += quantum; //Increment the current time by the quantum
                remainingTime[rq[0].id] -= quantum; //Subtrack the quantum from the remaining burst time of the process
                while (jq.front().arrival_time < curr_time) //While there are processes that arrived during the execution
                {
                    moveQueues(rq, jq); //Add the process that arrived to the ready queue
                    if (jq.size() == 0) //If the job queue is empty
                    {
                        break; //Exit the loop
                    }
                }
                moveQueues(rq, rq); //Move the element which just executed a quantum to the back of the queue
                while (jq.front().arrival_time == curr_time) //While their are processes that arrived at the same time that the execution finished
                {
                    moveQueues(rq, jq); //Move the process from the job queue to the ready queue
                    if (jq.size() == 0) //if the job queue is empty
                    {
                        break; //Exit the loop
                    }
                }
            }
            else //If the process will finish during the quantum
            {
                curr_time += remainingTime[rq[0].id]; //Add the time needed for the process to finish to the current time
                remainingTime[rq[0].id] = 0; //Set the remaining time of this process to 0 indicating it is completed
                processes[rq[0].id].finish_time = curr_time; //Set the finish time of this process to the current time
                rq.erase(rq.begin()); //Remove the finished process from the ready queue
                while (jq.front().arrival_time <= curr_time) //While there are processes that finished during or at the same time as the process finished
                {
                    moveQueues(rq, jq); //Move the process from the job queue to the ready queue
                    if (jq.size() == 0) //If the job queue is empty
                    {
                        break; //exit the loop
                    }
                }
            }
        }
        else if (jq.size() == 0) //If there are no elements in the job queue
        {
            sequence(seq, rq, max_seq_len); //Attempt to add the first process in the ready queue to the sequence
            startTime(processes, curr_time, rq[0].id); //Check to see if the start time for the process is recorded
            if (remainingTime[rq.front().id] > quantum) //If the process will take longer than the quantum to complete
            {
                curr_time += quantum; //Add the quantum to the current time
                remainingTime[rq[0].id] -= quantum; //Subtract the quantum from the remaining time of the process
                moveQueues(rq, rq); //Move the first element in the rq to the end of the rq
            }
            else
            {
                curr_time += remainingTime[rq[0].id]; //add the remaining time of the process to the current time
                remainingTime[rq[0].id] = 0; //Indicate that the process has completed
                processes[rq[0].id].finish_time = curr_time; //Set the processes finish time to the current time
                rq.erase(rq.begin()); //Remove the finished process from the ready queue
            }
        }
        else //No processes in the ready queue
        {
            curr_time = jq.front().arrival_time; //Set the current time to the arrival time of the next process in the job queue
            seq.push_back(-1); //Insert -1 to the sequence indicating no process executed
            while (jq[0].arrival_time <= curr_time) //While their exists processes in the job queue where the arrival time is less than the current time
            {
                moveQueues(rq, jq); //Move the process to the ready queue
                if (jq.size() == 0) //If the job queue is empty
                {
                    break; //Exit the loop
                }
            }
        }
    }
}
*/