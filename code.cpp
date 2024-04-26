#include <bits/stdc++.h>
#include <random>
#include <omp.h>

using namespace std;

// task structure
struct Task {
    int id;
    int processing_time;
    vector<pair<int, int>> successors; // (task_id, communication_cost)
};

//adjacency list representation of the DAG
vector<Task> tasks = {
    {0, 2, {{1, 4}, {2, 1}, {3, 1}, {6, 20}, {4, 1}}},
    {1, 3, {{5, 1}, {6, 5}, {7, 5}}},
    {2, 3, {{6, 5}, {7, 1}}},
    {3, 4, {{7, 1}}},
    {4, 5, {{7, 10}}},
    {5, 4, {{8, 10}}},
    {6, 4, {{8, 10}}},
    {7, 4, {{8, 10}}},
    {8, 1, {}}
};

const int NUM_TASKS = tasks.size();
const int NUM_PROCESSORS = 2;
const int NUM_ITERATIONS = 100;
const int mxIndividualTime = 5; // subject to change wrt input graph

int calculateMakespan(const vector<int>& task_assignments) {
    vector<int> processor_times(NUM_PROCESSORS, 0);
    vector<int> task_finish_times(NUM_TASKS, 0);

    for (int i = 0; i < NUM_TASKS; ++i) {
        int task_id = task_assignments[i];
        int processor_id = i % NUM_PROCESSORS;

        // Calculate task start time based on parents finish times
        int start_time = 0;
        for (auto& successor : tasks[task_id].successors) {
            int succ_id = successor.first;
            int comm_cost = successor.second;
            start_time = max(start_time, task_finish_times[succ_id] + comm_cost);
        }
        
        // Update task finish time
        task_finish_times[task_id] = start_time + tasks[task_id].processing_time;

        // Update processor time
        processor_times[processor_id] = max(processor_times[processor_id], task_finish_times[task_id]);
    }
    return max( mxIndividualTime, *min_element(processor_times.begin(), processor_times.end()));
}

// Glowworm Swarm Optimization
vector<int> gsoTaskScheduling() {
    
    // Initialize glowworm positions randomly
    vector<vector<int>> glowworms(NUM_ITERATIONS, vector<int>(NUM_TASKS));
    
    #pragma omp parallel for
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        // Randomly shuffle task assignments
        vector<int> task_order(NUM_TASKS);
        for (int j = 0; j < NUM_TASKS; ++j) {
            task_order[j] = j;
        }
        random_shuffle(task_order.begin(), task_order.end());

        // Assign shuffled tasks to glowworms
        for (int j = 0; j < NUM_TASKS; ++j) {
            glowworms[i][j] = task_order[j];
        }
    }

    // Perform optimization iterations
    vector<int> best_solution(NUM_TASKS);
    int best_makespan = numeric_limits<int>::max();
    for (int iter = 0; iter < NUM_ITERATIONS; ++iter) {
        // Evaluate current solution
        int current_makespan = calculateMakespan(glowworms[iter]);

        // Update best solution if needed
        if (current_makespan < best_makespan) {
            best_makespan = current_makespan;
            best_solution = glowworms[iter];
        }

        // randomly shuffle glowworms
        random_shuffle(glowworms[iter].begin(), glowworms[iter].end());
    }

    return best_solution;
}

int main() {
    
    vector<int> best_schedule = gsoTaskScheduling();

    cout << "Best Task Schedule:\n";
    for (int i = 0; i < NUM_TASKS; ++i) {
        cout << "Task " << tasks[best_schedule[i]].id << " -> Processor " << i % NUM_PROCESSORS << '\n';
    }
    
    cout << "Makespan: " << calculateMakespan(best_schedule) << '\n';

    return 0;
}
