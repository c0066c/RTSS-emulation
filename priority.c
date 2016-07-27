#include<stdio.h>
#include"tasks.h"
#include<stdlib.h>

#define CEILING_POS(X) ((X-(int)(X)) > 0 ? (int)(X+1) : (int)(X))
#define CEILING_NEG(X) ((X-(int)(X)) < 0 ? (int)(X-1) : (int)(X))
#define CEILING(X) ( ((X) > 0) ? CEILING_POS(X) : CEILING_NEG(X) )
#define ABS(X)((X)>=0)? X : (X*-1)

int comparePeriod(const void *a, const void *b){
	attri* c = (attri*) a;
        attri* d = (attri*) b;
        return (int)(c->period - d->period);
}


double abnorDemand(int R, attri* tasks, int ntask, int curIdx)
{
        double sumDM=0;
	int i=0;
        for(i=0; i<ntask; i++)
        {
		if(i != curIdx)
	                sumDM += tasks[i].abnormal_et*CEILING(R/tasks[i].period);
        }

        return sumDM;
}
double norDemand(int R, attri* tasks, int ntask, int curIdx)
{
        double sumDM=0;
	int i=0;
        for(i=0; i<ntask; i++)
        {
		if(i != curIdx)
	                sumDM += tasks[i].normal_et*CEILING(R/tasks[i].period);
        }

        return sumDM;
}

double RTA(int curIdx, attri* tasks, int nTask, int mode)
{
	int Dn = tasks[curIdx].period;
	double dm = 0;
	double Cn = tasks[curIdx].normal_et;
	if(mode == 1)
		Cn = tasks[curIdx].abnormal_et;
	double R = Cn;
	while(1)
	{
		if(R>Dn)
		{
			return 0; //fail, as R != 0;
		}else{
			if(mode == 1) //0 is soft task checking normal execution time
				dm=norDemand(R, tasks, nTask, curIdx);
			else
				dm=abnorDemand(R, tasks, nTask, curIdx);

			if(R>Dn){
				return 0; //fail
			}
			if(R < dm+Cn)
				R = dm + Cn;
			else{
				return R;
			}	
		}
	}
}

int findAssignment(attri* tasks, attri* hardTasks, attri* softTasks, int nTask, int hard, int soft)
{
	int detectIdx=0; //tau_l in the paper
	int lastHard = hard-1;
	int lastSoft = soft-1; //maybe it is -1;
	int n = 0;
	for(n = nTask; n >0; n--)
	{	
		if(lastHard>=0 && lastSoft>=0){
			detectIdx = hardTasks[lastHard--].id; //last element of Hardtasks
			if(detectIdx >= 0 && RTA(detectIdx, tasks, nTask, 0)){ 
				//first condition is designed to check whether the set is empty.
				tasks[detectIdx].priority=n;
			}else{
				detectIdx = softTasks[lastSoft--].id; //last element of softtasks
				if(detectIdx >= 0 && RTA(detectIdx, tasks, nTask, 1))
					tasks[detectIdx].priority=n;
				else
					return -1; //NOT POSSIBLE							
			}
		}else if(lastHard <0 && lastSoft>=0){
			detectIdx = softTasks[lastSoft--].id; //last element of softtasks
			if(detectIdx >= 0 && RTA(detectIdx, tasks, nTask, 1))
				tasks[detectIdx].priority=n;
			else
				return -1; //NOT POSSIBLE							
		
		}else if(lastSoft <0 && lastHard>=0){
			detectIdx = hardTasks[lastHard--].id; //last element of Hardtasks
			if(detectIdx >= 0 && RTA(detectIdx, tasks, nTask, 0)){ 
				//first condition is designed to check whether the set is empty.
				tasks[detectIdx].priority=n;
			}else
				return -1;
		}
	}

	return 1; //feasible
}

int priority_assignment(attri* tasks, int nTask)
{
	//if return -1, it means it is not possible to be feasible.
	int hardcount=0, softcount=0;
	int hardidx = 0, softidx = 0;
	int i=0;
	attri* hardTasks;
	attri* softTasks;
	for(i=0; i<nTask; i++){
		if (tasks[i].type == 0)
			hardcount +=1;
		else
			softcount +=1;
	} //find out how many tasks are there.
	hardTasks = (attri*)calloc(hardcount, sizeof(attri));
	softTasks = (attri*)calloc(softcount, sizeof(attri));
	for(i=0; i<nTask; i++){
		if (tasks[i].type == 0)
			hardTasks[hardidx++] = tasks[i];
		else
			softTasks[softidx++] = tasks[i];
	} //put those in the set.
	qsort(hardTasks, hardcount, sizeof(attri), comparePeriod);
	qsort(softTasks, softcount, sizeof(attri), comparePeriod);
/*
	printf("%d, %d\n", hardcount, softcount);
        for(i=0; i<hardcount; i++)
                printf("%d\n", hardTasks[i].period);

        for(i=0; i<softcount; i++)
                printf("%d\n", softTasks[i].period);
*/
	return findAssignment(tasks, hardTasks, softTasks, nTask, hardcount, softcount);
}
