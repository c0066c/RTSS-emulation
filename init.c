#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CEILING_POS(X) ((X-(int)(X)) > 0 ? (int)(X+1) : (int)(X))
#define CONFIGURE_INIT
#include "system.h"
#include "samples.h"
#include <rtems/test.h>
#include <rtems/status-checks.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#define UT 50

int task_set_idx = 0;
int task_set_idcheck = 100;
int running_flag[10] = {0,0,0,0,0,0,0,0,0,0};
int experiment_flag = 0;
rtems_id inittask_id;
rtems_id Task_id[ 11 ];
rtems_name Task_name[ 11 ];
uint32_t tick_per_second;
int sec_loopCount=0;
bool task_running_flag = FALSE;
bool sys_fault_flag = FALSE;
bool sys_stop_flag = FALSE;
double sys_healthy_start = 0;
double sys_healthy_end = 0;
double sys_unhealthy_start = 0;
double sys_unhealthy_end = 0;
double sys_healthy_duration = 0;
double sys_unhealthy_duration = 0;
double sys_healthy_total_duration = 0;
double sys_unhealthy_total_duration = 0;
int taskrunning_table[10];
int preempted_table[2][10];
double deltastart_table[10];
int sp_dl_missed_table[10];
double d_t[10];
attri tsk[10];
int ntask = 10;
int testnumber = 1; //TODO: change to hyperperiod? or the number of lowest priority task
double sys_totalruntime = 0;
double sys_totalhealthy_percentage = 0;
double sys_totalunhealthy_percentage = 0;
int refer_fault_rate = 0;
double fault_rate[4] = {0.01, 0.001, 0.0001, 0.00001};
int start_flag = 0;
int inittask_count=0;
double uTotal = (double)UT/100;

tinp taskinput[10];

rtems_task Init(
	rtems_task_argument argument
)
{
	rtems_status_code status;
	rtems_time_of_day time;
	int i = 0;
	int schedulability;
	double start, end;
 
	printf("10tasks_1.83hardFactor_1.83softFactor_60.0hardTasksPerc_70uti\nSet 0 to Set 9:\n\n");
	//printf("10tasks_1.83hardFactor_1.83softFactor_40.0hardTasksPerc_86uti\nSet 10 to Set 19:\n\n");
	//printf("10tasks_1.83hardFactor_1.0softFactor_60.0hardTasksPerc_78uti\nSet 10 to Set 19:\n\n");
	//printf("10tasks_1.08hardFactor_1.08softFactor_60.0hardTasksPerc_92uti\nSet 0 to Set 9:\n\n");
	//printf("10tasks_2.83hardFactor_2.83softFactor_60.0hardTasksPerc_50uti\nSet 0 to Set 9:\n\n");

	tick_per_second = rtems_clock_get_ticks_per_second();
	printf("\nTicks per second in your system: %" PRIu32 "\n", tick_per_second);

// sec_loopCount=LoopCountingForASec(); //for init the relative count
//	printf("Loop count per second: %i \n\n", sec_loopCount);

/*
  start = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
  LOOP(0.005);
  end = rtems_clock_get_ticks_since_boot() / (double)tick_per_second;
  printf("start = %.6f, end = %.6f\n", start, end);
*/
/*
  time.year   = 1988;
  time.month  = 12;
  time.day    = 31;
  time.hour   = 9;
  time.minute = 0;
  time.second = 0;
  time.ticks  = 0;

  status = rtems_clock_set( &time );
*/
	#include "10tasks_1.83hardFactor_1.83softFactor_60.0hardTasksPerc_70uti.h"
	//#include "10tasks_1.83hardFactor_1.83softFactor_40.0hardTasksPerc_86uti_mod.h"
	//#include "10tasks_1.83hardFactor_1.0softFactor_60.0hardTasksPerc_78uti_mod.h"
	//#include "10tasks_1.08hardFactor_1.08softFactor_60.0hardTasksPerc_92uti_mod.h"	
	//#include "10tasks_2.83hardFactor_2.83softFactor_60.0hardTasksPerc_50uti_mod.h"
	
	
  	Task_name[ 1 ] = rtems_build_name( 'T', 'A', '1', ' ' );
  	Task_name[ 2 ] = rtems_build_name( 'T', 'A', '2', ' ' );
  	Task_name[ 3 ] = rtems_build_name( 'T', 'A', '3', ' ' );
  	Task_name[ 4 ] = rtems_build_name( 'T', 'A', '4', ' ' );
  	Task_name[ 5 ] = rtems_build_name( 'T', 'A', '5', ' ' );
  	Task_name[ 6 ] = rtems_build_name( 'T', 'A', '6', ' ' );
  	Task_name[ 7 ] = rtems_build_name( 'T', 'A', '7', ' ' );
  	Task_name[ 8 ] = rtems_build_name( 'T', 'A', '8', ' ' );
  	Task_name[ 9 ] = rtems_build_name( 'T', 'A', '9', ' ' );
  	Task_name[ 10 ] = rtems_build_name( 'T', 'A', '1', '0' );

  	for(i = 0; i<ntask; i++){
		  taskrunning_table[i] = 0;
		  preempted_table[0][i] = 0;
		  preempted_table[1][i] = 0;
		  deltastart_table[i] = 0;
     	d_t[i] = 0;
    	sp_dl_missed_table[i] = 0;
   		tsk[i].period = 0;
   		tsk[i].utilization = 0;
		  tsk[i].task_type = 0;
    	tsk[i].normal_et = 0;
  		tsk[i].abnormal_et= 0;
   		tsk[i].id = i;
   		tsk[i].priority = -1;
	  }

  while(1){
#if 0
		int lcmAmong=(int)tsk[0].period;
  		for(i=1; i<ntask; i++){
			lcmAmong=lcm(lcmAmong, (int)tsk[i].period);
      			printf("\n");
  		}
  		testnumber = CEILING_POS((double)lcmAmong / tsk[4].period); //take hyperperiod to setup the lowest priority task
#else
    testnumber = 37;
  		//testnumber = taskinputs[task_set_idx].testnum;
  		//printf("testnumber %d,", testnumber);
#endif

		for(i=0; i<ntask; i++){
			tsk[i].period = taskinput[task_set_idx].tasks[i].period;
  			//tsk[i].utilization = taskinputs[task_set_idx].tasks[i].utilization;
			tsk[i].task_type = taskinput[task_set_idx].tasks[i].task_type;
			tsk[i].normal_et = taskinput[task_set_idx].tasks[i].normal_et;
			tsk[i].abnormal_et = taskinput[task_set_idx].tasks[i].abnormal_et;
			//printf("Task %i with normal exe %.1f\n",i,tsk[i].normal_et);
		}
  
/************************************************/
		//printf("Again Declaratioan of variable:\n");
		sys_totalruntime = 0;
	  sys_totalhealthy_percentage = 0;
		sys_totalunhealthy_percentage = 0;
		sys_healthy_start = 0;
    sys_healthy_end = 0;
		sys_unhealthy_start = 0;
    sys_unhealthy_end = 0;
		sys_healthy_duration = 0;
    sys_unhealthy_duration = 0;
    sys_healthy_total_duration = 0;
    sys_unhealthy_total_duration = 0;
    task_running_flag = FALSE;
    sys_fault_flag = FALSE;
    sys_stop_flag = FALSE;
      
		for(i=0; i<ntask; i++){
			taskrunning_table[i] = 0;
      tsk[i].priority = -1;
    }
//      printf("\n\n\n*** NXT TEST %d ***\n", inittask_count);

#if 1
/*
  		tsk[0].task_type = 0;
 		tsk[1].task_type = 0;
  		tsk[2].task_type = 0;
  		tsk[3].task_type = 1;
  		tsk[4].task_type = 1;
  
  		inittask_count = 28;
*/
#else
		Exhaustive_Task_Type(ntask, inittask_count, tsk);
#endif

		schedulability =priority_assignment(tsk, ntask);

		if(schedulability == -1){
			printf("x\n");
//         		printf("Tasks are not schedulable.\nThis Operation will skip immediately.\n");	 
    }else{
    	if(task_set_idcheck != task_set_idx){
				printf("Current task set is set %d. \n",task_set_idx);
				task_set_idcheck = task_set_idx;
			}

			status = rtems_task_create(
				Task_name[ 1 ], tsk[0].priority, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
            			RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 1 ]
         		);
         		if ( status != RTEMS_SUCCESSFUL) {
            			printf( "rtems_task_create 1 failed with status of %d.\n", status );
           		 	exit( 1 );
         		} 
			running_flag[0]=1; 

      			status = rtems_task_create(
				Task_name[ 2 ], tsk[1].priority, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
				RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 2 ]
			);
        	 	if ( status != RTEMS_SUCCESSFUL) {
				printf( "rtems_task_create 2 failed with status of %d.\n", status );
				exit( 1 );
			}
			running_flag[1]=1; 
#if 1
			status = rtems_task_create(
				Task_name[ 3 ], tsk[2].priority, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
				RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 3 ]
			);
        	 	if ( status != RTEMS_SUCCESSFUL) {
				printf( "rtems_task_create 3 failed with status of %d.\n", status );
				exit( 1 );
			}
	         	running_flag[2]=1; 
 
        		status = rtems_task_create(
				Task_name[ 4 ], tsk[3].priority, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
				RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 4 ]
			);
			if ( status != RTEMS_SUCCESSFUL) {
				printf( "rtems_task_create 4 failed with status of %d.\n", status );
				exit( 1 );
			} 
         		running_flag[3]=1;
 
			status = rtems_task_create(
				Task_name[ 5 ], tsk[4].priority, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
				RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 5 ]
			);
			if ( status != RTEMS_SUCCESSFUL) {
				printf( "rtems_task_create 5 failed with status of %d.\n", status );
				exit( 1 );
			}
	        	running_flag[4]=1; 

      			status = rtems_task_create(
         			Task_name[ 6 ], tsk[5].priority, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
         			RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 6 ]
         		);
        		if ( status != RTEMS_SUCCESSFUL) {
         			printf( "rtems_task_create 6 failed with status of %d.\n", status );
           			exit( 1 );
			}
         		running_flag[5]=1; 

      			status = rtems_task_create(
         			Task_name[ 7 ], tsk[6].priority, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
         			RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 7 ]
         		);
        		if ( status != RTEMS_SUCCESSFUL) {
         			printf( "rtems_task_create 7 failed with status of %d.\n", status );
            			exit( 1 );
        		} 
         		running_flag[6]=1; 

	      		status = rtems_task_create(
        	 		Task_name[ 8 ], tsk[7].priority, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
         			RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 8 ]
         		);
        		if ( status != RTEMS_SUCCESSFUL  ) {
         			printf( "rtems_task_create 8 failed with status of %d.\n", status );
      				exit( 1 );
     			}
         		running_flag[7]=1; 

	      		status = rtems_task_create(
        	 		Task_name[ 9 ], tsk[8].priority, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
         			RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 9 ]
         		);
        		if( status != RTEMS_SUCCESSFUL) {
				printf( "rtems_task_create 9 failed with status of %d.\n", status );
				exit( 1 );
			}
			running_flag[8]=1;
#endif
			status = rtems_task_create(
	         		Task_name[ 10 ], tsk[9].priority, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
        	 		RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 10 ]
         		);
        		if ( status != RTEMS_SUCCESSFUL) {
         			printf( "rtems_task_create 10 failed with status of %d.\n", status );
           			exit( 1 );
        		}
        		running_flag[9]=1; 

      // prototype: rtems_task_start( id, entry_point, argument );
			experiment_flag = 1;
			inittask_id=rtems_task_self();
			status = rtems_task_start( Task_id[ 1 ], Task_1, 1);
			if ( status != RTEMS_SUCCESSFUL) {
				printf( "rtems_task_start 1 failed with status of %d.\n", status );
          			exit( 1 );
      			}

			status = rtems_task_start( Task_id[ 2 ], Task_2, 1);
	       		if ( status != RTEMS_SUCCESSFUL) {
	        		printf( "rtems_task_start 2 failed with status of %d.\n", status );
	           		exit( 1 );
	       		}
#if 1
			status = rtems_task_start( Task_id[ 3 ], Task_3, 1);
		         if ( status != RTEMS_SUCCESSFUL) {
	       			printf( "rtems_task_start 3 failed with status of %d.\n", status );
	          		exit( 1 );
	       		}

			status = rtems_task_start( Task_id[ 4 ], Task_4, 1);
	 	  	if ( status != RTEMS_SUCCESSFUL) {
				printf( "rtems_task_start 4 failed with status of %d.\n", status );
		           	exit( 1 );
       			}

			status = rtems_task_start( Task_id[ 5 ], Task_5, 1);
			if ( status != RTEMS_SUCCESSFUL) {
				printf( "rtems_task_start 5 failed with status of %d.\n", status );
	        	  	exit( 1 );
      			}

      			status = rtems_task_start( Task_id[ 6 ], Task_6, 1);
       			if ( status != RTEMS_SUCCESSFUL) {
     				printf( "rtems_task_start 6 failed with status of %d.\n", status );
     		     		exit( 1 );
      			}

   			status = rtems_task_start( Task_id[ 7 ], Task_7, 1);
      			if ( status != RTEMS_SUCCESSFUL) {
        			printf( "rtems_task_start 7 failed with status of %d.\n", status );
        			exit( 1 );
    			}

    			status = rtems_task_start( Task_id[ 8 ], Task_8, 1);
	      		if ( status != RTEMS_SUCCESSFUL) {
       				printf( "rtems_task_start 8 failed with status of %d.\n", status );
      				exit( 1 );
    			}

     			status = rtems_task_start( Task_id[ 9 ], Task_9, 1);
    			if ( status != RTEMS_SUCCESSFUL) {
	       			printf( "rtems_task_start 9 failed with status of %d.\n", status );
				exit( 1 );
      			}
#endif
      			status = rtems_task_start( Task_id[ 10 ], Task_10, 1);
      			if ( status != RTEMS_SUCCESSFUL) {
       				printf( "rtems_task_start 10 failed with status of %d.\n", status );
        			exit( 1 );
       			}

          		//printf("checkpoint 1.\n");
      			// delete init task after starting the three working tasks
			status = rtems_task_suspend(RTEMS_SELF);
	//	      printf("YES, it is feasible to use suspend/resume on init\n");
		}

		printf("with fault rate at %.6f rate and task count now is %d. \n",fault_rate[refer_fault_rate],inittask_count);
		inittask_count+=1;
		if(refer_fault_rate+1 == 4){
		//if(refer_fault_rate+1 == 2){
			printf("checked\n\n");
  			task_set_idx++;
		}
		refer_fault_rate= (refer_fault_rate+1)%4;  
		//refer_fault_rate= (refer_fault_rate+1)%2;
      		//At this moment, the experiment is done; inittask_count for experiment to be finished is number of fault rate to be tested * number of task set
		//if (inittask_count == 40){
		if (inittask_count == 1){
			printf("The testing is finished among 40 combinations\n");
			break;
  		}
	}
	status = rtems_task_delete( RTEMS_SELF );
}
