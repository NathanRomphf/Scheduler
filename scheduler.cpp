
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
    if (processes[pid].start_time == -1)
    {
        processes[pid].start_time = ct;
    }
}

void sequence(std::vector<int> &seq, std::vector<Process> &p, int64_t msl)
{
    if ((seq.size() == 0 || seq[seq.size() - 1] != p[0].id) && (seq.size() < msl))
    {
        seq.push_back(p[0].id);
    }
    return;
}

void moveQueues(std::vector<Process> &p1, std::vector<Process> &p2, std::vector<int> &seq)
{
    p1.push_back(p2[0]);
    // sequence(seq, p2);
    p2.erase(p2.begin());
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
    std::vector<int64_t> remainingTime = std::vector<int64_t>();
    int64_t curr_time = 0;
    int64_t prevSeqSize = 0;
    bool manyq = false;
    int64_t largeQ = quantum * 1000;
    for (auto &p : processes)
    {
        remainingTime.push_back(p.burst);
        if (p.arrival_time <= 0)
        {
            rq.push_back(p);
        }
        else
        {
            jq.push_back(p);
        }
    }
    while (jq.size() > 0 || rq.size() > 0)
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
        if (rq.size() > 0 && jq.size() > 0)
        {
            sequence(seq, rq, max_seq_len);
            startTime(processes, curr_time, rq[0].id);
            if (remainingTime[rq[0].id] > quantum)
            {
                curr_time += quantum;
                remainingTime[rq[0].id] -= quantum;
                while (jq.front().arrival_time < curr_time)
                {
                    moveQueues(rq, jq, seq);
                    if (jq.size() == 0)
                    {
                        break;
                    }
                }
                moveQueues(rq, rq, seq);
                while (jq.front().arrival_time == curr_time)
                {
                    moveQueues(rq, jq, seq);
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
                    moveQueues(rq, jq, seq);
                    if (jq.size() == 0)
                    {
                        break;
                    }
                }
            }
        }
        else if (jq.size() == 0)
        {
            sequence(seq, rq, max_seq_len);
            startTime(processes, curr_time, rq[0].id);
            if (remainingTime[rq.front().id] > quantum)
            {

                curr_time += quantum;
                remainingTime[rq[0].id] -= quantum;
                moveQueues(rq, rq, seq);
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
                moveQueues(rq, jq, seq);
                if (jq.size() == 0)
                {
                    break;
                }
            }
        }
    }
}
