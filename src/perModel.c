#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "nodeInfo.h"
#include "perModel.h"

extern double r[(NUM_STA+1)*(NUM_STA+1)];

double distance(double x, double y){
	double distance = sqrt(pow(x, 2) + pow(y, 2));
	return distance;
}

void calculateRSSI(apInfo *ap, staInfo sta[]){
	double RSSI = 1;
	//double distance = sqrt(pow(sta->x, 2) + pow(sta->y, 2));
	int i, j;
	double uplink, downlink;
	double noise = -91.63;
	double ICInterference;   //Inter-client interference
	double snr;
	double r_mat[NUM_STA+1][NUM_STA+1] = {};
	/*
	if(isTxSta==false){
		RSSI = ap->txPower + ap->antennaGain + sta->antennaGain - (30*log10(distance) + 47);
	}else{
		RSSI = sta->txPower + ap->antennaGain + sta->antennaGain - (30*log10(distance) + 47);
	}*/
	for(i=0; i<NUM_STA+1; i++){
		for(j=0; j<NUM_STA+1; j++){
			if(i==j){
				r_mat[i][j] = 0;
			}else if(i==0){
				RSSI = sta[j-1].txPower + sta[j-1].antennaGain + ap->antennaGain - (30*log10(distance(sta[j-1].x, sta[j-1].y)) + 47);
				uplink = 20*log2(1+pow(10,(RSSI-noise)/10));
				r_mat[i][j] = uplink;
			}else if(j==0){
				RSSI = ap->txPower + ap->antennaGain + sta[i-1].antennaGain - (30*log10(distance(sta[i-1].x, sta[i-1].y)) + 47);
				downlink = 20*log2(1+pow(10,(RSSI-noise)/10));
				r_mat[i][j] = downlink;
			}else if(i!=j){
				RSSI = sta[j-1].txPower + ap->antennaGain + sta[j-1].antennaGain - (30*log10(distance(sta[j-1].x, sta[j-1].y)) + 47);
				snr = pow(10,RSSI/10) / (pow(10,noise/10) + pow(10,(sta->txPower-gSpec.SIFC)/10));
				uplink = 20*log2(1+snr);
				RSSI = ap->txPower + ap->antennaGain + sta[i].antennaGain - (30*log10(distance(sta[i-1].x, sta[i].y)) + 47);
				ICInterference = sta[j-1].txPower + sta[j-1].antennaGain + sta[i-1].antennaGain - (30*log10(distance(sta[i-1].x-sta[j-1].x, sta[i-1].y-sta[j-1].y)) + 47);
				snr = pow(10,(RSSI)/10)/(pow(10,(noise)/10)+pow(10,(ICInterference)/10));
				downlink = 20*log2(1+snr);
				r_mat[i][j] = uplink+downlink;
			}else{
				printf("RSSI error!\n");
			}
		}
	}

	printf("\n***** Rate Matrix *****\n");
	for(i=0;i<NUM_STA+1;i++){
		for(j=0;j<NUM_STA+1;j++){
			r[i*(NUM_STA+1)+j] = -r_mat[i][j];
			printf("%f, ", r_mat[i][j]);
		}
		printf("\n");
	}
	printf("***** Rate Matrix *****\n\n");
}
