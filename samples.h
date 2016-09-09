bool task_fault_check(double factor);
bool check_busyP(int detectIdx, int nTask);
bool check_deadline(bool* first_task_flag , int ntask, double deadline, double end, int task_type, int task_id, double tick_per_second);
void priority_assignment(attri* tasks, int nTask);
int check_running_task(int* suspendedTask);
