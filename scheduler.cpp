
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
}

void startTime(std::vector<int> &seq, std::vector<Process> &processes, int ct, int pid)
{
    if (processes[pid].start_time == -1)
    {
        processes[pid].start_time = ct;
    }
}

void sequence(std::vector<int> &seq, std::vector<Process> &p)
{
    if (seq.size() == 0 || seq[seq.size() - 1] != p[0].id)
    {
        seq.push_back(p[0].id);
    }
    return;
}

void simulate_rr(
    int64_t quantum,
    int64_t max_seq_len,
    std::vector<Process> &processes,
    std::vector<int> &seq)
{
    std::cout << "Quantum: " << quantum << std::endl;
    seq.clear();
    std::vector<Process> jq = std::vector<Process>();
    std::vector<Process> rq = std::vector<Process>();
    std::vector<int> remainingTime = std::vector<int>();
    int curr_time = 0;
    for (auto &p : processes)
    {
        remainingTime.push_back(p.burst);
        if (p.arrival_time <= 0)
        {
            rq.push_back(p);
            seq.push_back(p.id);
        }
        else
        {
            jq.push_back(p);
        }
    }
    while (jq.size() > 0 || rq.size() > 0) //&& seq.size() < max_seq_len)
    {
        if (rq.size() > 0 && jq.size() > 0)
        {
            startTime(seq, processes, curr_time, rq[0].id);
            if (remainingTime[rq[0].id] > quantum)
            {
                curr_time += quantum;
                remainingTime[rq[0].id] -= quantum;
                while (jq.front().arrival_time < curr_time)
                {
                    rq.push_back(jq[0]);
                    sequence(seq, jq);
                    jq.erase(jq.begin());
                    if (jq.size() == 0)
                    {
                        break;
                    }
                }
                rq.push_back(rq[0]);
                sequence(seq, rq);
                rq.erase(rq.begin());
                while (jq.front().arrival_time == curr_time)
                {
                    rq.push_back(jq[0]);
                    sequence(seq, jq);
                    jq.erase(jq.begin());
                    if (jq.size() == 0)
                    {
                        break;
                    }
                }
            }
            else
            {
                curr_time += remainingTime[rq[0].id];
                remainingTime[rq[0].id] = 0;
                processes[rq[0].id].finish_time = curr_time;
                // insertRQ(jq, rq, curr_time);
                rq.erase(rq.begin());
                while (jq.front().arrival_time <= curr_time)
                {
                    rq.push_back(jq[0]);
                    sequence(seq, jq);
                    jq.erase(jq.begin());
                    if (jq.size() == 0)
                    {
                        break;
                    }
                }
            }
        }
        else if (jq.size() == 0)
        {
            startTime(seq, processes, curr_time, rq[0].id);
            if (remainingTime[rq.front().id] > quantum)
            {
                curr_time += quantum;
                remainingTime[rq[0].id] -= quantum;
                rq.push_back(rq[0]);
                sequence(seq, rq);
                rq.erase(rq.begin());
            }
            else
            {
                curr_time += remainingTime[rq[0].id];
                remainingTime[rq[0].id] = 0;
                processes[rq[0].id].finish_time = curr_time;
                rq.erase(rq.begin());
            }
        }
        else
        {
            curr_time = jq.front().arrival_time;
            seq.push_back(-1);
            // insertRQ(jq, rq, curr_time);
            while (jq[0].arrival_time <= curr_time)
            {
                rq.push_back(jq[0]);
                sequence(seq, jq);
                jq.erase(jq.begin());
                if (jq.size() == 0)
                {
                    break;
                }
            }
        }
    }
}
