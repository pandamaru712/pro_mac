#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "probability.h"
#include "engine.h"
#include "matrix.h"
#include "tmwtypes.h"
#include "macro.h"

extern Engine *gEp;
extern double r[(NUM_STA+1)*(NUM_STA+1)];
extern double pro[NUM_STA+1][NUM_STA+1];
extern double dummyA[NUM_STA*2][(NUM_STA+1)*(NUM_STA+1)];
extern double A[NUM_STA*2][(NUM_STA+1)*(NUM_STA+1)];
extern double u[NUM_STA*2];
extern double dummyAeq[2][(NUM_STA+1)*(NUM_STA+1)];
extern double Aeq[2][(NUM_STA+1)*(NUM_STA+1)];
extern double beq[2];
extern double lb[(NUM_STA+1)*(NUM_STA+1)];

void calculateProbability(staInfo sta[], int mode){
	int nodeID;
	int i;
	if(mode == 0){   //random
		nodeID = rand() % gSpec.numSta;
		for(i=0; i<gSpec.numSta; i++){
			if(i==nodeID){
				sta[i].fTx = true;
			}else{
				sta[i].fTx = false;
			}
		}
	}else{   //probability
		solveLP();
	}
}

void initializeMatrix(){
	int tate = NUM_STA * 2;
	int yoko = pow((NUM_STA+1), 2);
	int i, j, no;

	for(i=0; i<NUM_STA; i++){
		for(j=0; j<yoko; j++){
			if((j/(NUM_STA+1)==i+1)&&(j%(NUM_STA+1)!=i+1)){
				dummyA[i][j] = -1;
			}else{
				dummyA[i][j] = 0;
			}
		}
	}
	for(i=NUM_STA; i<NUM_STA*2; i++){
		for(j=0; j<yoko; j++){
			if((j%(NUM_STA+1)==(i-NUM_STA+1))&&(j/(NUM_STA+1)!=(i-NUM_STA+1))){
				dummyA[i][j] = -1;
			}else{
				dummyA[i][j] = 0;
			}
		}
	}
	for(j=0; j<yoko; j++){
		for(i=0; i<tate; i++){
			no = tate * j + i + 1;
			if(no%yoko!=0){
				A[no/yoko][no%yoko-1] = dummyA[i][j];
			}else{
				A[no/yoko-1][yoko-1] = dummyA[i][j];
			}
		}
	}

	tate = 2;

	for(j=0; j<yoko; j++){
		if(j/(NUM_STA+1)==j%(NUM_STA+1)){
			dummyAeq[1][j] = 1;
		}else{
			dummyAeq[0][j] = 1;
		}
	}
	for(j=0; j<yoko; j++){
		for(i=0; i<tate; i++){
			no = tate * j + i + 1;
			if(no%yoko!=0){
				Aeq[no/yoko][no%yoko-1] = dummyAeq[i][j];
			}else{
				Aeq[no/yoko-1][yoko-1] = dummyAeq[i][j];
			}
		}
	}
	for(i=0; i<NUM_STA*2; i++){
		u[i] = 100/(2*NUM_STA);
	}
}

void solveLP(){
	int i, j;
	int tate = NUM_STA * 2;
	int yoko = pow(NUM_STA+1, 2);
	char buffer[EP_BUFFER_SIZE] = {'\0'};
	mxArray *mx_p = NULL;
	mxArray *mx_fval = NULL;
	mxArray *mx_r = NULL;
	mxArray *mx_A = NULL;
	mxArray *mx_u = NULL;
	mxArray *mx_Aeq = NULL;
	mxArray *mx_beq = NULL;
	mxArray *mx_lb = NULL;
	mx_p = mxCreateDoubleMatrix(1, yoko, mxREAL);
	mx_fval = mxCreateDoubleMatrix(1, 1, mxREAL);
	double *p, *fval;

	mx_r = mxCreateDoubleMatrix(1, yoko, mxREAL);
	memcpy((void *)mxGetPr(mx_r), (void *)r, sizeof(r));
	mx_A = mxCreateDoubleMatrix(tate, yoko, mxREAL);
	memcpy((void *)mxGetPr(mx_A), (void *)A, sizeof(A));
	mx_u = mxCreateDoubleMatrix(1, tate, mxREAL);
	memcpy((void *)mxGetPr(mx_u), (void *)u, sizeof(u));
	mx_Aeq = mxCreateDoubleMatrix(2, yoko, mxREAL);
	memcpy((void *)mxGetPr(mx_Aeq), (void *)Aeq, sizeof(Aeq));
	mx_beq = mxCreateDoubleMatrix(1, 2, mxREAL);
	memcpy((void *)mxGetPr(mx_beq), (void *)beq, sizeof(beq));
	mx_lb = mxCreateDoubleMatrix(1, yoko, mxREAL);
	memcpy((void *)mxGetPr(mx_lb), (void *)lb, sizeof(lb));

	engPutVariable(gEp, "mx_r", mx_r);
	engPutVariable(gEp, "mx_A", mx_A);
	engPutVariable(gEp, "mx_u", mx_u);
	engPutVariable(gEp, "mx_Aeq", mx_Aeq);
	engPutVariable(gEp, "mx_beq", mx_beq);
	engPutVariable(gEp, "mx_lb", mx_lb);

	//engOutputBuffer(gEp, buffer, EP_BUFFER_SIZE);
	//engEvalString(gEp, "mx_A");
	//printf("%s", buffer);

	engEvalString(gEp, "[p, fval] = linprog(mx_r, mx_A, mx_u, mx_Aeq, mx_beq, mx_lb, []);");
	//printf("%s", buffer);
	engEvalString(gEp, "p = p ./ 100;");
	engEvalString(gEp, "fval = fval / (-100);");
	//printf("%s", buffer);
	mx_p = engGetVariable(gEp, "p");
	p = mxGetPr(mx_p);
	mx_fval = engGetVariable(gEp, "fval");
	fval = mxGetPr(mx_fval);

	for(i=0; i<yoko; i++){
		pro[i/(NUM_STA+1)][i%(NUM_STA+1)] = p[i];
		printf("%f,", p[i]);
	}
	printf("\n\n");
	for(i=0; i<=NUM_STA; i++){
		for(j=0; j<=NUM_STA; j++){
			printf("%f,", pro[i][j]);
		}
		printf("\n");
	}

	double pd[NUM_STA]={};
	for(i=1;i<NUM_STA+1;i++){
		for(j=1;i<NUM_STA+1;j++){
			pd[i]+=p[i][j];
		}
	}
	/*printf("\n-----Optimization terminated.-----\n");
	for(i=0; i<yoko; i++){
		if(p[i]>0.00001){
			printf("   p[%d] = %f\n", i, p[i]);
		}
	}
	printf("   fval = %f\n\n", *fval);*/

	mxDestroyArray(mx_r);
	mxDestroyArray(mx_A);
	mxDestroyArray(mx_u);
	mxDestroyArray(mx_Aeq);
	mxDestroyArray(mx_beq);
	mxDestroyArray(mx_lb);
	mxDestroyArray(mx_p);
	mxDestroyArray(mx_fval);
	engEvalString(gEp, "close;");
}
