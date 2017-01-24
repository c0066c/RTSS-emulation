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

//KHCHEN 03.08 Global table for period ID
rtems_id period_id[11];

int task_set_idx = 0;
int running_flag[10] = {0,0,0,0,0,0,0,0,0,0};  /*0: task is deleted, 1: task is created*/
int experiment_flag = 0;  /*0: the simulation is stopped as termination criterion is met, 1: simulaton is started after all tasks are created*/
rtems_id inittask_id;
rtems_id Task_id[ 11 ];
rtems_name Task_name[ 11 ];
uint32_t tick_per_second;
bool task_running_flag = FALSE;  /*Indication of any task that is running in the system*/
bool sys_fault_flag = FALSE;    /*System status; False: System is healthy, True: System is unhealthy*/
bool sys_stop_flag = FALSE;     /*Flag to stop the system*/
double sys_healthy_start = 0;
double sys_healthy_end = 0;
double sys_unhealthy_start = 0;
double sys_unhealthy_end = 0;
double sys_healthy_duration = 0;
double sys_unhealthy_duration = 0;
double sys_healthy_total_duration = 0;
double sys_unhealthy_total_duration = 0;
int taskrunning_table[10];    /*0: task is not running, 1: task is running in normal system mode, 2: task is running in system fault mode*/
int preempted_table[2][10];   /*2D table that save task being preempted, first row is task id, second row: 1 mean preempted, 0 mean not.*/
int sp_dl_missed_table[10];   /* table that save the task that might miss the deadline from check_busy_P function*/
attri tsk[10];
int ntask = 10;
int testnumber = 1; /*Termination criterion for each simulaton: the number of time that lowest priority task should execute*/
double sys_totalruntime = 0;
double sys_totalhealthy_percentage = 0;
double sys_totalunhealthy_percentage = 0;
int refer_fault_rate = 0;
int inittask_count=0;  /*Criteria to stop this experiment which consists of several simulations that running iteratively*/
bool AllReady;       /*Flag to indicate whether all tasks 1 to 10 are created, simulation only starts after all tasks are created and ready to run, as RTEMS normally create 1 task and execute it immediately, the next task will only be created after the previous task has finished its execution*/
int seedseed = 5;   /*seed for random, to create different random value for each task, without this, all task will read the same random page and have same random value at same random count in each task*/

//User Setting
int iterative_run = 40;
double fault_rate[4] = {0.01, 0.001, 0.0001, 0.00001};  /*Fault rate for the simulation*/
int number_of_used_fault_rate = 4; /*must be identical to the number of fault rate in array fault_rate*/
int sim_duration = 1; /*The length of each simulaiton duration (in hour)*/

tinp taskinput[10];

rtems_task Init(
	rtems_task_argument argument
)
{
	rtems_status_code status;
	rtems_time_of_day time;
	int i = 0;
 
	printf("10tasks_1.83hardFactor_1.83softFactor_50.0hardTasksPerc_70uti\nSet 30 to Set 39:\n\n");

	tick_per_second = rtems_clock_get_ticks_per_second();
	printf("\nTicks per second in your system: %" PRIu32 "\n", tick_per_second);

	#include "10tasks_1.83hardFactor_1.83softFactor_50.0hardTasksPerc_70uti.h"

  time.year   = 1988;
  time.month  = 12;
  time.day    = 31;
  time.hour   = 9;
  time.minute = 0;
  time.second = 0;
  time.ticks  = 0;

  status = rtems_clock_set( &time );
  	
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

  /*Initialization of all attribute in task variable*/
  for(i = 0; i<ntask; i++){
    taskrunning_table[i] = 0;
		preempted_table[0][i] = 0;
		preempted_table[1][i] = 0;
    sp_dl_missed_table[i] = 0;
   	tsk[i].period = 0;
   	tsk[i].utilization = 0;
		tsk[i].task_type = 0;
    tsk[i].normal_et = 0;
  	tsk[i].abnormal_et= 0;
   	tsk[i].id = i;
   	tsk[i].priority = i+1;
	}

  while(1){

    /*Declare the value for task attribute*/
		for(i=0; i<ntask; i++){
			tsk[i].period = taskinput[task_set_idx].tasks[i].period;
			tsk[i].task_type = taskinput[task_set_idx].tasks[i].task_type;
			tsk[i].normal_et = taskinput[task_set_idx].tasks[i].normal_et;
			tsk[i].abnormal_et = taskinput[task_set_idx].tasks[i].abnormal_et;
		}
    
    testnumber = (sim_duration*60*60*1000/tsk[9].period)+1;

		/*Reinitialize all variables and flags after one simulation*/
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
    AllReady = FALSE;
    
		for(i=0; i<ntask; i++){
			taskrunning_table[i] = 0; 
    }
    
    /*Create rtems task*/
		status = rtems_task_create(
		  Task_name[ 1 ], tsk[0].priority, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 1 ]
    );
    if(status!= RTEMS_SUCCESSFUL){
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

	  status = rtems_task_create(
	    Task_name[ 10 ], tsk[9].priority, RTEMS_MINIMUM_STACK_SIZE * 2, RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES, &Task_id[ 10 ]
    );
    if ( status != RTEMS_SUCCESSFUL) {
      printf( "rtems_task_create 10 failed with status of %d.\n", status );
      exit( 1 );
    }
    running_flag[9]=1; 

    /* prototype: rtems_task_start( id, entry_point, argument );*/
    /* Start rtems task */
		experiment_flag = 1;
		inittask_id=rtems_task_self();
		status = rtems_task_start( Task_id[ 1 ], Task_1, 1);
		if(status != RTEMS_SUCCESSFUL) {
		  printf( "rtems_task_start 1 failed with status of %d.\n", status );
      exit( 1 );
    }

		status = rtems_task_start( Task_id[ 2 ], Task_2, 1);
	  if(status != RTEMS_SUCCESSFUL) {
	    printf( "rtems_task_start 2 failed with status of %d.\n", status );
	    exit( 1 );
	  }

		status = rtems_task_start( Task_id[ 3 ], Task_3, 1);
		if(status != RTEMS_SUCCESSFUL) {
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

    status = rtems_task_start( Task_id[ 10 ], Task_10, 1);
    if ( status != RTEMS_SUCCESSFUL) {
      printf( "rtems_task_start 10 failed with status of %d.\n", status );
      exit( 1 );
    }

    /* suspend the rtems task until the simulation is finish in tasks.c */
		status = rtems_task_suspend(RTEMS_SELF);

		printf("with fault rate at %.6f rate and task count now is %d. \n",fault_rate[refer_fault_rate],inittask_count);
		inittask_count+=1;
		if(refer_fault_rate+1 == number_of_used_fault_rate){
  			task_set_idx++;
        seedseed = seedseed + 20;
        printf("\n");
		}
		refer_fault_rate= (refer_fault_rate+1)%number_of_used_fault_rate;  
  
		if (inittask_count == iterative_run){
			printf("The experiment ends.\n");
      exit(1);
			break;
  	}
  }
  status = rtems_task_delete( RTEMS_SELF );
}
