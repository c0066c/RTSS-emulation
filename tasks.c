/* 
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/*  updated for triple test, 20003/11/06, Erik Adli */

#include "system.h"
#include "samples.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
//#include <bsp.h>
//#include <bsp/gpio.h> /* Calls the BSP gpio library */

/* CPU usage and Rate monotonic manger statistics */
#include "rtems/cpuuse.h"
//
// Periods for the various tasks [seconds]
#define PERIOD_TASK_1           tsk[0].period
#define PERIOD_TASK_2		tsk[1].period
#define PERIOD_TASK_3          	tsk[2].period
#define PERIOD_TASK_4           tsk[3].period
#define PERIOD_TASK_5		tsk[4].period
#define PERIOD_TASK_6          	tsk[5].period
#define PERIOD_TASK_7           tsk[6].period
#define PERIOD_TASK_8		tsk[7].period
#define PERIOD_TASK_9          	tsk[8].period
#define PERIOD_TASK_10          tsk[9].period


// Periods for the various tasks 
#define ID_TASK_1           	tsk[0].id
#define ID_TASK_2		tsk[1].id
#define ID_TASK_3          	tsk[2].id
#define ID_TASK_4           	tsk[3].id
#define ID_TASK_5		tsk[4].id
#define ID_TASK_6          	tsk[5].id
#define ID_TASK_7           	tsk[6].id
#define ID_TASK_8		tsk[7].id
#define ID_TASK_9          	tsk[8].id
#define ID_TASK_10          	tsk[9].id

// Task type for the various tasks : 0 means hard real time task, 1 means soft real time task 
#define TYPE_TASK_1           	tsk[0].task_type
#define TYPE_TASK_2		tsk[1].task_type
#define TYPE_TASK_3          	tsk[2].task_type
#define TYPE_TASK_4           	tsk[3].task_type
#define TYPE_TASK_5		tsk[4].task_type
#define TYPE_TASK_6          	tsk[5].task_type
#define TYPE_TASK_7           	tsk[6].task_type
#define TYPE_TASK_8		tsk[7].task_type
#define TYPE_TASK_9          	tsk[8].task_type
#define TYPE_TASK_10          	tsk[9].task_type


// Execution time for each task
#define task_1_normal_et	tsk[0].normal_et
#define task_1_abnormal_et	tsk[0].abnormal_et
#define task_2_normal_et	tsk[1].normal_et
#define task_2_abnormal_et	tsk[1].abnormal_et
#define task_3_normal_et	tsk[2].normal_et
#define task_3_abnormal_et	tsk[2].abnormal_et
#define task_4_normal_et	tsk[3].normal_et
#define task_4_abnormal_et	tsk[3].abnormal_et
#define task_5_normal_et	tsk[4].normal_et
#define task_5_abnormal_et	tsk[4].abnormal_et
#define task_6_normal_et	tsk[5].normal_et
#define task_6_abnormal_et	tsk[5].abnormal_et
#define task_7_normal_et	tsk[6].normal_et
#define task_7_abnormal_et	tsk[6].abnormal_et
#define task_8_normal_et	tsk[7].normal_et
#define task_8_abnormal_et	tsk[7].abnormal_et
#define task_9_normal_et	tsk[8].normal_et
#define task_9_abnormal_et	tsk[8].abnormal_et
#define task_10_normal_et	tsk[9].normal_et
#define task_10_abnormal_et	tsk[9].abnormal_et

// Number of task
#define nTask 			ntask

// TASK 1
rtems_task Task_1(
  rtems_task_argument unused
)
{
  rtems_status_code status;
  rtems_name        period_name; 
  rtems_id          RM_period;
  rtems_id selfid=rtems_task_self();
  double            start, end; //sttime, entime;
  int		    i=0;
  int		    j=0;
  bool 		    first_task_flag = FALSE;
  bool 		    task_fault = FALSE;
  //double	    delta_start;
  int		    task_id = ID_TASK_1;
  bool		    healthy;
  double 	    sys_turn_unhealthy;
  double	    remaining_time;
  int 		    task_type = TYPE_TASK_1; 
  double	    first_start;
  int		    tsk_counter = 0;
  double	    deadline;
  bool		    stop_sys;
  bool              meet = FALSE;
  int               totalruntasks = 0;
  int		    suspendedTask[10];
  int		    numberPreemptedTask = 0;
  int		    startTick = 0;
  int		    currentTick = 0;
  int 		    suspendedTaskid = 100;

  /* Initializes the GPIO API */
  tick_per_second = rtems_clock_get_ticks_per_second();

  /*create period*/

  period_name = rtems_build_name( 'P', 'E', 'R', '1' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

	while( 1 ) {

    /*this part for release offset*/
		status = rtems_rate_monotonic_period( RM_period,PERIOD_TASK_1);

/*    if( RTEMS_TIMEOUT == status ) {
	printf("Task 1 missed period!\n");
	break;
    }
    
*/  
		if(sys_stop_flag == TRUE || experiment_flag == 0){
			
			totalruntasks = 0;
			for(i=0; i<nTask; i++){
				totalruntasks +=running_flag[i];
			}
			
			
			sys_totalruntime = sys_healthy_total_duration + sys_unhealthy_total_duration;
			sys_totalhealthy_percentage = (sys_healthy_total_duration/ sys_totalruntime)*100;
			sys_totalunhealthy_percentage = (sys_unhealthy_total_duration/ sys_totalruntime)*100;

			if(totalruntasks == 1){//till all tasks are deleted besides task 1

				running_flag[0]=0;

				if(sys_stop_flag == TRUE)
					printf("Failed to meet deadline.\n");

				//printf("Total system runtime is %.6f second.\n", sys_totalruntime);
				//printf("Total system runtime in healthy condition is %.6f second and its percentage is %.3f.\n",sys_healthy_total_duration,sys_totalhealthy_percentage);
				//printf("Total system runtime in unhealthy condition is %.6f second and its percentage is %.3f.\n", sys_unhealthy_total_duration, sys_totalunhealthy_percentage);
				printf("\nSystem healthy percentage is %.6f ",sys_totalhealthy_percentage);

				status = rtems_task_resume(inittask_id);
 				if ( status != RTEMS_SUCCESSFUL) {
					printf("BUG::init task cannot be resumed\n");
					exit(1);
				}

				status = rtems_rate_monotonic_delete(RM_period);
				if(status != RTEMS_SUCCESSFUL){
					printf("BUG: Cannot delete the period 1\n");
					exit(1);
				}
                  
				status=rtems_task_delete(selfid);
				if(status != RTEMS_SUCCESSFUL){
					printf("BUG: Cannot delete the task 1\n");
					exit(1);
				}

				printf("%.3f ", sys_totalhealthy_percentage);
			}else{
				//printf("Trying to delete task 1 and totalruntasks is %i\n", totalruntasks);
			}
		}else{
					
			//printf("checkpoint t1.\n");
			startTick = rtems_clock_get_ticks_since_boot();
			start = startTick  / (double)tick_per_second;

			if(tsk_counter == 0){
				first_start = start;
		   	}
			
			numberPreemptedTask = check_running_task(&suspendedTask);
			
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

	    		//printf("Task 1 starts current second %.6f.\n", start);

			LOOP(task_1_normal_et,task_id);
			//LOOPA(task_1_normal_et);
      //printf("current tick is %i with exe tick %.6f .\n",rtems_clock_get_ticks_since_boot(),task_1_normal_et);
			//task_fault = TRUE;
			task_fault = task_fault_check(task_1_normal_et);
		    	//printf(task_fault? "Task 1 is faulty. \n" : "Task 1 has no fault.\n"  );
		    	//printf("Task 1 checked fault\n");
	    		//printf("Task 1 finished fault check.\n");
			if(task_fault == TRUE){
				
				printf("task 1 is faulty.\n");
				healthy = check_busyP(task_id, nTask);
				if(healthy == FALSE  && sys_fault_flag == FALSE){
					sys_fault_flag = TRUE;
					sys_turn_unhealthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
					sys_healthy_end = sys_turn_unhealthy;
					sys_unhealthy_start = sys_turn_unhealthy;
					sys_healthy_duration = sys_healthy_end - sys_healthy_start;
					sys_healthy_total_duration += sys_healthy_duration;
				   //printf("System turned unhealthy in task %d at %.6f seconds.\n",task_id+1 , sys_turn_unhealthy);
		//		   printf("System was healthy with duration of %.6f second. \n", sys_healthy_duration);
				}

				remaining_time = task_1_abnormal_et - task_1_normal_et;
				if(remaining_time != 0)
					LOOP(remaining_time,task_id);

			}else{
				//printf("			task 1 is not faulty.\n");
			}

			end = rtems_clock_get_ticks_since_boot()/(double)tick_per_second;

			//printf("					Task 1 ends at current second  %.6f .\n", end);
			deadline = first_start + (tsk_counter +1)*PERIOD_TASK_1;
			stop_sys = check_deadline(&first_task_flag ,nTask, deadline, end, task_type, task_id, tick_per_second, &meet);
      
			if(stop_sys == TRUE){
				sys_stop_flag = TRUE;
				//printf("sys_stop_flag set as true\n");
				//break;
			}else{
				//printf("Task 1 is executing at %d time with deadline at %.6f second.\n\n",tsk_counter, deadline);
		    		taskrunning_table[task_id] = 0;

		    		if(meet == TRUE){
	    				deltastart_table[task_id] = 0;
	    				d_t[task_id] = 0;
	    			}

	    			tsk_counter += 1;

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
  rtems_id selfid=rtems_task_self();
  //rtems_interval    tick_per_second;
  double 	    start, end;
  //int               i=0;
  bool 		    first_task_flag = FALSE;
  bool 		    task_fault = FALSE;
  //double	    delta_start;
  int		    task_id = ID_TASK_2;
  bool		    healthy;
  double 	    sys_turn_unhealthy;
  double	    remaining_time;
  int 		    task_type = TYPE_TASK_2;
  double	    first_start;
  int		    tsk_counter = 0;
  double	    deadline;
  bool		    stop_sys;
  bool              meet = FALSE;
  int		    suspendedTask[10];
  int		    numberPreemptedTask = 0;
  int		    startTick = 0;
  int 		    suspendedTaskid = 100;
  int		    j = 0;

  /* Initializes the GPIO API */
  tick_per_second = rtems_clock_get_ticks_per_second();

  /*create period*/

  period_name = rtems_build_name( 'P', 'E', 'R', '2' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

	while( 1 ) {
 
    /*this part for release offset*/
    status = rtems_rate_monotonic_period( RM_period, PERIOD_TASK_2);

/*   if( RTEMS_TIMEOUT == status ) {
	printf("Task 2 missed period!\n");
	break;
    }
*/
		if(sys_stop_flag == TRUE || experiment_flag == 0){
			running_flag[1]=0;
			//printf("deleting task 2\n");
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
		
			//printf("checkpoint t2.\n");
			startTick = rtems_clock_get_ticks_since_boot();
			start = startTick  / (double)tick_per_second;

			if(tsk_counter == 0){
				first_start = start;
		   	}
			
			numberPreemptedTask = check_running_task(&suspendedTask);
			
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
			//printf("Task 2 starts at current second %.6f.\n", start);

			//LOOPA(task_2_normal_et);
			LOOP(task_2_normal_et,task_id);

			task_fault = task_fault_check(task_2_normal_et);

			//printf(task_fault? "Task 2 is faulty. \n" : "Task 2 has no fault.\n"  );

			if(task_fault == TRUE){
#if 0
				delta_start = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;

				for(i=0; i < nTask; i++){
					if(taskrunning_table[i] == 1 && deltastart_table[i] == 0)
						deltastart_table[i] = delta_start;
					else if(taskrunning_table[i] == 1 && deltastart_table[i] != 0)
						d_t[i] = delta_start - deltastart_table[i];	
				}
#endif
				printf("task 2 is faulty.\n");
				healthy = check_busyP(task_id, nTask);
				if(healthy == FALSE && sys_fault_flag == FALSE){
					sys_fault_flag = TRUE;
					sys_turn_unhealthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
					sys_healthy_end = sys_turn_unhealthy;
					sys_unhealthy_start = sys_turn_unhealthy;
					sys_healthy_duration = sys_healthy_end - sys_healthy_start;
					sys_healthy_total_duration += sys_healthy_duration;
					//printf("System turned unhealthy in task %d at %.6f seconds.\n",task_id+1 , sys_turn_unhealthy);
					//printf("System was healthy with duration of %.6f second. \n", sys_healthy_duration);
				}

				remaining_time = task_2_abnormal_et - task_2_normal_et;
				if(remaining_time != 0)
					LOOP(remaining_time,task_id);
			}

			end = rtems_clock_get_ticks_since_boot()/(double)tick_per_second;

			//printf("					Task 2 ends at current second %.6f.\n", end);

			deadline = first_start + (tsk_counter +1)*PERIOD_TASK_2;

			stop_sys = check_deadline(&first_task_flag ,nTask, deadline, end, task_type, task_id, tick_per_second, &meet);
			if(stop_sys == TRUE){
				sys_stop_flag = TRUE;
				//printf("sys_stop_flag set as true\n");
				//break;
			}else{
				    
				//printf("Task 2 is executing at %d time with deadline at %.6f second.\n\n",tsk_counter, deadline);
				taskrunning_table[task_id] = 0;
#if 0
				if(meet == TRUE){
					deltastart_table[task_id] = 0;
					d_t[task_id] = 0;
				}
#endif
				tsk_counter += 1;
				
/*				if(experiment_flag ==0){
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
				}
*/			}
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
  rtems_id selfid=rtems_task_self();
  //rtems_interval    tick_per_second;
  double            start, end;
  //int		    i=0;
  bool 		    first_task_flag = FALSE;
  bool 		    task_fault = FALSE;
  //double	    delta_start;
  int		    task_id = ID_TASK_3;
  bool		    healthy;
  double 	    sys_turn_unhealthy;
  double	    remaining_time;
  int 		    task_type = TYPE_TASK_3;
  double	    first_start;
  int		    tsk_counter = 0;
  double	    deadline;
  bool		    stop_sys;
  bool              meet = FALSE;
  int		    suspendedTask[10];
  int		    numberPreemptedTask = 0;
  int		    startTick = 0;
  int 		    suspendedTaskid = 100;
  int		    j = 0;

  /* Initializes the GPIO API */
  tick_per_second = rtems_clock_get_ticks_per_second();

  /*create period*/

  period_name = rtems_build_name( 'P', 'E', 'R', '3' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

	while( 1 ) {

    /*this part for release offset*/
		status = rtems_rate_monotonic_period( RM_period, PERIOD_TASK_3);

/*    if( RTEMS_TIMEOUT == status ) {
			printf("Task 3 missed period!\n");
			break;
		}
  
*/    
		if(sys_stop_flag == TRUE || experiment_flag == 0){
			//printf("deleting task 3\n");
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
 			

			//printf("checkpoint t3.\n");
			startTick = rtems_clock_get_ticks_since_boot();
			start = startTick  / (double)tick_per_second;

			if(tsk_counter == 0){
				first_start = start;
		   	}
			
			numberPreemptedTask = check_running_task(&suspendedTask);
			
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

			//printf("Task 3 starts at current second %.6f.\n", start);
	  
			//LOOP(task_3_normal_et);    
			LOOP(task_3_normal_et,task_id);

			task_fault = task_fault_check(task_3_normal_et);
			//printf(task_fault? "Task 3 is faulty. \n" : "Task 3 has no fault.\n"  );
	    
			if(task_fault == TRUE){
#if 0
				delta_start = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;

				for(i=0; i < nTask; i++){
					if(taskrunning_table[i] == 1 && deltastart_table[i] == 0)
						deltastart_table[i] = delta_start;
		   		else if(taskrunning_table[i] == 1 && deltastart_table[i] != 0)
		      		d_t[i] = delta_start - deltastart_table[i];	
				}
#endif
				printf("task 3 is faulty.\n");
				healthy = check_busyP(task_id, nTask);
				if(healthy == FALSE  && sys_fault_flag == FALSE){
		   		sys_fault_flag = TRUE;
					sys_turn_unhealthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
					sys_healthy_end = sys_turn_unhealthy;
					sys_unhealthy_start = sys_turn_unhealthy;
					sys_healthy_duration = sys_healthy_end - sys_healthy_start;
					sys_healthy_total_duration += sys_healthy_duration;
				   //printf("System turned unhealthy in task %d at %.6f seconds.\n",task_id+1 , sys_turn_unhealthy);
				   //printf("System was healthy with duration of %.6f second. \n", sys_healthy_duration);
				}

				remaining_time = task_3_abnormal_et - task_3_normal_et;
				if(remaining_time != 0)
					LOOP(remaining_time,task_id);
			}

			end = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;

		//	printf("					Task 3 ends at current second %.6f.\n", end);

			deadline = first_start + (tsk_counter +1)*PERIOD_TASK_3;

			stop_sys = check_deadline(&first_task_flag ,nTask, deadline, end, task_type, task_id, tick_per_second, &meet);
	    	if(stop_sys == TRUE){
				sys_stop_flag = TRUE;
				//printf("sys_stop_flag set as true\n");
				//break;
			}else{
				//printf("Task 3 is executing at %d time with deadline at %.6f second.\n\n",tsk_counter, deadline);
				taskrunning_table[task_id] = 0;
#if 0
				if(meet == TRUE){
					deltastart_table[task_id] = 0;
					d_t[task_id] = 0;
				}
#endif
				tsk_counter += 1;
/*				if(experiment_flag ==0){
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
				}
*/			}
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
  rtems_id selfid=rtems_task_self();
  //rtems_interval    tick_per_second;
  double 	    start, end;
  //int               i=0;
  bool 		    first_task_flag = FALSE;
  bool 		    task_fault = FALSE;
  //double	    delta_start;
  int		    task_id = ID_TASK_4;
  bool		    healthy;
  double 	    sys_turn_unhealthy;
  double	    remaining_time;
  int 		    task_type = TYPE_TASK_4;
  double	    first_start;
  int		    tsk_counter = 0;
  double	    deadline;
  bool		    stop_sys;
  bool              meet = FALSE;
  int		    suspendedTask[10];
  int		    numberPreemptedTask = 0;
  int		    startTick = 0;
  int 		    suspendedTaskid = 100;
  int		    j = 0;

  /* Initializes the GPIO API */
  tick_per_second = rtems_clock_get_ticks_per_second();

  /*create period*/

  period_name = rtems_build_name( 'P', 'E', 'R', '4' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

	while( 1 ) {
 
		/*this part for release offset*/
		status = rtems_rate_monotonic_period( RM_period, PERIOD_TASK_4);

/*   	if( RTEMS_TIMEOUT == status ) {
			printf("Task 4 missed period!\n");
			break;
    	}

*/		
		if(sys_stop_flag == TRUE || experiment_flag ==0 ){
			//printf("deleting task 4\n");
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

			//printf("checkpoint t4.\n");
			startTick = rtems_clock_get_ticks_since_boot();
			start = startTick  / (double)tick_per_second;

			if(tsk_counter == 0){
				first_start = start;
		   	}
			
			numberPreemptedTask = check_running_task(&suspendedTask);
			
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

			//printf("Task 4 starts at current second %.6f.\n", start);

			//LOOP(task_4_normal_et);   
			LOOP(task_4_normal_et,task_id);
			task_fault = task_fault_check(task_4_normal_et);

			//printf(task_fault? "Task 4 is faulty. \n" : "Task 4 has no fault.\n"  );

			if(task_fault == TRUE){
#if 0
				delta_start = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;

				for(i=0; i < nTask; i++){
					if(taskrunning_table[i] == 1 && deltastart_table[i] == 0)
						deltastart_table[i] = delta_start;
					else if(taskrunning_table[i] == 1 && deltastart_table[i] != 0)
						d_t[i] = delta_start - deltastart_table[i];	
				}
#endif
				printf("task 4 is faulty.\n");
				healthy = check_busyP(task_id, nTask);
				
				if(healthy == FALSE && sys_fault_flag == FALSE){
					sys_fault_flag = TRUE;
					sys_turn_unhealthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
					sys_healthy_end = sys_turn_unhealthy;
					sys_unhealthy_start = sys_turn_unhealthy;
					sys_healthy_duration = sys_healthy_end - sys_healthy_start;
					sys_healthy_total_duration += sys_healthy_duration;
				   //printf("System turned unhealthy in task %d at %.6f seconds.\n",task_id+1 , sys_turn_unhealthy);
				   //printf("System was healthy with duration of %.6f second. \n", sys_healthy_duration);
				}

				remaining_time = task_4_abnormal_et - task_4_normal_et;
				if(remaining_time != 0)
					LOOP(remaining_time,task_id);
			}

			end = rtems_clock_get_ticks_since_boot()/(double)tick_per_second;

			//printf("					Task 4 ends at current second %.6f.\n", end);
	 
			deadline = first_start + (tsk_counter +1)*PERIOD_TASK_4;

			stop_sys = check_deadline(&first_task_flag ,nTask, deadline, end, task_type, task_id, tick_per_second, &meet);
			if(stop_sys == TRUE){
				sys_stop_flag = TRUE;
				//printf("sys_stop_flag set as true\n");
				//break;
			}else{
				//printf("Task 4 is executing at %d time with deadline at %.6f second.\n\n",tsk_counter, deadline);
				taskrunning_table[task_id] = 0;
#if 0
	    		if(meet == TRUE){
	    			deltastart_table[task_id] = 0;
	    			d_t[task_id] = 0;
	    		}
#endif
				tsk_counter += 1;
/*				if(experiment_flag ==0){
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
				}
*/			}
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
  rtems_id selfid=rtems_task_self();
  //rtems_interval    tick_per_second;
  double            start, end;
  //int		    i=0;
  bool 		    first_task_flag = FALSE;
  bool 		    task_fault = FALSE;
  //double	    delta_start;
  int		    task_id = ID_TASK_5;
  bool		    healthy;
  double 	    sys_turn_unhealthy;
  double	    remaining_time;
  int 		    task_type = TYPE_TASK_5;
  double	    first_start;
  int		    tsk_counter = 0;
  double	    deadline;
  bool		    stop_sys;
  bool              meet = FALSE;
  int		    suspendedTask[10];
  int		    numberPreemptedTask = 0;
  int		    startTick = 0;
  int 		    suspendedTaskid = 100;
  int 		    j = 0;

  /* Initializes the GPIO API */
  tick_per_second = rtems_clock_get_ticks_per_second();

  /*create period*/

  period_name = rtems_build_name( 'P', 'E', 'R', '5' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

	while( 1 ) {

		/*this part for release offset*/
		status = rtems_rate_monotonic_period( RM_period, PERIOD_TASK_5);

/*    if( RTEMS_TIMEOUT == status ) {
			printf("Task 5 missed period!\n");
			break;
    	}
*/   
 
		if(sys_stop_flag == TRUE  || experiment_flag ==0 ){
			//printf("deleting task 5\n");
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
					
			//printf("checkpoint t5.\n");
			startTick = rtems_clock_get_ticks_since_boot();
			start = startTick  / (double)tick_per_second;

			if(tsk_counter == 0){
				first_start = start;
		   	}
			
			numberPreemptedTask = check_running_task(&suspendedTask);
			
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

			//printf("Task 5 starts at current second %.6f.\n", start);
	  
			//LOOP(task_5_normal_et);    
			LOOP(task_5_normal_et,task_id);

			task_fault = task_fault_check(task_5_normal_et);
			//printf(task_fault? "Task 5 is faulty. \n" : "Task 5 has no fault.\n"  );
	    
			if(task_fault == TRUE){
#if 0
				delta_start = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;

				for(i=0; i < nTask; i++){
					if(taskrunning_table[i] == 1 && deltastart_table[i] == 0)
						deltastart_table[i] = delta_start;
		   		else if(taskrunning_table[i] == 1 && deltastart_table[i] != 0)
		      		d_t[i] = delta_start - deltastart_table[i];	
				}
#endif
				printf("task 5 is faulty.\n");
				healthy = check_busyP(task_id, nTask);
				if(healthy == FALSE  && sys_fault_flag == FALSE){
					sys_fault_flag = TRUE;
					sys_turn_unhealthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
					sys_healthy_end = sys_turn_unhealthy;
					sys_unhealthy_start = sys_turn_unhealthy;
					sys_healthy_duration = sys_healthy_end - sys_healthy_start;
					sys_healthy_total_duration += sys_healthy_duration;
				   //printf("System turned unhealthy in task %d at %.6f seconds.\n",task_id+1 , sys_turn_unhealthy);
				   //printf("System was healthy with duration of %.6f second. \n", sys_healthy_duration);
				}

				remaining_time = task_5_abnormal_et - task_5_normal_et;
				if(remaining_time != 0)
					LOOP(remaining_time,task_id);
			}

			end = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;

			//printf("					Task 5 ends at current second %.6f. \n", end);

			deadline = first_start + (tsk_counter +1)*PERIOD_TASK_5;

			stop_sys = check_deadline(&first_task_flag ,nTask, deadline, end, task_type, task_id, tick_per_second, &meet);
			if(stop_sys == TRUE){
				sys_stop_flag = TRUE;
				//printf("sys_stop_flag set as true\n");
				//break;
			}else{

				//printf("Task 5 is executing at %d time with deadline at %.6f second.\n\n",tsk_counter, deadline);
				taskrunning_table[task_id] = 0;
#if 0
				if(meet == TRUE){
					deltastart_table[task_id] = 0;
					d_t[task_id] = 0;
				}
#endif
				tsk_counter += 1;
/*				if(experiment_flag ==0){
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
				}
*/			}
		}
	}
}

// TASK 6
rtems_task Task_6(
  rtems_task_argument unused
)
{
  rtems_id selfid=rtems_task_self();
  rtems_status_code status;
  rtems_name        period_name; 
  rtems_id          RM_period;
  //rtems_interval    tick_per_second;
  double 	    start, end;
  //int               i=0;
  bool 		    first_task_flag = FALSE;
  bool 		    task_fault = FALSE;
  //double	    delta_start;
  int		    task_id = ID_TASK_6;
  bool		    healthy;
  double 	    sys_turn_unhealthy;
  double	    remaining_time;
  int 		    task_type = TYPE_TASK_6;
  double	    first_start;
  int		    tsk_counter = 0;
  double	    deadline;
  bool		    stop_sys;
  bool              meet = FALSE;
  int		    suspendedTask[10];
  int		    numberPreemptedTask = 0;
  int		    startTick = 0;
  int 		    suspendedTaskid = 100;
  int		    j = 0;

  /* Initializes the GPIO API */
  tick_per_second = rtems_clock_get_ticks_per_second();

  /*create period*/

  period_name = rtems_build_name( 'P', 'E', 'R', '6' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

	while( 1 ) {
 
		/*this part for release offset*/
		status = rtems_rate_monotonic_period( RM_period, PERIOD_TASK_6);

/* 	if( RTEMS_TIMEOUT == status ) {
			printf("Task 6 missed period!\n");
			break;
		}  
*/		
		if(sys_stop_flag == TRUE  || experiment_flag ==0){
			//printf("deleting task 6\n");
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
 
			//printf("checkpoint t6.\n");
			startTick = rtems_clock_get_ticks_since_boot();
			start = startTick  / (double)tick_per_second;

			if(tsk_counter == 0){
				first_start = start;
		   	}
			
			numberPreemptedTask = check_running_task(&suspendedTask);
			
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
	 
			//printf("Task 6 starts at current second %.6f.\n", start);

			//LOOP(task_6_normal_et);   
			LOOP(task_6_normal_et,task_id);

			task_fault = task_fault_check(task_6_normal_et);

			//printf(task_fault? "Task 6 is faulty. \n" : "Task 6 has no fault.\n"  );

			if(task_fault == TRUE){
#if 0
				delta_start = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;

				for(i=0; i < nTask; i++){
					if(taskrunning_table[i] == 1 && deltastart_table[i] == 0)
						deltastart_table[i] = delta_start;
					else if(taskrunning_table[i] == 1 && deltastart_table[i] != 0)
						d_t[i] = delta_start - deltastart_table[i];	
				}
#endif
				printf("task 6 is faulty.\n");
				healthy = check_busyP(task_id, nTask);
				if(healthy == FALSE && sys_fault_flag == FALSE){
					sys_fault_flag = TRUE;
					sys_turn_unhealthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
					sys_healthy_end = sys_turn_unhealthy;
					sys_unhealthy_start = sys_turn_unhealthy;
					sys_healthy_duration = sys_healthy_end - sys_healthy_start;
					sys_healthy_total_duration += sys_healthy_duration;
					//printf("System turned unhealthy in task %d at %.3f seconds.\n",task_id+1 , sys_turn_unhealthy);
					//printf("System was healthy with duration of %.3f second. \n", sys_healthy_duration);
				}

				remaining_time = task_6_abnormal_et - task_6_normal_et;
				if(remaining_time != 0)
					LOOP(remaining_time,task_id);
		    }

			end = rtems_clock_get_ticks_since_boot()/(double)tick_per_second;

			//printf("					Task 6 ends at current second %.6f.\n", end);
	 
			deadline = first_start + (tsk_counter +1)*PERIOD_TASK_6;

			stop_sys = check_deadline(&first_task_flag ,nTask, deadline, end, task_type, task_id, tick_per_second, &meet);
			if(stop_sys == TRUE){
				sys_stop_flag = TRUE;
				//printf("sys_stop_flag set as true\n");
				//break;
			}else{
				//printf("Task 6 is executing at %d time with deadline at %.3f second.\n\n",tsk_counter, deadline);
				taskrunning_table[task_id] = 0;
#if 0
				if(meet == TRUE){
					deltastart_table[task_id] = 0;
					d_t[task_id] = 0;
				}
#endif
				tsk_counter += 1;
/*				if(experiment_flag ==0){
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
				}
*/			}
		}
	}
}

// TASK 7
rtems_task Task_7(
  rtems_task_argument unused
)
{
  rtems_id selfid=rtems_task_self();
  rtems_status_code status;
  rtems_name        period_name; 
  rtems_id          RM_period;
  //rtems_interval    tick_per_second;
  double            start, end;
  //int		    i=0;
  bool 		    first_task_flag = FALSE;
  bool 		    task_fault = FALSE;
  //double	    delta_start;
  int		    task_id = ID_TASK_7;
  bool		    healthy;
  double 	    sys_turn_unhealthy;
  double	    remaining_time;
  int 		    task_type = TYPE_TASK_7;
  double	    first_start;
  int		    tsk_counter = 0;
  double	    deadline;
  bool		    stop_sys;
  bool              meet = FALSE;
  int		    suspendedTask[10];
  int		    numberPreemptedTask = 0;
  int		    startTick = 0;
  int 		    suspendedTaskid = 100;
  int		    j = 0;

  /* Initializes the GPIO API */
  tick_per_second = rtems_clock_get_ticks_per_second();

  /*create period*/

  period_name = rtems_build_name( 'P', 'E', 'R', '7' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

	while( 1 ) {

		/*this part for release offset*/
		status = rtems_rate_monotonic_period( RM_period, PERIOD_TASK_7);

/*		if( RTEMS_TIMEOUT == status ) {
			printf("Task 7 missed period!\n");
			break;
    	}
*/    
  
		if(sys_stop_flag == TRUE  || experiment_flag ==0){
			//printf("deleting task 7\n");
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

			//printf("checkpoint t7.\n");
			startTick = rtems_clock_get_ticks_since_boot();
			start = startTick  / (double)tick_per_second;

			if(tsk_counter == 0){
				first_start = start;
		   	}
			
			numberPreemptedTask = check_running_task(&suspendedTask);
			
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

			//printf("Task 7 starts at current second %.6f. \n", start);
	  
			//LOOP(task_7_normal_et);    
			LOOP(task_7_normal_et,task_id);

			task_fault = task_fault_check(task_7_normal_et);
			//printf(task_fault? "Task 7 is faulty. \n" : "Task 7 has no fault.\n"  );
	    
			if(task_fault == TRUE){
#if 0
				delta_start = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;

				for(i=0; i < nTask; i++){
					if(taskrunning_table[i] == 1 && deltastart_table[i] == 0)
						deltastart_table[i] = delta_start;
					else if(taskrunning_table[i] == 1 && deltastart_table[i] != 0)
						d_t[i] = delta_start - deltastart_table[i];	
				}
#endif
				printf("task 7 is faulty.\n");
				healthy = check_busyP(task_id, nTask);
				if(healthy == FALSE  && sys_fault_flag == FALSE){
					sys_fault_flag = TRUE;
					sys_turn_unhealthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
					sys_healthy_end = sys_turn_unhealthy;
					sys_unhealthy_start = sys_turn_unhealthy;
					sys_healthy_duration = sys_healthy_end - sys_healthy_start;
					sys_healthy_total_duration += sys_healthy_duration;
					//printf("System turned unhealthy in task %d at %.3f seconds.\n",task_id+1 , sys_turn_unhealthy);
					//printf("System was healthy with duration of %.3f second. \n", sys_healthy_duration);
				}

				remaining_time = task_7_abnormal_et - task_7_normal_et;
				if(remaining_time != 0)
					LOOP(remaining_time,task_id);
			}

			end = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;

			//printf("					Task 7 ends at current second %.6f. \n", end);

			deadline = first_start + (tsk_counter +1)*PERIOD_TASK_7;

			stop_sys = check_deadline(&first_task_flag ,nTask, deadline, end, task_type, task_id, tick_per_second, &meet);
			if(stop_sys == TRUE){
				sys_stop_flag = TRUE;
				//printf("sys_stop_flag set as true\n");
				//break;
			}else{
				//printf("Task 7 is executing at %d time with deadline at %.3f second.\n\n",tsk_counter, deadline);
				taskrunning_table[task_id] = 0;
#if 0
				if(meet == TRUE){
					deltastart_table[task_id] = 0;
					d_t[task_id] = 0;
				}
#endif
				tsk_counter += 1;
/*				if(experiment_flag ==0){
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
				}
*/			}
		}
	}
}

// TASK 8
rtems_task Task_8(
  rtems_task_argument unused
)
{
  rtems_id selfid=rtems_task_self();
  rtems_status_code status;
  rtems_name        period_name; 
  rtems_id          RM_period;
  //rtems_interval    tick_per_second;
  double 	    start, end;
  //int               i=0;
  bool 		    first_task_flag = FALSE;
  bool 		    task_fault = FALSE;
  //double	    delta_start;
  int		    task_id = ID_TASK_8;
  bool		    healthy;
  double 	    sys_turn_unhealthy;
  double	    remaining_time;
  int 		    task_type = TYPE_TASK_8;
  double	    first_start;
  int		    tsk_counter = 0;
  double	    deadline;
  bool		    stop_sys;
  bool              meet = FALSE;
  int		    suspendedTask[10];
  int		    numberPreemptedTask = 0;
  int		    startTick = 0;
  int 		    suspendedTaskid = 100;
  int		    j = 0;

  /* Initializes the GPIO API */
  tick_per_second = rtems_clock_get_ticks_per_second();

  /*create period*/

  period_name = rtems_build_name( 'P', 'E', 'R', '8' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

	while( 1 ) {
 
		/*this part for release offset*/
		status = rtems_rate_monotonic_period( RM_period, PERIOD_TASK_8);

/*		if( RTEMS_TIMEOUT == status ) {
			printf("Task 8 missed period!\n");
			break;
		}
*/
	
		if(sys_stop_flag == TRUE  || experiment_flag ==0){
			//printf("deleting task 8\n");
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

			//printf("checkpoint t8.\n");
			startTick = rtems_clock_get_ticks_since_boot();
			start = startTick  / (double)tick_per_second;

			if(tsk_counter == 0){
				first_start = start;
		   	}
			
			numberPreemptedTask = check_running_task(&suspendedTask);
			
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

			//printf("Task 8 starts at current second %.6f.\n", start);

			//LOOP(task_8_normal_et);   
			LOOP(task_8_normal_et,task_id);

			task_fault = task_fault_check(task_8_normal_et);

			//printf(task_fault? "Task 8 is faulty. \n" : "Task 8 has no fault.\n"  );

			if(task_fault == TRUE){
#if 0
				delta_start = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;

				for(i=0; i < nTask; i++){
					if(taskrunning_table[i] == 1 && deltastart_table[i] == 0)
						deltastart_table[i] = delta_start;
					else if(taskrunning_table[i] == 1 && deltastart_table[i] != 0)
						d_t[i] = delta_start - deltastart_table[i];	
				}
#endif
				printf("task 8 is faulty.\n");
				healthy = check_busyP(task_id, nTask);
				if(healthy == FALSE && sys_fault_flag == FALSE){
					sys_fault_flag = TRUE;
					sys_turn_unhealthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
					sys_healthy_end = sys_turn_unhealthy;
					sys_unhealthy_start = sys_turn_unhealthy;
					sys_healthy_duration = sys_healthy_end - sys_healthy_start;
					sys_healthy_total_duration += sys_healthy_duration;
					//printf("System turned unhealthy in task %d at %.3f seconds.\n",task_id+1 , sys_turn_unhealthy);
					//printf("System was healthy with duration of %.3f second. \n", sys_healthy_duration);
				}

				remaining_time = task_8_abnormal_et - task_8_normal_et;
				if(remaining_time != 0)
					LOOP(remaining_time,task_id);
			}

			end = rtems_clock_get_ticks_since_boot()/(double)tick_per_second;

			//printf("					Task 8 ends at current second %.6f.\n", end);
	 
			deadline = first_start + (tsk_counter +1)*PERIOD_TASK_8;

			stop_sys = check_deadline(&first_task_flag ,nTask, deadline, end, task_type, task_id, tick_per_second, &meet);
			if(stop_sys == TRUE){
				sys_stop_flag = TRUE;
				//printf("sys_stop_flag set as true\n");
				//break;
			}else{
				//printf("Task 8 is executing at %d time with deadline at %.3f second.\n\n",tsk_counter, deadline);
				taskrunning_table[task_id] = 0;
#if 0
				if(meet == TRUE){
					deltastart_table[task_id] = 0;
					d_t[task_id] = 0;
				}
#endif
				tsk_counter += 1;
/*				if(experiment_flag ==0){
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
				}
*/			}
		}
	}
}

// TASK 9
rtems_task Task_9(
  rtems_task_argument unused
)
{
  rtems_id selfid=rtems_task_self();
  rtems_status_code status;
  rtems_name        period_name; 
  rtems_id          RM_period;
  //rtems_interval    tick_per_second;
  double            start, end;
  //int		    i=0;
  bool 		    first_task_flag = FALSE;
  bool 		    task_fault = FALSE;
  //double	    delta_start;
  int		    task_id = ID_TASK_9;
  bool		    healthy;
  double 	    sys_turn_unhealthy;
  double	    remaining_time;
  int 		    task_type = TYPE_TASK_9;
  double	    first_start;
  int		    tsk_counter = 0;
  double	    deadline;
  bool		    stop_sys;
  bool              meet = FALSE;
  int		    suspendedTask[10];
  int		    numberPreemptedTask = 0;
  int		    startTick = 0;
  int 		    suspendedTaskid = 100;
  int		    j = 0;

  /* Initializes the GPIO API */
  tick_per_second = rtems_clock_get_ticks_per_second();

  /*create period*/

  period_name = rtems_build_name( 'P', 'E', 'R', '9' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

	while( 1 ) {

		/*this part for release offset*/
		status = rtems_rate_monotonic_period( RM_period, PERIOD_TASK_9);

/* 	if( RTEMS_TIMEOUT == status ) {
			printf("Task 9 missed period!\n");
			break;
		}
*/   
  
		if(sys_stop_flag == TRUE  || experiment_flag ==0){
			//printf("deleting task 9\n");
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
 		
			//printf("checkpoint t9.\n");
			startTick = rtems_clock_get_ticks_since_boot();
			start = startTick  / (double)tick_per_second;

			if(tsk_counter == 0){
				first_start = start;
		   	}
			
			numberPreemptedTask = check_running_task(&suspendedTask);
			
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

			//printf("Task 9 starts at current second %.6f. \n", start);
	  
			//LOOP(task_9_normal_et);    
			LOOP(task_9_normal_et,task_id);

			task_fault = task_fault_check(task_9_normal_et);
			//printf(task_fault? "Task 9 is faulty. \n" : "Task 9 has no fault.\n"  );
	    
			if(task_fault == TRUE){
#if 0
				delta_start = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;

				for(i=0; i < nTask; i++){
					if(taskrunning_table[i] == 1 && deltastart_table[i] == 0)
						deltastart_table[i] = delta_start;
					else if(taskrunning_table[i] == 1 && deltastart_table[i] != 0)
						d_t[i] = delta_start - deltastart_table[i];	
				}
#endif
				printf("task 9 is faulty.\n");
				healthy = check_busyP(task_id, nTask);
				if(healthy == FALSE  && sys_fault_flag == FALSE){
					sys_fault_flag = TRUE;
					sys_turn_unhealthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
					sys_healthy_end = sys_turn_unhealthy;
					sys_unhealthy_start = sys_turn_unhealthy;
					sys_healthy_duration = sys_healthy_end - sys_healthy_start;
					sys_healthy_total_duration += sys_healthy_duration;
					//printf("System turned unhealthy in task %d at %.3f seconds.\n",task_id+1 , sys_turn_unhealthy);
					//printf("System was healthy with duration of %.3f second. \n", sys_healthy_duration);
				}

				remaining_time = task_9_abnormal_et - task_9_normal_et;
				if(remaining_time != 0)
					LOOP(remaining_time,task_id);
			}

			end = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;

			//printf("					Task 9 ends at current second %.6f. \n", end);

			deadline = first_start + (tsk_counter +1)*PERIOD_TASK_9;

			stop_sys = check_deadline(&first_task_flag ,nTask, deadline, end, task_type, task_id, tick_per_second, &meet);
			if(stop_sys == TRUE){
				sys_stop_flag = TRUE;
				//printf("sys_stop_flag set as true\n");
				//break;
			}else{

				//printf("Task 9 is executing at %d time with deadline at %.3f second.\n\n",tsk_counter, deadline);
				taskrunning_table[task_id] = 0;
#if 0
				if(meet == TRUE){
					deltastart_table[task_id] = 0;
					d_t[task_id] = 0;
				}
#endif
				tsk_counter += 1;
/*				if(experiment_flag ==0){
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
				}
*/			}
		}
	}
}

// TASK 10
rtems_task Task_10(
  rtems_task_argument unused
)
{
  rtems_id selfid=rtems_task_self();
  rtems_status_code status;
  rtems_name        period_name; 
  rtems_id          RM_period;
  //rtems_interval    tick_per_second;
  double 	    start, end;
  //int               i=0;
  bool 		    first_task_flag = FALSE;
  bool 		    task_fault = FALSE;
  //double	    delta_start;
  int		    task_id = ID_TASK_10;
  bool		    healthy;
  double 	    sys_turn_unhealthy;
  double	    remaining_time;
  int 		    task_type = TYPE_TASK_10;
  double	    first_start;
  int		    tsk_counter = 0;
  double	    deadline;
  bool		    stop_sys;
  bool              meet = FALSE;
  int		    suspendedTask[10];
  int		    numberPreemptedTask = 0;
  int		    startTick = 0;
  int 		    suspendedTaskid = 100;
  int		    j = 0;

  /* Initializes the GPIO API */
  tick_per_second = rtems_clock_get_ticks_per_second();

  /*create period*/

  period_name = rtems_build_name( 'P', 'E', '1', '0' );
  status = rtems_rate_monotonic_create( period_name, &RM_period );
  if( RTEMS_SUCCESSFUL != status ) {
    printf("RM failed with status: %d\n", status);
    exit(1);
  }

	while( 1 ) {
 
		/*this part for release offset*/
		status = rtems_rate_monotonic_period( RM_period, PERIOD_TASK_10);

/*		if( RTEMS_TIMEOUT == status ) {
			printf("Task 10 missed period!\n");
			break;
		}
*/
    
		if(sys_stop_flag == TRUE  || experiment_flag ==0){
			//printf("deleting task 10\n");
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

			//printf("checkpoint t10.\n");
			startTick = rtems_clock_get_ticks_since_boot();
			start = startTick  / (double)tick_per_second;

			if(tsk_counter == 0){
				first_start = start;
		   	}
			
			numberPreemptedTask = check_running_task(&suspendedTask);
			
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

			//printf("Task 10 starts at current second %.6f.\n", start);

			//LOOPA(task_10_normal_et);
			LOOP(task_10_normal_et,task_id);

			task_fault = task_fault_check(task_10_normal_et);

			//printf(task_fault? "Task 10 is faulty. \n" : "Task 10 has no fault.\n"  );

			if(task_fault == TRUE){
#if 0
				delta_start = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;

				for(i=0; i < nTask; i++){
					if(taskrunning_table[i] == 1 && deltastart_table[i] == 0)
						deltastart_table[i] = delta_start;
					else if(taskrunning_table[i] == 1 && deltastart_table[i] != 0)
						d_t[i] = delta_start - deltastart_table[i];	
				}
#endif
				printf("task 10 is faulty.\n");
				healthy = check_busyP(task_id, nTask);
				if(healthy == FALSE && sys_fault_flag == FALSE){
					sys_fault_flag = TRUE;
					sys_turn_unhealthy = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
					sys_healthy_end = sys_turn_unhealthy;
					sys_unhealthy_start = sys_turn_unhealthy;
					sys_healthy_duration = sys_healthy_end - sys_healthy_start;
					sys_healthy_total_duration += sys_healthy_duration;
					//printf("System turned unhealthy in task %d at %.3f seconds.\n",task_id+1 , sys_turn_unhealthy);
					//printf("System was healthy with duration of %.3f second. \n", sys_healthy_duration);
				}

				remaining_time = task_10_abnormal_et - task_10_normal_et;
				if(remaining_time != 0)
					LOOP(remaining_time,task_id);
				}

			end = rtems_clock_get_ticks_since_boot()/(double)tick_per_second;

			//printf("					%d Task 10 ends at current second %.6f.\n", tsk_counter+1, end);
	    
			deadline = first_start + (tsk_counter +1)*PERIOD_TASK_10;

			stop_sys = check_deadline(&first_task_flag ,nTask, deadline, end, task_type, task_id, tick_per_second, &meet);
			if(stop_sys == TRUE){
				sys_stop_flag = TRUE;
				//printf("sys_stop_flag set as true\n");
				//break;
			}else{
	    
				//printf("Task 10 is executing at %d time with deadline at %.3f second.\n\n",tsk_counter, deadline);
				taskrunning_table[task_id] = 0;
#if 0
				if(meet == TRUE){
					deltastart_table[task_id] = 0;
					d_t[task_id] = 0;
				}
#endif
				tsk_counter += 1;
				//printf("task 10 counter is %d.\n",tsk_counter);
				//printf("					Total system runtime in healthy condition is %.6f second.\n",sys_healthy_total_duration);
				//printf("					Total system runtime in unhealthy condition is %.6f second.\n", sys_unhealthy_total_duration);
	    
				if(tsk_counter == testnumber){
					//printf("task 10 counter is %d.\n",tsk_counter);
					//printf("task 10 period is %.6f.\n",PERIOD_TASK_10);
					//printf("current time (second) is %.6f.\n",end);
					
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
					//printf("\n");
				}
			}
		}
	}
}

