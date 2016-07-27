#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define FLOORING_POS(X) ((int)(X))

#if 1
//for experiments
#include "system.h"
#include "samples.h"
#else
//for testing
#include "uunifast.h"
#endif

double U_mean = 0.25;
int u_sum = 0;
int numLog = 2;
double gscaleFac = 1.5;
int numMode;
int xMode;
double vRatio;
//double USet[5];
int Pmin = 1;
int Pmax = 1000000;


void UUniFast(double U_avg, double *US)
{
	double sumU=U_avg;
	double nextSumU=0;
	int i=0;
	srand(time(NULL));
	for(i=0; i<ntask-1; i++){
		double randuni = ((double) rand() / (RAND_MAX));
		nextSumU=sumU*pow(randuni, (double)1/(ntask-i));
		
		US[i]=sumU-nextSumU;
		sumU=nextSumU;
	}
	US[ntask-1]=sumU;
}
double randMToN(double M, double N)
{
    return M + (rand() / ( RAND_MAX / (N-M) ) ) ;  
}
void CSet_generate(attri *tg, double* USet)
{
	int i=0, j=0;
	for(i=0; i<ntask; i++){
		int thN=j%numLog;
		double randmin = Pmin*pow(10, thN);
		double randmax = Pmin*pow(10, thN+1);		
		tg[i].period =FLOORING_POS(randMToN(randmax, randmin)); 
		tg[i].utilization = USet[i];		
		j+=1;
	}
}
#if 0
int main(){
	int i=0;
	double checkU=0.0;
	attri taskgs[5];
	srand(time(NULL));
	UUniFast(uTotal, USet);
	CSet_generate(taskgs);
	for(i=0; i<numTasks; i++){
		printf("Task %d :", i);
		printf("period %d ", taskgs[i].period);
		printf("uti %lf ", taskgs[i].utilization);
		printf("\n");
		checkU+=taskgs[i].utilization;
	}
	printf("checkU %lf\n", checkU);
}
#endif
