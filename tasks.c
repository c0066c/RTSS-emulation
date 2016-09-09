/**
 *  
 *  This project is done by Faculty of informatics Chair 12 TU Dortmund
 *  Author: Huan Fui Lee & Kuan Hsun Chen
 *  Implementation for paper
 */


#include "system.h"
#include "samples.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* CPU usage and Rate monotonic manger statistics */
#include "rtems/cpuuse.h"

/* Periods for the various tasks [seconds]*/
#define PERIOD_TASK_1  tsk[0].period
#define PERIOD_TASK_2  tsk[1].period
#define PERIOD_TASK_3  tsk[2].period
#define PERIOD_TASK_4  tsk[3].period
#define PERIOD_TASK_5	 tsk[4].period
#define PERIOD_TASK_6  tsk[5].period
#define PERIOD_TASK_7  tsk[6].period
#define PERIOD_TASK_8	 tsk[7].period
#define PERIOD_TASK_9  tsk[8].period
#define PERIOD_TASK_10 tsk[9].period

/* Periods for the various tasks */
#define ID_TASK_1  tsk[0].id
#define ID_TASK_2	 tsk[1].id
#define ID_TASK_3  tsk[2].id
#define ID_TASK_4  tsk[3].id
#define ID_TASK_5	 tsk[4].id
#define ID_TASK_6  tsk[5].id
#define ID_TASK_7  tsk[6].id
#define ID_TASK_8	 tsk[7].id
#define ID_TASK_9  tsk[8].id
#define ID_TASK_10 tsk[9].id

/* Task type for the various tasks : 0 means hard real time task, 1 means soft real time task */
#define TYPE_TASK_1  tsk[0].task_type
#define TYPE_TASK_2	 tsk[1].task_type
#define TYPE_TASK_3  tsk[2].task_type
#define TYPE_TASK_4  tsk[3].task_type
#define TYPE_TASK_5	 tsk[4].task_type
#define TYPE_TASK_6  tsk[5].task_type
#define TYPE_TASK_7  tsk[6].task_type
#define TYPE_TASK_8	 tsk[7].task_type
#define TYPE_TASK_9  tsk[8].task_type
#define TYPE_TASK_10 tsk[9].task_type

/* Execution time for each task */
#define task_1_normal_et	  tsk[0].normal_et
#define task_1_abnormal_et	tsk[0].abnormal_et
#define task_2_normal_et	  tsk[1].normal_et
#define task_2_abnormal_et	tsk[1].abnormal_et
#define task_3_normal_et	  tsk[2].normal_et
#define task_3_abnormal_et	tsk[2].abnormal_et
#define task_4_normal_et	  tsk[3].normal_et
#define task_4_abnormal_et	tsk[3].abnormal_et
#define task_5_normal_et	  tsk[4].normal_et
#define task_5_abnormal_et	tsk[4].abnormal_et
#define task_6_normal_et	  tsk[5].normal_et
#define task_6_abnormal_et	tsk[5].abnormal_et
#define task_7_normal_et	  tsk[6].normal_et
#define task_7_abnormal_et	tsk[6].abnormal_et
#define task_8_normal_et	  tsk[7].normal_et
#define task_8_abnormal_et	tsk[7].abnormal_et
#define task_9_normal_et	  tsk[8].normal_et
#define task_9_abnormal_et	tsk[8].abnormal_et
#define task_10_normal_et	  tsk[9].normal_et
#define task_10_abnormal_et	tsk[9].abnormal_et

/* Number of task */
#define nTask 			ntask

/* TASK 1 */
rtems_task Task_1(
  rtems_task_argument unused
)
{
  rtems_status_code status;
  rtems_name        period_name; 
  rtems_id          RM_period;
  rtems_id          selfid=rtems_task_self();
  double            first_start, start, end, deadline, sys_turn_unhealthy, remaining_time;
  int		            i=0, j=0;
  bool 		          first_task_flag = FALSE;
  bool 		          task_fault = FALSE;
  int		            task_id = ID_TASK_1;
  bool		          healthy, stop_sys;
  int 		          task_type = TYPE_TASK_1; 
  int		            tsk_counter =0; /* counter for this task */
  int               totalruntasks =0;
  int		            suspendedTask[10]; /* Table to save the check the preempted task */
  int		            numberPreemptedTask =0;
  int		            startTick =0; 
  int 		          suspendedTaskid =100; 

  /* Random seed */
  srand(seedseed+task_id);
  
  /* create and register period in scheduler */
  period_name = rtems_build_name( 'P', 'E', 'R', '1' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

  //KHCHEN 03.08, record the period ID for helper function
  period_id[task_id] = RM_period;
  while( 1 ) {

    /* Check the internal time of system and call/run this task again at its period */
    status = rtems_rate_monotonic_period( RM_period,PERIOD_TASK_1);

    /* wait for all tasks are released */
    if(AllReady){
      /* Sys_stop_flag only set True when the hard real time task failed to meet its deadline;
       * if system is termimated under normal condition where lowest priority task meet the predefined number, it will set experiment flag to 0
       */ 
      if(sys_stop_flag == TRUE || experiment_flag == 0){
        totalruntasks = 0;
        for(i=0; i<nTask; i++){
          totalruntasks +=running_flag[i];
        }
        
        /* Calculate the percentage of system being in healthy condition */
        sys_totalruntime = sys_healthy_total_duration + sys_unhealthy_total_duration;
        sys_totalhealthy_percentage = (sys_healthy_total_duration/ sys_totalruntime)*100;
        sys_totalunhealthy_percentage = (sys_unhealthy_total_duration/ sys_totalruntime)*100;

        if(totalruntasks == 1){/*till all tasks are deleted besides task 1 */

          running_flag[0]=0;

          if(sys_stop_flag == TRUE)
            printf("Failed to meet deadline.\n");

          printf("System healthy percentage is %.6f ",sys_totalhealthy_percentage);

          /* To resume at the point where it is suspended in init.c */
          status = rtems_task_resume(inittask_id);
          if(status!= RTEMS_SUCCESSFUL) {
            printf("BUG::init task cannot be resumed\n");
            exit(1);
          }

          /* Delete this task and its period in scheduler */
          status = rtems_rate_monotonic_delete(RM_period);
          if(status!= RTEMS_SUCCESSFUL){
            printf("BUG: Cannot delete the period 1\n");
            exit(1);
          }
                    
          status = rtems_task_delete(selfid);
          if(status != RTEMS_SUCCESSFUL){
            printf("BUG: Cannot delete the task 1\n");
            exit(1);
          }
        }
      }else{
        
        /* Start time of task */    
        startTick = rtems_clock_get_ticks_since_boot();
        start = startTick  / (double)tick_per_second;

        if(tsk_counter == 0){
          first_start = start;
        }
        
        /* Check if this task preempt the other tasks */
        numberPreemptedTask = check_running_task(suspendedTask);
        
        if(numberPreemptedTask != 0){
          suspendedTaskid = 0;
          for(j=0; j<numberPreemptedTask; j++){
            suspendedTaskid = suspendedTask[j];
            if(preempted_table[0][suspendedTaskid]==0){
              preempted_table[0][suspendedTaskid]= numberPreemptedTask;
              preempted_table[1][suspendedTaskid]= startTick;
            }
          }
        }
        
        taskrunning_table[task_id] = 1;
        
        if(first_task_flag == FALSE && task_running_flag == FALSE){
          if(sys_fault_flag == FALSE){
            sys_healthy_start = start;
          }else if(sys_fault_flag == TRUE){
            sys_unhealthy_start = start;
          }
          task_running_flag = TRUE;
          first_task_flag = TRUE;
        }

        /* Run its normal execution time  */
        LOOP(task_1_normal_et,task_id);
        
        /* Perform fault checking; to mimic the behaviour of the task might go wrong at any time with percentage-wise over its normal execution time */
        task_fault = task_fault_check(task_1_normal_et);
  

        if(task_fault == TRUE){
          
          taskrunning_table[task_id] = 2;
          
          /* Check whether this faulty task will cause system become unhealthy or not */
          healthy = check_busyP(task_id, nTask);

          /* If faulty task will cause system become unheathly and system is currently in healthy condition */
          if(healthy == FALSE  && sys_fault_flag == FALSE){
            sys_fault_flag = TRUE;
            sys_turn_unhealthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
            sys_healthy_end = sys_turn_unhealthy;
            sys_unhealthy_start = sys_turn_unhealthy;
            sys_healthy_duration = sys_healthy_end - sys_healthy_start;
            sys_healthy_total_duration += sys_healthy_duration;
          }

          remaining_time = task_1_abnormal_et - task_1_normal_et;
          /* Since this task is faulty then it execute for its abnormal execution time*/
          if(remaining_time != 0)
            LOOP(remaining_time,task_id);
        }
        
        /* End time of task */
        end = rtems_clock_get_ticks_since_boot()/(double)tick_per_second;

        /* Check whether the task missed its deadline*/
        deadline = first_start + (tsk_counter +1)*PERIOD_TASK_1/ (double)tick_per_second;
        stop_sys = check_deadline(&first_task_flag ,nTask, deadline, end, task_type, task_id, tick_per_second);
        
        if(stop_sys == TRUE){
          sys_stop_flag = TRUE;
        }else{
          taskrunning_table[task_id] = 0;
          tsk_counter += 1;
        }
			}
		}
	}
}

// TASK 2
rtems_task Task_2(
  rtems_task_argument unused
)
{
  rtems_status_code status;
  rtems_name        period_name; 
  rtems_id          RM_period;
  rtems_id          selfid=rtems_task_self();
  double 	          first_start, start, end, deadline, sys_turn_unhealthy, remaining_time;
  bool 		          first_task_flag = FALSE;
  bool 		          task_fault = FALSE;
  int		            task_id = ID_TASK_2;
  bool		          healthy, stop_sys;
  int 		          task_type = TYPE_TASK_2;
  int		            tsk_counter = 0; /* counter for this task */
  int		            suspendedTask[10];/* Table to save the check the preempted task */
  int		            numberPreemptedTask = 0;
  int		            startTick = 0;
  int 		          suspendedTaskid = 100;
  int		            j = 0;

  /* Random seed */
  srand(seedseed+task_id);

  /* Create and register period in scheduler */
  period_name = rtems_build_name( 'P', 'E', 'R', '2' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

  //KHCHEN 03.08, record the period ID for helper function
  period_id[task_id] = RM_period;

	while( 1 ) {
 
    /* Check the internal time of system and call/run this task again at its period */
    status = rtems_rate_monotonic_period( RM_period, PERIOD_TASK_2);

    /* wait for all tasks are released */
    if(AllReady){

      if(sys_stop_flag == TRUE || experiment_flag == 0){
        /* Deleting task and its period as termination criterion is met */
        running_flag[1]=0;
        status = rtems_rate_monotonic_delete(RM_period);
        if(status != RTEMS_SUCCESSFUL){
          printf("BUG: Cannot delete the period 2\n");
          exit(1);
        }

        running_flag[1]=0;
        status=rtems_task_delete(selfid);
        if(status != RTEMS_SUCCESSFUL){
          printf("BUG: Cannot delete the task 2\n");
          exit(1);
        }
        
      }else{
        /* Start time of task */
        startTick = rtems_clock_get_ticks_since_boot();
        start = startTick  / (double)tick_per_second;

        if(tsk_counter == 0){
          first_start = start;
          }
        
        /* Check whether this task preempt the other tasks */
        numberPreemptedTask = check_running_task(suspendedTask);
        
        if(numberPreemptedTask != 0){
          suspendedTaskid = 0;
          for(j=0; j<numberPreemptedTask; j++){
            suspendedTaskid = suspendedTask[j];
            if(preempted_table[0][suspendedTaskid]==0){
              preempted_table[0][suspendedTaskid]= numberPreemptedTask;
              preempted_table[1][suspendedTaskid]= startTick;
            }
          }
        }

        taskrunning_table[task_id] = 1;

        if(first_task_flag == FALSE && task_running_flag == FALSE){
          if(sys_fault_flag == FALSE){
            sys_healthy_start = start;
          }else if(sys_fault_flag == TRUE){
            sys_unhealthy_start = start;
          }
          task_running_flag = TRUE;
          first_task_flag = TRUE;
        }
  
        /* Run for its normal execution time */
        LOOP(task_2_normal_et,task_id);

        /* Perform fault checking */
        task_fault = task_fault_check(task_2_normal_et);

        if(task_fault == TRUE){

          taskrunning_table[task_id] = 2;

          /* Check if this faulty task affect the health of system*/
          healthy = check_busyP(task_id, nTask);
          if(healthy == FALSE && sys_fault_flag == FALSE){
            sys_fault_flag = TRUE;
            sys_turn_unhealthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
            sys_healthy_end = sys_turn_unhealthy;
            sys_unhealthy_start = sys_turn_unhealthy;
            sys_healthy_duration = sys_healthy_end - sys_healthy_start;
            sys_healthy_total_duration += sys_healthy_duration;
          }
          remaining_time = task_2_abnormal_et - task_2_normal_et;
          if(remaining_time != 0)
            LOOP(remaining_time,task_id);
        }

        /* End time of task */
        end = rtems_clock_get_ticks_since_boot()/(double)tick_per_second;

        /* Check if task missed its deadline */
        deadline = first_start + (tsk_counter +1)*PERIOD_TASK_2/ (double)tick_per_second;
        stop_sys = check_deadline(&first_task_flag ,nTask, deadline, end, task_type, task_id, tick_per_second);

        if(stop_sys == TRUE){
          sys_stop_flag = TRUE;
        }else{      
          taskrunning_table[task_id] = 0;
          tsk_counter += 1;
        }
   		}
		}
 	}
}

// TASK 3
rtems_task Task_3(
  rtems_task_argument unused
)
{
  rtems_status_code status;
  rtems_name        period_name; 
  rtems_id          RM_period;
  rtems_id          selfid=rtems_task_self();
  double            first_start, start, end, deadline, sys_turn_unhealthy, remaining_time;
  bool 		          first_task_flag = FALSE;
  bool 		          task_fault = FALSE;
  int		            task_id = ID_TASK_3;
  bool		          healthy, stop_sys;
  int 		          task_type = TYPE_TASK_3;
  int		            tsk_counter = 0; /* counter for this task */
  int		            suspendedTask[10];/* Table to save the check the preempted task */
  int		            numberPreemptedTask = 0;
  int		            startTick = 0;
  int 		          suspendedTaskid = 100;
  int		            j = 0;

  /* Random seed */
  srand(seedseed+task_id);

  /* Create and register period */
  period_name = rtems_build_name( 'P', 'E', 'R', '3' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

  //KHCHEN 03.08, record the period ID for helper function
  period_id[task_id] = RM_period;

	while( 1 ) {

    /* Check the internal time of system and call/run this task again at its period */
		status = rtems_rate_monotonic_period( RM_period, PERIOD_TASK_3);
    
   /* wait for all tasks are released */
   if(AllReady){ 
      if(sys_stop_flag == TRUE || experiment_flag == 0){
        /* Deleting task and its period as termination criterion is met */
        status = rtems_rate_monotonic_delete(RM_period);
        if(status != RTEMS_SUCCESSFUL){
          printf("BUG: Cannot delete the period 3\n");
          exit(1);
        }

        running_flag[2]=0;
        status=rtems_task_delete(selfid);
        if(status != RTEMS_SUCCESSFUL){
          printf("BUG: Cannot delete the task 3\n");
          exit(1);
        }		
      }else{
        /* Start time of the task */
        startTick = rtems_clock_get_ticks_since_boot();
        start = startTick  / (double)tick_per_second;

        if(tsk_counter == 0){
          first_start = start;
        }
        
        /* Check if this task preempts the other tasks */
        numberPreemptedTask = check_running_task(suspendedTask);
        
        if(numberPreemptedTask != 0){
          suspendedTaskid = 0;
          for(j=0; j<numberPreemptedTask; j++){
            suspendedTaskid = suspendedTask[j];
            if(preempted_table[0][suspendedTaskid]==0){
              preempted_table[0][suspendedTaskid]= numberPreemptedTask;
              preempted_table[1][suspendedTaskid]= startTick;
            }
          }
        }

        taskrunning_table[task_id] = 1;
        
        if(first_task_flag == FALSE && task_running_flag == FALSE){
          if(sys_fault_flag == FALSE){
            sys_healthy_start = start;
          }else if(sys_fault_flag == TRUE){
            sys_unhealthy_start = start;
          }
          task_running_flag = TRUE;
          first_task_flag = TRUE;
        }

        /* Run for its normal execution time */
        LOOP(task_3_normal_et,task_id);

        /* Perform task fault checking */
        task_fault = task_fault_check(task_3_normal_et);
        
        if(task_fault == TRUE){
          taskrunning_table[task_id] = 2;
          /* Check if this faulty task affect the health of the system */
          healthy = check_busyP(task_id, nTask);	
          if(healthy == FALSE  && sys_fault_flag == FALSE){
            sys_fault_flag = TRUE;
            sys_turn_unhealthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
            sys_healthy_end = sys_turn_unhealthy;
            sys_unhealthy_start = sys_turn_unhealthy;
            sys_healthy_duration = sys_healthy_end - sys_healthy_start;
            sys_healthy_total_duration += sys_healthy_duration;
          }
          remaining_time = task_3_abnormal_et - task_3_normal_et;
          if(remaining_time != 0)
            LOOP(remaining_time,task_id);
        }

        /* End time of task */
        end = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;

        /* Check if the task missed deadline */
        deadline = first_start + (tsk_counter +1)*PERIOD_TASK_3/ (double)tick_per_second;
        stop_sys = check_deadline(&first_task_flag ,nTask, deadline, end, task_type, task_id, tick_per_second);

        if(stop_sys == TRUE){
          sys_stop_flag = TRUE;
        }else{
          taskrunning_table[task_id] = 0;
          tsk_counter += 1;
        }
      }
		}
	}
}

// TASK 4
rtems_task Task_4(
  rtems_task_argument unused
)
{
  rtems_status_code status;
  rtems_name        period_name; 
  rtems_id          RM_period;
  rtems_id          selfid=rtems_task_self();
  double 	          first_start, start, end, deadline, sys_turn_unhealthy, remaining_time;
  bool 		          first_task_flag = FALSE;
  bool 		          task_fault = FALSE;
  int		            task_id = ID_TASK_4;
  bool		          healthy, stop_sys;
  int 		          task_type = TYPE_TASK_4;
  int		            tsk_counter = 0; /* counter for this task */
  int		            suspendedTask[10];/* Table to save the check the preempted task */
  int		            numberPreemptedTask = 0;
  int		            startTick = 0;
  int 		          suspendedTaskid = 100;
  int		            j = 0;
  
  /* Random seed */
  srand(seedseed+task_id);

  /* Create and register period in scheduler */
  period_name = rtems_build_name( 'P', 'E', 'R', '4' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

  //KHCHEN 03.08, record the period ID for helper function
  period_id[task_id] = RM_period;

	while( 1 ) {
 
    /* Check the internal time of system and call/run this task again at its period */
		status = rtems_rate_monotonic_period( RM_period, PERIOD_TASK_4);

    /* wait for all tasks are released */
    if(AllReady){
      if(sys_stop_flag == TRUE || experiment_flag ==0 ){
        /* Delete the task and its period */
        status = rtems_rate_monotonic_delete(RM_period);
        if(status != RTEMS_SUCCESSFUL){
          printf("BUG: Cannot delete the period 4\n");
          exit(1);
        }

        running_flag[3]=0;
        status=rtems_task_delete(selfid);
        if(status != RTEMS_SUCCESSFUL){
          printf("BUG: Cannot delete the task 4\n");
          exit(1);
        }
      
      }else{
        /* Start time of the task*/
        startTick = rtems_clock_get_ticks_since_boot();
        start = startTick  / (double)tick_per_second;

        if(tsk_counter == 0){
          first_start = start;
        }
        
        /* Check if this task preempts the other tasks */
        numberPreemptedTask = check_running_task(suspendedTask);
        
        if(numberPreemptedTask != 0){
          suspendedTaskid = 0;
          for(j=0; j<numberPreemptedTask; j++){
            suspendedTaskid = suspendedTask[j];
            if(preempted_table[0][suspendedTaskid]==0){
              preempted_table[0][suspendedTaskid]= numberPreemptedTask;
              preempted_table[1][suspendedTaskid]= startTick;
            }
          }
        }
       
        taskrunning_table[task_id] = 1;

        if(first_task_flag == FALSE && task_running_flag == FALSE){
          if(sys_fault_flag == FALSE){
            sys_healthy_start = start;
          }else if(sys_fault_flag == TRUE){
            sys_unhealthy_start = start;
          }
          task_running_flag = TRUE;
          first_task_flag = TRUE;
        }

        /* Run for its normal execution time */
        LOOP(task_4_normal_et,task_id);

        /* Perform fault checking */
        task_fault = task_fault_check(task_4_normal_et);

        if(task_fault == TRUE){
          taskrunning_table[task_id] = 2;
          /* Check if this faulty task affect the health of the system*/
          healthy = check_busyP(task_id, nTask);
          if(healthy == FALSE && sys_fault_flag == FALSE){
            sys_fault_flag = TRUE;
            sys_turn_unhealthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
            sys_healthy_end = sys_turn_unhealthy;
            sys_unhealthy_start = sys_turn_unhealthy;
            sys_healthy_duration = sys_healthy_end - sys_healthy_start;
            sys_healthy_total_duration += sys_healthy_duration;
          }
          remaining_time = task_4_abnormal_et - task_4_normal_et;
          if(remaining_time != 0)
            LOOP(remaining_time,task_id);
        }

        /* End time of the task */
        end = rtems_clock_get_ticks_since_boot()/(double)tick_per_second;

        /* Check if the task missed its deadline*/
        deadline = first_start + (tsk_counter +1)*PERIOD_TASK_4/ (double)tick_per_second;
        stop_sys = check_deadline(&first_task_flag ,nTask, deadline, end, task_type, task_id, tick_per_second);

        if(stop_sys == TRUE){
          sys_stop_flag = TRUE;
        }else{
          taskrunning_table[task_id] = 0;
          tsk_counter += 1;
        }
			}
		}    
	}
}

// TASK 5
rtems_task Task_5(
  rtems_task_argument unused
)
{
  rtems_status_code status;
  rtems_name        period_name; 
  rtems_id          RM_period;
  rtems_id          selfid=rtems_task_self();
  double            first_start, start, end, deadline, sys_turn_unhealthy, remaining_time;
  bool 		          first_task_flag = FALSE;
  bool 		          task_fault = FALSE;
  int		            task_id = ID_TASK_5;
  bool		          healthy, stop_sys;
  int 		          task_type = TYPE_TASK_5;
  int		            tsk_counter = 0; /* counter for this task */
  int		            suspendedTask[10];/* Table to save the check the preempted task */
  int		            numberPreemptedTask = 0;
  int		            startTick = 0;
  int 		          suspendedTaskid = 100;
  int 		          j = 0;

  /* Random seed */
  srand(seedseed+task_id);

  /* Create and register period */
  period_name = rtems_build_name( 'P', 'E', 'R', '5' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

  //KHCHEN 03.08, record the period ID for helper function
  period_id[task_id] = RM_period;

	while( 1 ) {

    /* Check the internal time of system and call/run this task again at its period */
		status = rtems_rate_monotonic_period( RM_period, PERIOD_TASK_5);
  
    /* wait for all tasks are released */
    if(AllReady){
      if(sys_stop_flag == TRUE  || experiment_flag ==0 ){
        /* Delete the task and its period as termination criterion is met */
        status = rtems_rate_monotonic_delete(RM_period);
        if(status != RTEMS_SUCCESSFUL){
          printf("BUG: Cannot delete the period 5\n");
          exit(1);
        }

        running_flag[4]=0;
        status=rtems_task_delete(selfid);
        if(status != RTEMS_SUCCESSFUL){
          printf("BUG: Cannot delete the task 5\n");
          exit(1);
        }		
      }else{
        /* Start time of the task*/
        startTick = rtems_clock_get_ticks_since_boot();
        start = startTick  / (double)tick_per_second;

        if(tsk_counter == 0){
          first_start = start;
        }
        
        /* Check if the task preempts the other tasks*/
        numberPreemptedTask = check_running_task(suspendedTask);
        
        if(numberPreemptedTask != 0){
          suspendedTaskid = 0;
          for(j=0; j<numberPreemptedTask; j++){
            suspendedTaskid = suspendedTask[j];
            if(preempted_table[0][suspendedTaskid]==0){
              preempted_table[0][suspendedTaskid]= numberPreemptedTask;
              preempted_table[1][suspendedTaskid]= startTick;
            }
          }
        }

        taskrunning_table[task_id] = 1;
        
        if(first_task_flag == FALSE && task_running_flag == FALSE){
          if(sys_fault_flag == FALSE){
            sys_healthy_start = start;
          }else if(sys_fault_flag == TRUE){
            sys_unhealthy_start = start;
          }
          task_running_flag = TRUE;
          first_task_flag = TRUE;
        }

        /* Run for its normal execution time */
        LOOP(task_5_normal_et,task_id);

        /* Perform fault checking */
        task_fault = task_fault_check(task_5_normal_et);
        
        if(task_fault == TRUE){
          taskrunning_table[task_id] = 2;
          /* Check if this faulty task affect the health of the system */
          healthy = check_busyP(task_id, nTask);
          if(healthy == FALSE  && sys_fault_flag == FALSE){
            sys_fault_flag = TRUE;
            sys_turn_unhealthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
            sys_healthy_end = sys_turn_unhealthy;
            sys_unhealthy_start = sys_turn_unhealthy;
            sys_healthy_duration = sys_healthy_end - sys_healthy_start;
            sys_healthy_total_duration += sys_healthy_duration;
          }
          remaining_time = task_5_abnormal_et - task_5_normal_et;
          if(remaining_time != 0)
            LOOP(remaining_time,task_id);
        }

        /* End time of the task*/
        end = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;

        /* Check if the task missed its deadline */
        deadline = first_start + (tsk_counter +1)*PERIOD_TASK_5/ (double)tick_per_second;
        stop_sys = check_deadline(&first_task_flag ,nTask, deadline, end, task_type, task_id, tick_per_second);

        if(stop_sys == TRUE){
          sys_stop_flag = TRUE;
        }else{
          taskrunning_table[task_id] = 0;
          tsk_counter += 1;
        }
    	}
		}
	}
}

// TASK 6
rtems_task Task_6(
  rtems_task_argument unused
)
{
  rtems_id          selfid=rtems_task_self();
  rtems_status_code status;
  rtems_name        period_name; 
  rtems_id          RM_period;
  double 	          first_start, start, end, deadline, sys_turn_unhealthy, remaining_time;
  bool 		          first_task_flag = FALSE;
  bool 		          task_fault = FALSE;
  int		            task_id = ID_TASK_6;
  bool		          healthy, stop_sys;
  int 		          task_type = TYPE_TASK_6;
  int		            tsk_counter = 0; /* counter for this task */
  int		            suspendedTask[10];/* Table to save the check the preempted task */
  int		            numberPreemptedTask = 0;
  int		            startTick = 0;
  int 		          suspendedTaskid = 100;
  int		            j = 0;

  /* Random seed */
  srand(seedseed+task_id);

  /* Create and register period */
  period_name = rtems_build_name( 'P', 'E', 'R', '6' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

  //KHCHEN 03.08, record the period ID for helper function
  period_id[task_id] = RM_period;

	while( 1 ) {
 
    /* Check the internal time of system and call/run this task again at its period */
		status = rtems_rate_monotonic_period( RM_period, PERIOD_TASK_6);

    /* wait for all tasks are released */
    if(AllReady){    
      if(sys_stop_flag == TRUE  || experiment_flag ==0){
        /* Delete the task and its period as termination criterion is met */
        status = rtems_rate_monotonic_delete(RM_period);
        if(status != RTEMS_SUCCESSFUL){
          printf("BUG: Cannot delete the period 6\n");
          exit(1);
        }

        running_flag[5]=0;
        status=rtems_task_delete(selfid);
        if(status != RTEMS_SUCCESSFUL){
          printf("BUG: Cannot delete the task 6\n");
          exit(1);
        }		
      }else{ 
        /* Start time of the task */
        startTick = rtems_clock_get_ticks_since_boot();
        start = startTick  / (double)tick_per_second;

        if(tsk_counter == 0){
          first_start = start;
         }
        
        /* Check if this task preempts the other tasks */
        numberPreemptedTask = check_running_task(suspendedTask);
        
        if(numberPreemptedTask != 0){
          suspendedTaskid = 0;
          for(j=0; j<numberPreemptedTask; j++){
            suspendedTaskid = suspendedTask[j];
            if(preempted_table[0][suspendedTaskid]==0){
              preempted_table[0][suspendedTaskid]= numberPreemptedTask;
              preempted_table[1][suspendedTaskid]= startTick;
            }
          }
        }
       
        taskrunning_table[task_id] = 1;

        if(first_task_flag == FALSE && task_running_flag == FALSE){
          if(sys_fault_flag == FALSE){
            sys_healthy_start = start;
          }else if(sys_fault_flag == TRUE){
            sys_unhealthy_start = start;
          }
          task_running_flag = TRUE;
          first_task_flag = TRUE;
        }

        /* Run for its normal execution time */
        LOOP(task_6_normal_et,task_id);

        /* Perform fault checking */
        task_fault = task_fault_check(task_6_normal_et);

        if(task_fault == TRUE){
          taskrunning_table[task_id] = 2;
          /* Check if this faulty task affect the health of the system */
          healthy = check_busyP(task_id, nTask);
          if(healthy == FALSE && sys_fault_flag == FALSE){
            sys_fault_flag = TRUE;
            sys_turn_unhealthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
            sys_healthy_end = sys_turn_unhealthy;
            sys_unhealthy_start = sys_turn_unhealthy;
            sys_healthy_duration = sys_healthy_end - sys_healthy_start;
            sys_healthy_total_duration += sys_healthy_duration;
          }
          remaining_time = task_6_abnormal_et - task_6_normal_et;
          if(remaining_time != 0)
            LOOP(remaining_time,task_id);
          }

        /* End time of task */
        end = rtems_clock_get_ticks_since_boot()/(double)tick_per_second;
     
        /* Check if the task missed its deadline */
        deadline = first_start + (tsk_counter +1)*PERIOD_TASK_6/ (double)tick_per_second;
        stop_sys = check_deadline(&first_task_flag ,nTask, deadline, end, task_type, task_id, tick_per_second);

        if(stop_sys == TRUE){
          sys_stop_flag = TRUE;
        }else{
          taskrunning_table[task_id] = 0;
          tsk_counter += 1;
        }
			}
		}
	}
}

// TASK 7
rtems_task Task_7(
  rtems_task_argument unused
)
{
  rtems_id          selfid=rtems_task_self();
  rtems_status_code status;
  rtems_name        period_name; 
  rtems_id          RM_period;
  double            first_start, start, end, deadline, sys_turn_unhealthy, remaining_time;
  bool 		          first_task_flag = FALSE;
  bool 		          task_fault = FALSE;
  int		            task_id = ID_TASK_7;
  bool		          healthy, stop_sys;
  int 		          task_type = TYPE_TASK_7;
  int		            tsk_counter = 0; /* counter for this task */
  int		            suspendedTask[10];/* Table to save the check the preempted task */
  int		            numberPreemptedTask = 0;
  int		            startTick = 0;
  int 		          suspendedTaskid = 100;
  int		            j = 0;

  /* Random seed */
  srand(seedseed+task_id);
  
  /*  Create and register period */
  period_name = rtems_build_name( 'P', 'E', 'R', '7' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

  //KHCHEN 03.08, record the period ID for helper function
  period_id[task_id] = RM_period;

	while( 1 ) {

    /* Check the internal time of system and call/run this task again at its period */
		status = rtems_rate_monotonic_period( RM_period, PERIOD_TASK_7);
  
    /* wait for all tasks are released */
    if(AllReady){
      if(sys_stop_flag == TRUE  || experiment_flag ==0){
        /* Delete the task and its period as termination criterion is met */
        status = rtems_rate_monotonic_delete(RM_period);
        if(status != RTEMS_SUCCESSFUL){
          printf("BUG: Cannot delete the period 7\n");
          exit(1);
        }

        running_flag[6]=0;
        status=rtems_task_delete(selfid);
        if(status != RTEMS_SUCCESSFUL){
          printf("BUG: Cannot delete the task 7\n");
          exit(1);
        }
      }else{  
        /* Start time of the task */
        startTick = rtems_clock_get_ticks_since_boot();
        start = startTick  / (double)tick_per_second;

        if(tsk_counter == 0){
          first_start = start;
        }
     
        /* Check if the task preempts the other tasks */
        numberPreemptedTask = check_running_task(suspendedTask);
        
        if(numberPreemptedTask != 0){
          suspendedTaskid = 0;
          for(j=0; j<numberPreemptedTask; j++){
            suspendedTaskid = suspendedTask[j];
            if(preempted_table[0][suspendedTaskid]==0){
              preempted_table[0][suspendedTaskid]= numberPreemptedTask;
              preempted_table[1][suspendedTaskid]= startTick;
            }
          }
        }

        taskrunning_table[task_id] = 1;
        
        if(first_task_flag == FALSE && task_running_flag == FALSE){
          if(sys_fault_flag == FALSE){
            sys_healthy_start = start;
          }else if(sys_fault_flag == TRUE){
            sys_unhealthy_start = start;
          }
          task_running_flag = TRUE;
          first_task_flag = TRUE;
        }

        /* Run for its normal execution time */
        LOOP(task_7_normal_et,task_id);

        /* Perform fault checking */
        task_fault = task_fault_check(task_7_normal_et);
        
        if(task_fault == TRUE){
          taskrunning_table[task_id] = 2;
          /* Check if this faulty task affect the health of system */
          healthy = check_busyP(task_id, nTask);
          if(healthy == FALSE  && sys_fault_flag == FALSE){
            sys_fault_flag = TRUE;
            sys_turn_unhealthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
            sys_healthy_end = sys_turn_unhealthy;
            sys_unhealthy_start = sys_turn_unhealthy;
            sys_healthy_duration = sys_healthy_end - sys_healthy_start;
            sys_healthy_total_duration += sys_healthy_duration;
          }
          remaining_time = task_7_abnormal_et - task_7_normal_et;
          if(remaining_time != 0)
            LOOP(remaining_time,task_id);
        }

        /* End time of the task */
        end = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;

        /* Check if the task missed its deadine */
        deadline = first_start + (tsk_counter +1)*PERIOD_TASK_7/ (double)tick_per_second;
        stop_sys = check_deadline(&first_task_flag ,nTask, deadline, end, task_type, task_id, tick_per_second);

        if(stop_sys == TRUE){
          sys_stop_flag = TRUE;
        }else{
          taskrunning_table[task_id] = 0;
          tsk_counter += 1;
        }
			}
		}
	}
}

// TASK 8
rtems_task Task_8(
  rtems_task_argument unused
)
{
  rtems_id          selfid=rtems_task_self();
  rtems_status_code status;
  rtems_name        period_name; 
  rtems_id          RM_period;
  double 	          first_start, start, end, deadline, sys_turn_unhealthy, remaining_time;
  bool 		          first_task_flag = FALSE;
  bool 		          task_fault = FALSE;
  int		            task_id = ID_TASK_8;
  bool		          healthy, stop_sys;
  int 		          task_type = TYPE_TASK_8;
  int		            tsk_counter = 0; /* counter for this task */
  int		            suspendedTask[10];/* Table to save the check the preempted task */
  int		            numberPreemptedTask = 0;
  int		            startTick = 0;
  int 		          suspendedTaskid = 100;
  int		            j = 0;

  /* Random seed*/
  srand(seedseed+task_id);

  /* Create and register period in scheduler */
  period_name = rtems_build_name( 'P', 'E', 'R', '8' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }


  //KHCHEN 03.08, record the period ID for helper function
  period_id[task_id] = RM_period;

	while( 1 ) {
 
    /* Check the internal time of system and call/run this task again at its period */
		status = rtems_rate_monotonic_period( RM_period, PERIOD_TASK_8);

    /* wait for all tasks are released */
    if(AllReady){
      if(sys_stop_flag == TRUE  || experiment_flag ==0){
        /* Delete the task and its period as termination criterion is met */
        status = rtems_rate_monotonic_delete(RM_period);
        if(status != RTEMS_SUCCESSFUL){
          printf("BUG: Cannot delete the period 8\n");
          exit(1);
        }

        running_flag[7]=0;
        status=rtems_task_delete(selfid);
        if(status != RTEMS_SUCCESSFUL){
          printf("BUG: Cannot delete the task 8\n");
          exit(1);
        }		
      }else{
        /* Start time of the task */
        startTick = rtems_clock_get_ticks_since_boot();
        start = startTick  / (double)tick_per_second;

        if(tsk_counter == 0){
          first_start = start;
          }
        
        /* Check if the task preempt the other tasks */
        numberPreemptedTask = check_running_task(suspendedTask);
        
        if(numberPreemptedTask != 0){
          suspendedTaskid = 0;
          for(j=0; j<numberPreemptedTask; j++){
            suspendedTaskid = suspendedTask[j];
            if(preempted_table[0][suspendedTaskid]==0){
              preempted_table[0][suspendedTaskid]= numberPreemptedTask;
              preempted_table[1][suspendedTaskid]= startTick;
            }
          }
        }
       
        taskrunning_table[task_id] = 1;

        if(first_task_flag == FALSE && task_running_flag == FALSE){
          if(sys_fault_flag == FALSE){
            sys_healthy_start = start;
          }else if(sys_fault_flag == TRUE){
            sys_unhealthy_start = start;
          }
          task_running_flag = TRUE;
          first_task_flag = TRUE;
        }

        /* Run for its normal execution time*/
        LOOP(task_8_normal_et,task_id);

        /* Performs fault checking */
        task_fault = task_fault_check(task_8_normal_et);

        if(task_fault == TRUE){
          taskrunning_table[task_id] = 2;
          /* Check if this faulty task affect the health of system */
          healthy = check_busyP(task_id, nTask);
          if(healthy == FALSE && sys_fault_flag == FALSE){
            sys_fault_flag = TRUE;
            sys_turn_unhealthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
            sys_healthy_end = sys_turn_unhealthy;
            sys_unhealthy_start = sys_turn_unhealthy;
            sys_healthy_duration = sys_healthy_end - sys_healthy_start;
            sys_healthy_total_duration += sys_healthy_duration;
          }
          remaining_time = task_8_abnormal_et - task_8_normal_et;
          if(remaining_time != 0)
            LOOP(remaining_time,task_id);
        }

        /* End time of the task*/
        end = rtems_clock_get_ticks_since_boot()/(double)tick_per_second;

        /* Check if the task missed its deadline */
        deadline = first_start + (tsk_counter +1)*PERIOD_TASK_8/ (double)tick_per_second;
        stop_sys = check_deadline(&first_task_flag ,nTask, deadline, end, task_type, task_id, tick_per_second);

        if(stop_sys == TRUE){
          sys_stop_flag = TRUE;
        }else{
          taskrunning_table[task_id] = 0;
          tsk_counter += 1;
        }
			}
		}
	}
}

// TASK 9
rtems_task Task_9(
  rtems_task_argument unused
)
{
  rtems_id          selfid=rtems_task_self();
  rtems_status_code status;
  rtems_name        period_name; 
  rtems_id          RM_period;
  double            first_start, start, end, deadline, sys_turn_unhealthy, remaining_time;
  bool 		          first_task_flag = FALSE;
  bool 		          task_fault = FALSE;
  int		            task_id = ID_TASK_9;
  bool		          healthy, stop_sys;
  int 		          task_type = TYPE_TASK_9;
  int		            tsk_counter = 0; /* counter for this task */
  int		            suspendedTask[10];/* Table to save the check the preempted task */
  int		            numberPreemptedTask = 0;
  int		            startTick = 0;
  int 		          suspendedTaskid = 100;
  int		            j = 0;

  /* Random seed */
  srand(seedseed+task_id);

  /* Create and register period in scheduler */
  period_name = rtems_build_name( 'P', 'E', 'R', '9' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

  //KHCHEN 03.08, record the period ID for helper function
  period_id[task_id] = RM_period;

	while( 1 ) {

    /* Check the internal time of system and call/run this task again at its period */
		status = rtems_rate_monotonic_period( RM_period, PERIOD_TASK_9);
   
    /* wait for all tasks are released */
    if(AllReady){
      if(sys_stop_flag == TRUE  || experiment_flag ==0){
        /* Delete the task and its period as termination criterion is met */
        status = rtems_rate_monotonic_delete(RM_period);
        if(status != RTEMS_SUCCESSFUL){
          printf("BUG: Cannot delete the period 9\n");
          exit(1);
        }

        running_flag[8]=0;
        status=rtems_task_delete(selfid);
        if(status != RTEMS_SUCCESSFUL){
          printf("BUG: Cannot delete the task 9\n");
          exit(1);
        }	
      }else{
        /* Start time of the task*/
        startTick = rtems_clock_get_ticks_since_boot();
        start = startTick  / (double)tick_per_second;

        if(tsk_counter == 0){
          first_start = start;
          }
        
        /* Check if this task preempts the other task */
        numberPreemptedTask = check_running_task(suspendedTask);
        
        if(numberPreemptedTask != 0){
          suspendedTaskid = 0;
          for(j=0; j<numberPreemptedTask; j++){
            suspendedTaskid = suspendedTask[j];
            if(preempted_table[0][suspendedTaskid]==0){
              preempted_table[0][suspendedTaskid]= numberPreemptedTask;
              preempted_table[1][suspendedTaskid]= startTick;
            }
          }
        }

        taskrunning_table[task_id] = 1;
        
        if(first_task_flag == FALSE && task_running_flag == FALSE){
          if(sys_fault_flag == FALSE){
            sys_healthy_start = start;
          }else if(sys_fault_flag == TRUE){
            sys_unhealthy_start = start;
          }
          task_running_flag = TRUE;
          first_task_flag = TRUE;
        }

        /* Run for its normal execution time */
        LOOP(task_9_normal_et,task_id);

        /* Perform fault checking */
        task_fault = task_fault_check(task_9_normal_et);
        
        if(task_fault == TRUE){
          taskrunning_table[task_id] = 2;
          /* Check if this faulty task affect the health of the whole system*/
          healthy = check_busyP(task_id, nTask);
          if(healthy == FALSE  && sys_fault_flag == FALSE){
            sys_fault_flag = TRUE;
            sys_turn_unhealthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
            sys_healthy_end = sys_turn_unhealthy;
            sys_unhealthy_start = sys_turn_unhealthy;
            sys_healthy_duration = sys_healthy_end - sys_healthy_start;
            sys_healthy_total_duration += sys_healthy_duration;
          }
          remaining_time = task_9_abnormal_et - task_9_normal_et;
          if(remaining_time != 0)
            LOOP(remaining_time,task_id);
        }

        /* End time of the task */
        end = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;

        /* Check if the task missed its deadline */
        deadline = first_start + (tsk_counter +1)*PERIOD_TASK_9/ (double)tick_per_second;
        stop_sys = check_deadline(&first_task_flag ,nTask, deadline, end, task_type, task_id, tick_per_second);

        if(stop_sys == TRUE){
          sys_stop_flag = TRUE;
        }else{
          taskrunning_table[task_id] = 0;
          tsk_counter += 1;
        }
	  	}
		}
	}
}

// TASK 10
rtems_task Task_10(
  rtems_task_argument unused
)
{
  rtems_id          selfid=rtems_task_self();
  rtems_status_code status;
  rtems_name        period_name; 
  rtems_id          RM_period;
  double 	          first_start, start, end, deadline, sys_turn_unhealthy, remaining_time;
  bool 		          first_task_flag = FALSE;
  bool 		          task_fault = FALSE;
  int		            task_id = ID_TASK_10;
  bool		          healthy, stop_sys;
  int 		          task_type = TYPE_TASK_10;
  int		            tsk_counter = 0; /* counter for this task */
  int		            suspendedTask[10];/* Table to save the check the preempted task */
  int		            numberPreemptedTask = 0;
  int		            startTick = 0;
  int 		          suspendedTaskid = 100;
  int		            j = 0;

  /* Random seed */
  srand(seedseed+task_id);

  /* Create and register period in scheduler */
  period_name = rtems_build_name( 'P', 'E', '1', '0' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

  //KHCHEN 03.08, record the period ID for helper function
  period_id[task_id] = RM_period;

	while( 1 ) {
 
    /* Check the internal time of system and call/run this task again at its period */
		status = rtems_rate_monotonic_period( RM_period, PERIOD_TASK_10);
    
    /* All tasks are ready when lowest priority task is released */
    AllReady = TRUE;

		if(sys_stop_flag == TRUE){
			/* Delete the task and its period as termination criterion is met */
      status = rtems_rate_monotonic_delete(RM_period);
			if(status != RTEMS_SUCCESSFUL){
				printf("BUG: Cannot delete the period 10\n");
				exit(1);
			}

			running_flag[9]=0;
			status=rtems_task_delete(selfid);
      if(status != RTEMS_SUCCESSFUL){
				printf("BUG: Cannot delete the task 10\n");
				exit(1);
			}		
		}else{
      /* Start time of the task */
			startTick = rtems_clock_get_ticks_since_boot();
			start = startTick  / (double)tick_per_second;

			if(tsk_counter == 0){
				first_start = start;
		   	}
			
      /* Check if the task preempts the other tasks */
			numberPreemptedTask = check_running_task(suspendedTask);
			
			if(numberPreemptedTask != 0){
				suspendedTaskid = 0;
				for(j=0; j<numberPreemptedTask; j++){
					suspendedTaskid = suspendedTask[j];
					if(preempted_table[0][suspendedTaskid]==0){
						preempted_table[0][suspendedTaskid]= numberPreemptedTask;
						preempted_table[1][suspendedTaskid]= startTick;
					}
				}
			}
	   
			taskrunning_table[task_id] = 1;

			if(first_task_flag == FALSE && task_running_flag == FALSE){
				if(sys_fault_flag == FALSE){
					sys_healthy_start = start;
				}else if(sys_fault_flag == TRUE){
					sys_unhealthy_start = start;
				}
				task_running_flag = TRUE;
				first_task_flag = TRUE;
			}

      /* Run for its normal execution time */
			LOOP(task_10_normal_et,task_id);

      /* Perform fault checking */
			task_fault = task_fault_check(task_10_normal_et);

			if(task_fault == TRUE){
		   	taskrunning_table[task_id] = 2;
        /* Check if this faulty task affect the health of the system */
				healthy = check_busyP(task_id, nTask);
				if(healthy == FALSE && sys_fault_flag == FALSE){
					sys_fault_flag = TRUE;
					sys_turn_unhealthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
					sys_healthy_end = sys_turn_unhealthy;
					sys_unhealthy_start = sys_turn_unhealthy;
					sys_healthy_duration = sys_healthy_end - sys_healthy_start;
					sys_healthy_total_duration += sys_healthy_duration;
				}
				remaining_time = task_10_abnormal_et - task_10_normal_et;
				if(remaining_time != 0){
					LOOP(remaining_time,task_id);
        }
			}

      /* End time of the task */
			end = rtems_clock_get_ticks_since_boot()/(double)tick_per_second;

      /* Check if the task missed its deadline */
			deadline = first_start + (tsk_counter +1)*PERIOD_TASK_10/ (double)tick_per_second;
			stop_sys = check_deadline(&first_task_flag ,nTask, deadline, end, task_type, task_id, tick_per_second);

			if(stop_sys == TRUE){
				sys_stop_flag = TRUE;
			}else{
				taskrunning_table[task_id] = 0;
				tsk_counter += 1;
				 
				if(tsk_counter == testnumber){
			    /* Termination criterion is met*/		
					experiment_flag=0;
					status = rtems_rate_monotonic_delete(RM_period);
					if(status != RTEMS_SUCCESSFUL){
						printf("BUG: Cannot delete the period 10\n");
						exit(1);
					}

					running_flag[9]=0;
					status=rtems_task_delete(selfid);
					if(status != RTEMS_SUCCESSFUL){
						printf("BUG: Cannot delete the task 10\n");
						exit(1);
					}
				}
			}
		}
	}
}
