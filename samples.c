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

bool task_fault_check(double factor)
{
	int fault_rand;
	double cfec = factor/tick_per_second;
	double trial = fault_rate[refer_fault_rate]*100000*cfec;

  fault_rand = rand()%100000;

  if(fault_rand < trial)
      return TRUE; 
  else
      return FALSE;
}

bool check_busyP(int detectIdx, int nTask)
{
  //KHCHEN: fix the busyperiod prediction after discussing with Georg
  //TODO Syncronize the # of postponed jobs table.
  int x[11], i=0;

  for (i=0; i<11; i++){
    x[i]=0;
  }
 
  for(i=detectIdx; i<nTask; i++)
  {
    //call the helper function in RMS manager to get the number of x
    x[i] = rtems_rate_monotonic_Postponed_num(period_id[i]);
    if(x[i] < 0)
      printf("BUG: # of postponed jobs is not positive\n");
  }

  //for (i=0; i<11; i++){
    //printf("%d,",x[i]);
  //}
  //printf("\n");

	int sum = 0, j=0;
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
                  if(taskrunning_table[j]== 2){
                    carry_in += tsk[j].abnormal_et;
                  }
                  else if(taskrunning_table[j]== 1){
                    carry_in += tsk[j].normal_et;
                  }
                  if(tsk[j].task_type == 0 && x[j] > 0)
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
	else{
      return FALSE;
  }

}

bool check_deadline(bool* first_task_flag , int nTask, double deadline, double end, int task_type, int task_id, double tick_per_second)
{
	int i = 0;
	int check_task_dl = 0;
	double sys_turn_healthy;

	if(sys_stop_flag != TRUE){	
		if(end <= deadline || (end > deadline && task_type == 1)){
			if(end > deadline && task_type == 1){
				sp_dl_missed_table[task_id] = 1;
			}
			else if(end <= deadline){
				sp_dl_missed_table[task_id] = 0;
			}

			for(i=0; i < nTask; i++){
				check_task_dl += sp_dl_missed_table[i];
			}

			if(*first_task_flag == TRUE && task_running_flag == TRUE){
				if(sys_fault_flag == TRUE){
					sys_unhealthy_end = end;
					sys_unhealthy_duration = sys_unhealthy_end - sys_unhealthy_start;
					sys_unhealthy_total_duration += sys_unhealthy_duration;
		      
					if(check_task_dl == 0){
						sys_fault_flag = FALSE;      
					}
				}else if(sys_fault_flag == FALSE){
					sys_healthy_end = end;
					sys_healthy_duration = sys_healthy_end - sys_healthy_start;
					sys_healthy_total_duration += sys_healthy_duration;
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
				printf("Deadline is %lf and Task ends at %lf\n", deadline, end);
			}else if(sys_fault_flag == FALSE){
				sys_healthy_end = end;
				sys_healthy_duration = sys_healthy_end - sys_healthy_start;
				sys_healthy_total_duration += sys_healthy_duration;
				printf("Deadline is %lf and Task ends at %lf\n", deadline, end);
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

void priority_assignment(attri* tasks, int nTask)
{
  //assume the given input is already assigned priority.
	int i =0;
	for(i=0; i<nTask; i++){
		tasks[i].id=i; //very important
		tasks[i].priority=i+1;
	}
}

int check_running_task(int* suspendedTask){
	int j = 0;
	int i = 0;
	int numberPreemptedTask = 0;

	for(j=0; j<ntask; j++){
		
      if(taskrunning_table[j] > 0){
        //recording the number of the tasks that are being suspended into a table 
         suspendedTask[i] = j;
         i++;
      }
	}
  numberPreemptedTask = i;
	return numberPreemptedTask;
}
