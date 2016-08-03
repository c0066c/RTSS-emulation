#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CEILING_POS(X) ((X-(int)(X)) > 0 ? (int)(X+1) : (int)(X))

#include "system.h"
#include "samples.h"
#include <rtems/test.h>
#include <rtems/status-checks.h>
#include <stdio.h>
#include <stdlib.h>

#define CEILING_POS(X) ((X-(int)(X)) > 0 ? (int)(X+1) : (int)(X))
#define CEILING_NEG(X) ((X-(int)(X)) < 0 ? (int)(X-1) : (int)(X))
#define CEILING(X) ( ((X) > 0) ? CEILING_POS(X) : CEILING_NEG(X) )
#define ABS(X)((X)>=0)? X : (X*-1)

int gcd(int a, int b)
{
	if(b == 0)
		return a;
	else
		return gcd(b, a%b);
}
int lcm(int a, int b)
{
	return a*b/gcd(a, b);
}

int LoopCountingForASec(void)
{
    uint32_t start, now;
    int loopInterLoop = 0, loopcount = 0;
    int loopcountpersec;
    start = rtems_clock_get_ticks_since_boot();
    rtems_interval    sec_ticks;
    sec_ticks = rtems_clock_get_ticks_per_second();

    while(1){
        loopInterLoop+=1;
        if(loopInterLoop > 500000){
            loopcount+=1;
            loopInterLoop = 0;
        }
        
        loopcountpersec = loopcount*500000 + loopInterLoop;
        now = rtems_clock_get_ticks_since_boot();
        if(now - start == sec_ticks){
                  //printf("LoopCount (500000) for one sec is %d", loopcount);
            break;
        }
    }
    
    loopcountpersec = loopcount*500000 + loopInterLoop;

    return loopcountpersec;
}

bool task_fault_check(double factor)
{
	int fault_rand;
	int cfec = factor/tick_per_second;
	double trial = fault_rate[refer_fault_rate]*100000*cfec;

  fault_rand = rand()%100000;
	  
  if(fault_rand < trial)
      return TRUE; 
  else
      return FALSE;
}

bool check_R_table(int detectIdx, int nTask)
{
	int sum = 0;
	int i = 0;
	OnlineDetectiveRTA(detectIdx, taskrunning_table, sp_dl_missed_table, d_t, tsk, nTask);
	//OnlineDetectiveK2U(detectIdx, taskrunning_table, sp_dl_missed_table, d_t, tsk, nTask);
	
	for(i=0; i<nTask; i++){		
      sum = sum + sp_dl_missed_table[i];
	}
	
	if(sum==0)
      return TRUE;
	else
      return FALSE;

}
bool check_busyP(int detectIdx, int nTask)
{
  //KHCHEN: fix the busyperiod prediction after discussing with Georg
  //TODO Syncronize the # of postponed jobs table.
  int x[11];

  for(i=0; i<nTask; i++)
  {
    //call the helper function in RMS manager to get the number of x
    x[i] = rtems_monotonic_Postponed_num(period_id[i]);
    if(x[i] < 0)
      printf("BUG: # of postponed jobs is not positive\n");
  }

	int sum = 0, i = 0, j=0;
	double busy = 0, Dn = 0, sumU=0, sumF=0;

	for(i=detectIdx; i<nTask; i++) //test from detectIdx to so-on, as detectIdx task is affected.
	{
      double carry_in = 0;
      if(tsk[i].task_type == 0) //hard task no needs to monitor
          continue;
      else{
          Dn = tsk[i].period;
          sumU=0;
          sumF=0;
          //virtual task k' Ck'=Gk
          for(j=0; j<=i; j++)//calculate all high priority tasks' properties + itself
          {
              sumU+=(tsk[j].normal_et/tsk[j].period);
              sumF+=(1-tsk[j].normal_et/tsk[j].period)*tsk[j].normal_et;
          }  
          /* KHCHEN 03.08.16
           * As there is no ready queue, to trace the number of executing jobs (normal or abnormal) 
           * or postponed jobs (which are only normal), we have to co-work with RMS manager.
           * If we only trace the number by using the application layer manner, the # of postponed jobs may not be precise in time.
           * That means, the moment we predicit the deadline misses may lack of the information of the already postponed jobs.
           */

          for(j=detectIdx; j<=i; j++) //X is the number of postponed jobs from RMS manager.
          {
              if(j == detectIdx)
              {
                  carry_in += (tsk[j].abnormal_et-tsk[j].normal_et);
              }
              else
              {
                  carry_in += x[j]*tsk[j].normal_et;
                  if(table_value[j]==){
                    carry_in += tsk[j].abnormal_et;
                  }
                  else if(table_value[j]==){
                    carry_in += tsk[j].normal_et;
                  }
                  if(tsk[j].tsk_type == 0 && x[j] > 0)
                      printf("BUG: hard task is postponed somehow\n");

              }
          }

          busy = (carry_in+sumF)/(1-sumU); //eq. 6
            
          if(busy > Dn) //if WCRT in the busy period is larger than the task deadline, mark it as suspecious.
              sp_dl_missed_table[i]=1;
      }
	}
	for(i=0; i<nTask; i++){
      sum = sum + sp_dl_missed_table[i];
	}
	if(sum == 0){
      return TRUE;
	}
	else
      return FALSE;

}

bool check_deadline(bool* first_task_flag , int nTask, double deadline, double end, int task_type, int task_id, double tick_per_second, bool* meet)
{
	int i = 0;
	int check_task_dl = 0;
	double sys_turn_healthy;

	if(sys_stop_flag != TRUE){	
		if(end <= deadline || (end > deadline && task_type == 1)){
			if(end > deadline && task_type == 1){
				//printf("Soft real-time Task %d has missed its deadline. \n", task_id+1);
				sp_dl_missed_table[task_id] = 1;
				*meet = FALSE;
			}
			else if(end <= deadline){
				sp_dl_missed_table[task_id] = 0;
				*meet = TRUE;
			}

			for(i=0; i < nTask; i++){
				check_task_dl += sp_dl_missed_table[i];
			}

			if(*first_task_flag == TRUE && task_running_flag == TRUE){
				if(sys_fault_flag == TRUE){
					sys_unhealthy_end = end;
					sys_unhealthy_duration = sys_unhealthy_end - sys_unhealthy_start;
					sys_unhealthy_total_duration += sys_unhealthy_duration;
					//printf("System is currently unhealthy with duration of %.6f second. \n", sys_unhealthy_duration);
					//printf("Total duration of system in unhealthy condition is %.3f second.\n\n",sys_unhealthy_total_duration);
		      
					if(check_task_dl == 0){
						sys_fault_flag = FALSE;   
						printf("System become healthy again after no other tasks is suspected to be missed its deadline.\n\n");   
					}
				}else if(sys_fault_flag == FALSE){
					sys_healthy_end = end;
					sys_healthy_duration = sys_healthy_end - sys_healthy_start;
					sys_healthy_total_duration += sys_healthy_duration;
					//printf("System is currently healthy with duration of %.6f second. \n", sys_healthy_duration);
					//printf("Total duration of system in healthy condition is %.3f second. \n\n", sys_healthy_total_duration);
				}
				task_running_flag = FALSE;
				*first_task_flag = FALSE;
			}else{
				if(sys_fault_flag == TRUE){

					if(check_task_dl == 0){
						sys_fault_flag = FALSE;
						sys_turn_healthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
						sys_healthy_start = sys_turn_healthy;
						sys_unhealthy_end = sys_turn_healthy;
						sys_unhealthy_duration = sys_unhealthy_end - sys_unhealthy_start;
						sys_unhealthy_total_duration += sys_unhealthy_duration;
						printf("System turned healthy in task %d at %.6f seconds.\n",task_id+1 , sys_turn_healthy);
						//printf("System was unhealthy with duration of %.3f second. \n\n", sys_unhealthy_duration);
			  		}
				}else if(sys_fault_flag == FALSE){
				}
			}
				return FALSE;
		}else if(end > deadline && task_type == 0){
			printf("Hard real-time Task %d has missed its deadline and whole system will be shut down. \n", task_id+1);

			if(sys_fault_flag == TRUE){
				sys_unhealthy_end = end;
				sys_unhealthy_duration = sys_unhealthy_end - sys_unhealthy_start;
				sys_unhealthy_total_duration += sys_unhealthy_duration;
				//printf("System is currently unhealthy with duration of %.6f second. \n", sys_unhealthy_duration);
				printf("Deadline is %lf and Task ends at %lf\n", deadline, end);
				//printf("\nTotal duration of system in healthy condition is %.3f second.\n", sys_healthy_total_duration);
				//printf("Total duration of system in unhealthy condition is %.3f second.\n\n",sys_unhealthy_total_duration);

			}else if(sys_fault_flag == FALSE){
				sys_healthy_end = end;
				sys_healthy_duration = sys_healthy_end - sys_healthy_start;
				sys_healthy_total_duration += sys_healthy_duration;
				//printf("System is currently healthy with duration of %.6f second. \n", sys_healthy_duration);
				printf("Deadline is %lf and Task ends at %lf\n", deadline, end);
				//printf("\nTotal duration of system in healthy condition is %.3f second.\n", sys_healthy_total_duration);
				//printf("Total duration of system in unhealthy condition is %.3f second.\n\n",sys_unhealthy_total_duration);
			}
			return TRUE;
		}
		printf("BUG:Exception in check_deadline\n");
		return FALSE;
	}else{
		printf("System shutting down, not necessary to check anymore. ");
		return FALSE;
	} 
}

double normalDemand(double R, attri* tasks, int curIdx)
{
	double sumDM=0;
	int i=0;
	for(i=0; i<curIdx; i++)
	{		
		sumDM += tasks[i].normal_et*CEILING(R/tasks[i].period);
	}

	return sumDM;
}
double recoveryDemand(attri* tasks, int curIdx)
{ 
	//sum all high priority tasks' (Ca-Cn)
	double sumF=0;
	int i=0;
	for(i=0; i<curIdx; i++)
	{
		sumF+=(tasks[i].abnormal_et-tasks[i].normal_et);
	}
	return sumF;
}

double Omega(int curIdx, attri* tasks, double delta)
{
	double Dn = tasks[curIdx].period;
	double Cn = tasks[curIdx].normal_et;
	double Ca = tasks[curIdx].abnormal_et;
	double R = Cn;
	double Rd = recoveryDemand(tasks, curIdx);
	volatile double dm = 0;
	while(1)
	{
		dm=delta+Rd+normalDemand(R, tasks, curIdx)+(Ca-Cn);
		if(R>Dn)
		{
			return -1; //unschedule
		}
		if(R < dm+Cn)
		{
			R= dm + Cn;
		}else
		{
			return R; //so R-Dn must be <= 0
		}		
	}
}

void OnlineDetectiveRTA(int detectIdx, int* running, int* susTable, double* delta, attri* tasks, int nTask)
{
	//assume the priorities of tasks are assigned sequentially from 1 to n is high to low.
	//assume input susTable is initialized as all 0
	//Output: the suspicious table result
	int i = 0;
	for(i=detectIdx; i<nTask; i++) //test from detectIdx to so-on
	{	
		if(running[i]){
			int Ok = Omega(i, tasks, delta[i]);
			if(Ok < 0)
				susTable[i]=1; //suspicious task
		}
	}
}
void OnlineDetectiveK2U(int detectIdx, int* running, int* susTable, double* delta, attri* tasks, int nTask)
{
	//assume the priorities of tasks are assigned sequentially from 1 to n is high to low.
	//assume input susTable is initialized as all 0
	//Output: the suspicious table result
	int i=0;
	for(i=detectIdx; i<nTask; i++) //test from detectIdx to so-on
	{	
		if(running[i]){
			double Dn = tasks[i].period;
			double Ca = tasks[i].abnormal_et;
			double prodU=1;
			double sumF=0;
			int j=0;
			for(j=0; j<i; j++)//calculate all high priority tasks' properties
			{
				prodU=(tasks[j].normal_et/tasks[j].period+1)*prodU;
				sumF+=(tasks[j].abnormal_et-tasks[j].normal_et);
			}
			if(((delta[i]+sumF+Ca)/Dn+1)>2/prodU) //task i is not schedulable
				susTable[i]=1; //suspicious task
		}
	}
}

int comparePeriod(const void *a, const void *b){
	attri* c = (attri*) a;
        attri* d = (attri*) b;
        return (double)(c->period - d->period);
}

int comparePriority(const void *a, const void *b){
	attri* c = (attri*) a;
        attri* d = (attri*) b;
        return (int)(c->priority - d->priority);
}

double abnorDemand(double R, attri* tasks, int ntask, int curIdx)
{
        double sumDM=tasks[curIdx].abnormal_et;
	int i=0;
        for(i=0; i<ntask; i++)
        {
		if(i != curIdx && tasks[i].priority == -1)
	                sumDM += tasks[i].abnormal_et*CEILING(R/tasks[i].period);
        }

        return sumDM;
}
double norDemand(double R, attri* tasks, int ntask, int curIdx)
{
        double sumDM=tasks[curIdx].normal_et;
	int i=0;
        for(i=0; i<ntask; i++)
        {
		if(i != curIdx && tasks[i].priority == -1)
	                sumDM += tasks[i].normal_et*CEILING(R/tasks[i].period);
        }

        return sumDM;
}

double RTA(int curIdx, attri* tasks, int nTask, int mode)
{
	double Dn = tasks[curIdx].period;
//	printf("Tsk %d deadline %lf\n", curIdx+1, Dn);
	double dm = 0;
	if(mode == 0) //0 is soft task checking normal execution time
	    dm=norDemand(Dn, tasks, nTask, curIdx);
	else
	    dm=abnorDemand(Dn, tasks, nTask, curIdx);
	if( dm <= Dn ){
//	    printf("Feasible dm %lf\n", dm);
	    return dm;
	}

	int i=0;
	double t = tasks[curIdx].normal_et; 	
	if(mode == 1)
      t = tasks[curIdx].abnormal_et;
  for(i=0; i<ntask; i++)
  {
      if(i != curIdx && tasks[i].priority==-1){
          if(mode ==0)
              t += tasks[i].normal_et;
          else
              t += tasks[i].abnormal_et;
      }
  }

//	printf("task %d t=%lf\n", curIdx+1, t);
        //recursive TDA
	while(t < Dn)
	{	
      if(mode == 0) //0 is soft task checking normal execution time
          dm=norDemand(t, tasks, nTask, curIdx);
      else
          dm=abnorDemand(t, tasks, nTask, curIdx);
      if (dm <= t){
  //			printf("Feasible t %lf\n", t);
        return t;
      }
      else{
  //			printf("Demand is larger than the current t=%lf, dm=%lf\n", t, dm);
        t = dm;
      }
	}
//	printf("task %d fails to schedule, t=%lf\n", curIdx+1, t);
	return 0; //fail
}

int findAssignment(attri* tasks, attri* hardTasks, attri* softTasks, int nTask, int hard, int soft)
{
	int detectIdx=0; //tau_l in the paper
	int lastHard = hard-1;
	int lastSoft = soft-1; //maybe it is -1;
	int n = 0;
	for(n = nTask; n >0; n--)
	{	
		if(lastHard>=0){
			detectIdx = hardTasks[lastHard].id; //last element of Hardtasks
			if(detectIdx >= 0 && RTA(detectIdx, tasks, nTask, 1)){ 
				//first condition is designed to check whether the set is empty.
				tasks[detectIdx].priority=n;
//				printf("hard task %d assigns as priority %d\n", detectIdx+1, n);
				lastHard--;
			}else{
				if (lastSoft >= 0){
					detectIdx = softTasks[lastSoft].id; //last element of softtasks
					if(detectIdx >= 0 && RTA(detectIdx, tasks, nTask, 0)){
						tasks[detectIdx].priority=n;
//						printf("soft task %d assigns as priority %d\n", detectIdx+1, n);
						lastSoft--;
					}else{
						return -1; //NOT POSSIBLE
					}
				}else{
					return -1;			
				}
			}
		}else if(lastSoft>=0){
			detectIdx = softTasks[lastSoft].id; //last element of softtasks
			if(detectIdx >= 0 && RTA(detectIdx, tasks, nTask, 0)){
				tasks[detectIdx].priority=n;
//				printf("Remaining soft task %d assigns as priority %d\n", detectIdx+1, n);
				lastSoft--;
			}else
				return -1; //NOT POSSIBLE
		}					
	}

	return 1; //feasible
}

int priority_assignment(attri* tasks, int nTask)
{
  //assume the given input is already assigned priority.
	int i =0;
	for(i=0; i<nTask; i++){
		tasks[i].id=i; //very important
		tasks[i].priority=i+1;
	}

	return 1;
}

void Random_Generate_Task_Type(int nTask, double desired_hardrealtime_percentage, attri* tasks){
        int hardRTtask_total;
        int softRTtask_total;
        int val;
        int i = 0;
        int hardcount = 0;
        int softcount = 0;

        hardRTtask_total = nTask*desired_hardrealtime_percentage;
        softRTtask_total = nTask-hardRTtask_total;
        printf("Random Task type: hard = %d, soft = %d\n", hardRTtask_total, softRTtask_total);

        for(i = 0; i<nTask; i++){
                if(hardcount < hardRTtask_total && softcount < softRTtask_total){
                    val=rand()%2;
                    if(val == 0){
                        tasks[i].task_type = 0;
                        hardcount++;
                    }else{
                        tasks[i].task_type = 1;
                        softcount++;
                    }
                }else if(hardcount >= hardRTtask_total){
                    tasks[i].task_type = 1;
                    softcount++;
                }else if(softcount >= softRTtask_total){
                    tasks[i].task_type = 0;
                    hardcount++;
                }
        }

}

void Exhaustive_Task_Type(int nTask, int decimal, attri* tasks){
	int i, binary[5];
	for(i=1;i<=nTask;i++){
      binary[nTask-i]=decimal%2;
      decimal=decimal/2;
	}	
	for(i=0; i<nTask; i++)		
      tasks[i].task_type=binary[i];
}

int check_running_task(int* suspendedTask){
	int j = 0;
	int i = 0;
	int numberPreemptedTask = 0;

	for(j=0; j<ntask; j++){
      numberPreemptedTask += taskrunning_table[j];
		
      if(taskrunning_table[j] == 1){
          suspendedTask[i] = j;
          i++;
      }
	}
	
	return numberPreemptedTask;
}
