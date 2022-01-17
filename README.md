# JobSystem

[![Windows](https://github.com/Chaphlagical/JobSystem/actions/workflows/windows.yml/badge.svg)](https://github.com/Chaphlagical/JobSystem/actions/workflows/windows.yml)

A simple threading framework

## Build

```cmake
mkdir build
cd build
cmake ..
cmake --build ./
```

## Usage

### Job Graph

```c++
// 1 -> 5
// 1 -> 6
// 2 -> 4
// 2 -> 5
// 3 -> 6
// 4 -> 5
// 6 -> 5
// 5 -> 7
// 5 -> 8
// 5 -> 9
// 5 -> 10

std::cout << "Job Graph Test" << std::endl;

// Declare a job graph
JobGraph job_graph;

std::array<std::thread::id, 10> thread_ids;

// Declare some nodes
JobNode node1([&thread_ids]() { thread_ids[0] = test_func(); });
JobNode node2([&thread_ids]() { thread_ids[1] = test_func(); });
JobNode node3([&thread_ids]() { thread_ids[2] = test_func(); });
JobNode node4([&thread_ids]() { thread_ids[3] = test_func(); });
JobNode node5([&thread_ids]() { thread_ids[4] = test_func(); });
JobNode node6([&thread_ids]() { thread_ids[5] = test_func(); });
JobNode node7([&thread_ids]() { thread_ids[6] = test_func(); });
JobNode node8([&thread_ids]() { thread_ids[7] = test_func(); });
JobNode node9([&thread_ids]() { thread_ids[8] = test_func(); });
JobNode node10([&thread_ids]() { thread_ids[9] = test_func(); });

// Adding nodes to the graph
job_graph.addNode(&node1)
    .addNode(&node2)
    .addNode(&node3)
    .addNode(&node4)
    .addNode(&node5)
    .addNode(&node6)
    .addNode(&node7)
    .addNode(&node8)
    .addNode(&node9)
    .addNode(&node10);

// Setting topology
node1.percede(&node5);
node1.percede(&node6);
node2.percede(&node4);
node2.percede(&node5);
node3.percede(&node6);
node4.percede(&node5);
node6.percede(&node5);
node5.percede(&node7);
node5.percede(&node8);
node5.percede(&node9);
node5.percede(&node10);

// Compilation will check DAG and topology sort
bool result = job_graph.compile();

// Run the job graph
JobHandle job_graph_handle;
JobSystem::execute(job_graph_handle, job_graph);

// Wait for the result
JobSystem::wait(job_graph_handle);

for (uint32_t i = 0; i < 10; i++)
{
    std::cout << "node " << i << " on thread " << thread_ids[i] << std::endl;
}
```

Output:

```
Job Graph Test
node 0 on thread 29312
node 1 on thread 5232
node 2 on thread 43608
node 3 on thread 20008
node 4 on thread 38280
node 5 on thread 18476
node 6 on thread 39048
node 7 on thread 41500
node 8 on thread 37216
node 9 on thread 40176
```

### Future Pattern

```c++
std::cout << "Future Pattern Test" << std::endl;

JobHandle future_handle;

// Store the results
std::vector<std::future<std::thread::id>> futures;

for (uint32_t i = 0; i < 10; i++)
{
    // Adding tasks, saving result
    futures.emplace_back(JobSystem::execute(future_handle, test_func));
}

// Waiting for the result, you can also do other things
while (!futures.empty())
{
    for (auto iter = futures.begin(); iter != futures.end(); )
    {
        // Get the result
        if (iter->wait_for(std::chrono::seconds(0)) == std::future_status::ready)
        {
            std::cout << "Run on thread "<< iter->get() << std::endl;
            iter = futures.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}
```

Output:

```
Future Pattern Test
Run on thread 31684
Run on thread 35240
Run on thread 5232
Run on thread 30988
Run on thread 44020
Run on thread 39852
Run on thread 25048
Run on thread 31500
Run on thread 42340
Run on thread 39860
```

### Dispatch

```c++
std::cout << "Dispatch Test" << std::endl;

JobHandle dispatch_handle;

// We will fill a 10 length array
uint32_t data[100];

JobSystem::dispatch(dispatch_handle, 100, 10, [&data](uint32_t group_id) {
    for (uint32_t i = 0; i < 10; i++)
    {
        data[group_id * 10 + i] = group_id;
    }
});

JobSystem::wait(dispatch_handle);

for (uint32_t i = 0; i < 100; i++)
{
    std::cout << data[i] << " ";
}
```

Output:

```
Dispatch Test
0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3 3 3 3 3 3 3 3 3 3 4 4 4 4 4 4 4 4 4 4 5 5 5 5 5 5 5 5 5 5 6 6 6 6 6 6 6 6 6 6 7 7 7 7 7 7 7 7 7 7 8 8 8 8 8 8 8 8 8 8 9 9 9 9 9 9 9 9 9 9
```

