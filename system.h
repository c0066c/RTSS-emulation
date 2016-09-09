/*
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/*  updated for SEMAPHORE test, 15/02/2016, Kuan-Hsun Chen */


#include <inttypes.h>
#include <rtems.h>

typedef struct task_attr{ 
	double period; //implicit deadline
	double utilization;
	int task_type;
        double abnormal_et;
        double normal_et;
	int priority;
	int id;
} attri;

typedef struct task_input{
	attri tasks[10];
	int testnum;
} tinp;

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

rtems_task Task_1(
  rtems_task_argument argument
);

rtems_task Task_2(
  rtems_task_argument argument
);

rtems_task Task_3(
  rtems_task_argument argument
);

rtems_task Task_4(
  rtems_task_argument argument
);

rtems_task Task_5(
  rtems_task_argument argument
);

rtems_task Task_6(
  rtems_task_argument argument
);

rtems_task Task_7(
  rtems_task_argument argument
);

rtems_task Task_8(
  rtems_task_argument argument
);

rtems_task Task_9(
  rtems_task_argument argument
);

rtems_task Task_10(
  rtems_task_argument argument
);


/* global variables */

/*
 *  Keep the names and IDs in global variables so another task can use them.
 */ 

extern int running_flag[10];
extern rtems_id   Task_id[ 11 ];         /* array of task ids */
extern rtems_name Task_name[ 11 ];       /* array of task names */
extern rtems_id   Sem_id;               /* Semaphore id */
extern rtems_id   inittask_id;
extern uint32_t tick_per_second;
extern bool task_running_flag;
extern bool sys_fault_flag;
extern bool sys_stop_flag;
extern double sys_healthy_start;
extern double sys_healthy_end;
extern double sys_unhealthy_start;
extern double sys_unhealthy_end;
extern double sys_healthy_duration;
extern double sys_unhealthy_duration;
extern double sys_healthy_total_duration;
extern double sys_unhealthy_total_duration;
extern double sys_totalruntime;
extern double sys_totalhealthy_percentage;
extern double sys_totalunhealthy_percentage;
extern int preempted_table[2][10];
extern int taskrunning_table[10];
extern int sp_dl_missed_table[10];
extern attri tsk[10];
extern int ntask;
extern int testnumber;
//extern double fault_rate[4];
extern double fault_rate[4];
extern int refer_fault_rate;
extern int experiment_flag;
extern rtems_id period_id[11];
extern bool AllReady;
extern int seedseed;

/* configuration information */

#include <bsp.h> /* for device driver prototypes */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_MICROSECONDS_PER_TICK     1000   // NB: 10 and lower gives system failure for erc32 simulator
#define CONFIGURE_MAXIMUM_TASKS             11
#define CONFIGURE_MAXIMUM_SEMAPHORES        1
#define CONFIGURE_MAXIMUM_PRIORITY          15
#define CONFIGURE_EXTRA_TASK_STACKS         (20 * RTEMS_MINIMUM_STACK_SIZE)

// Needed for RM Mangager
#define CONFIGURE_MAXIMUM_PERIODS           11

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

//KHCHEN
//#define CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM

/* Needed for erc32 simulator.. */
/* ..for using "CPU_usage_Dump", since it uses printf("%f") if your processor has floating points) */
/* If you want to take away FP support (to avoid heavy context switch), you must rewrite CPU_usage_Dump instead */
//#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT
//#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_PREEMPT


#include <rtems/confdefs.h>

/*
 *  Handy macros and static inline functions
 */

/*
 *  Macro to hide the ugliness of printing the time.
 */

#define print_time(_s1, _tb, _s2) \
  do { \
    printf( "%s%02" PRIu32 ":%02" PRIu32 ":%02" PRIu32 \
       "   %02" PRIu32 "/%02" PRIu32 "/%04" PRIu32 "%s", \
       _s1, (_tb)->hour, (_tb)->minute, (_tb)->second, \
       (_tb)->month, (_tb)->day, (_tb)->year, _s2 ); \
    fflush(stdout); \
  } while ( 0 )

/*
 *  Macro to print an task name that is composed of ASCII characters.
 *
 */

#define put_name( _name, _crlf ) \
  do { \
    uint32_t c0, c1, c2, c3; \
    \
    c0 = ((_name) >> 24) & 0xff; \
    c1 = ((_name) >> 16) & 0xff; \
    c2 = ((_name) >> 8) & 0xff; \
    c3 = (_name) & 0xff; \
    putchar( (char)c0 ); \
    if ( c1 ) putchar( (char)c1 ); \
    if ( c2 ) putchar( (char)c2 ); \
    if ( c3 ) putchar( (char)c3 ); \
    if ( (_crlf) ) \
      putchar( '\n' ); \
  } while (0)

/*
 *  Macro to loop.
 */


#define LOOP(_et, taskID){ \
  \
  int start = 0; \
  int now = 0; \
  int elapsedTick = 0; \
  int tmp = _et;\
  \
  start = rtems_clock_get_ticks_since_boot();\
  \
  while(1){\
	\
    now = rtems_clock_get_ticks_since_boot();\
 	  if(preempted_table[0][taskID]!=0){	\
		  now = rtems_clock_get_ticks_since_boot();\
  		elapsedTick = preempted_table[1][taskID]- start;	\
		  tmp = tmp - elapsedTick; \
		  start = now; \
  		preempted_table[0][taskID] = 0; \
		  preempted_table[1][taskID] = 0;\
    }\
	  if(now-start >= tmp){\
      break;\
     }\
   }\
 }

/*
 *  This allows us to view the "Test_task" instantiations as a set
 *  of numbered tasks by eliminating the number of application
 *  tasks created.
 *
 *  In reality, this is too complex for the purposes of this
 *  example.  It would have been easier to pass a task argument. :)
 *  But it shows how rtems_id's can sometimes be used.
 */

#define task_number( tid ) \
  ( rtems_get_index( tid ) - \
     rtems_configuration_get_rtems_api_configuration()->number_of_initialization_tasks )

/* end of include file */
