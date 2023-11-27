
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

void insertRQ(std::vector<Process> &jq,
              std::vector<Process> &rq,
              int curr_time)
{
    std::cout << "inserting into rq" << std::endl;
    while (jq.front().arrival_time < curr_time)
    {
        rq.push_back(jq[0]);
        jq.erase(jq.begin());
        if (jq.size() == 0)
        {
            break;
        }
    }
}

void simulate_rr(
    int64_t quantum,
    int64_t max_seq_len,
    std::vector<Process> &processes,
    std::vector<int> &seq)
{
    seq.clear();
    std::vector<Process> jq = std::vector<Process>();
    std::vector<Process> rq = std::vector<Process>();
    std::vector<int> remainingTime = std::vector<int>();
    int curr_time = 0;
    std::cout << "Afer inintializing variables\n";
    for (auto &p : processes)
    {
        // remainingTime.insert(p.id, p.burst);
        if (p.arrival_time <= 0)
        {
            rq.push_back(p);
        }
        else
        {
            jq.push_back(p);
        }
    }
    std::cout << "After adding processes to queues\n";
    while (jq.size() > 0 || rq.size() > 0)
    {
        std::cout << "In the while loop\n";
        if (rq.size() > 0 && jq.size() > 0)
        {
            std::cout << "Processes in both queues\n";
            if (remainingTime[rq.front().id] > quantum)
            {
                curr_time += quantum;
                insertRQ(jq, rq, curr_time);
                rq.push_back(rq[0]);
                rq.erase(rq.begin());
            }
            else
            {
                curr_time += remainingTime[rq.front().id];
                insertRQ(jq, rq, curr_time);
                rq.erase(rq.begin());
            }
        }
        else if (jq.size() == 0)
        {
            std::cout << "Processes in rq only\n";
            if (remainingTime[rq.front().id] > quantum)
            {
                curr_time += quantum;
                rq.push_back(rq[0]);
                rq.erase(rq.begin());
            }
            else
            {
                curr_time += remainingTime[rq[0].id];
                rq.erase(rq.begin());
            }
        }
        else
        {
            std::cout << "Processes in jq only\n";
            curr_time = jq.front().arrival_time;
            insertRQ(jq, rq, curr_time);
        }
    }
}
