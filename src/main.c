#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include "nodeInfo.h"
#include "setting.h"
#include "initialization.h"
#include "success.h"
#include "collision.h"
#include "idle.h"
#include "result.h"
#include "debug.h"
#include "macro.h"
#include "probability.h"
#include "success.h"

#include "engine.h"
#include "matrix.h"
#include "tmwtypes.h"

double gElapsedTime;
std11 gStd;
simSpec gSpec;
FILE *gFileSta;

Engine *gEp;
double r[(NUM_STA+1)*(NUM_STA+1)] = {-1, -4, -45, -51, -29, -42, -16, -1, -25, -39, -24, -35, -3, -23, -1, -56, -78, -10, -11, -34, -22, -1, -7, -67, -45, -23, -65, -55, -1, -76, -12, -6, -95, -67, -52, -1};
double pro[NUM_STA+1][NUM_STA+1];
double dummyA[NUM_STA*2][(NUM_STA+1)*(NUM_STA+1)];
double A[NUM_STA*2][(NUM_STA+1)*(NUM_STA+1)];
double u[NUM_STA*2] = {-5, -10, -12.5, -7.5, -15, -10, -22.5, -2.5, -7.5, -7.5};;
double dummyAeq[2][(NUM_STA+1)*(NUM_STA+1)];
double Aeq[2][(NUM_STA+1)*(NUM_STA+1)];
double beq[2] = {100, 0};
double lb[(NUM_STA+1)*(NUM_STA+1)] = {};

void simSetting(int, char**);

int main(int argc, char *argv[]){
	//Check option values from command line.
	//checkOption(argc, argv);
	//Apply option values to simulation settings.

	if(!(gEp = engOpen(""))){
		fprintf(stderr, "\nCan't start MATLAB engine\n");
		return EXIT_FAILURE;
	}

	simSetting(argc,argv);
	initializeMatrix();
	/*if((gFileSta=fopen("sta's buffer.txt", "w"))==NULL){
		printf("File cannot open! 3");
		exit(33);
	}*/

	staInfo sta[NUM_STA];
	//sta = (staInfo*)malloc(sizeof(staInfo)*gSpec.numSta);
	apInfo ap;
	resultInfo result;
	//Intialize result information.
	initializeResult(&result);

	if(gSpec.fDebug==true){
		debug();
		//printf("End debug.\n");
		//exit(99);
	}

	int numTx = 0;
	int trialID;
	bool fEmpty = false;
	double lastBeacon = 0;

	for (trialID=0; trialID<gSpec.numTrial; trialID++){
		srand(trialID);
		numTx = 0;
		fEmpty = false;
		initializeNodeInfo(sta, &ap);
		calculateProbability(sta, 1);

		for( ;gElapsedTime<gSpec.simTime*1000000; ){
			transmission(sta, &ap);

			if(lastBeacon+100000<gElapsedTime){
				//calculateProbability(sta, 1);
				lastBeacon = gElapsedTime;
			}
		}

		/*gElapsedTime = (double)gStd.difs;
		idle(sta, &ap, &numTx, &fEmpty);
		//Wrong? fEmpty=true?

		for(; gElapsedTime<gSpec.simTime*1000000;){
			if(numTx==1){
				//debugSta(&sta[7],7);
				txSuccess(sta, &ap, &numTx);
				fEmpty = true;
				for(int i=0; i<gSpec.numSta; i++){
					if(sta[i].buffer[0].lengthMsdu!=0){
						fEmpty = false;
						break;
					}
				}
				if(ap.buffer[0].lengthMsdu!=0){
					fEmpty = false;
				}
				if(fEmpty==true){
					idle(sta, &ap, &numTx, &fEmpty);
				}else{
					afterSuccess(sta, &ap, &numTx);
				}
			}else{
				txCollision(sta, &ap);
				afterCollision(sta, &ap, &numTx);
			}
		}*/
		simulationResult(sta, &ap, &result, trialID);
	}

	if(gSpec.fOutput==true){
		fclose(gSpec.output);
	}
	//fclose(gFileSta);
	//free(sta);

	engClose(gEp);
	return 0;
}
