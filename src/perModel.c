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

void calculateRSSI(apInfo *ap, staInfo *sta, bool isTxSta){
	double RSSI = 1;
	//double distance = sqrt(pow(sta->x, 2) + pow(sta->y, 2));
	int i, j;
	double uplink, downlink;
	double noise = -91.63;
	double inter;
	double snr;
	double r_mat[NUM_STA+1][NUM_STA+1];
	/*
	if(isTxSta==false){
		RSSI = ap->txPower + ap->antennaGain + sta->antennaGain - (30*log10(distance) + 47);
	}else{
		RSSI = sta->txPower + ap->antennaGain + sta->antennaGain - (30*log10(distance) + 47);
	}*/
	for(i=0; i<NUM_STA+1; i++){
		for(j=0; j<NUM_STA+1; j++){
			if(i==0){
				RSSI = ap->txPower + ap->antennaGain + sta->antennaGain - (30*log10(distance(sta[j].x, sta[j].y)) + 47);
				uplink = 20*log2(1+pow(10,(RSSI-noise)/10));
				r_mat[i][j] = uplink;
			}else if(j==0){
				RSSI = ap->txPower + ap->antennaGain + sta->antennaGain - (30*log10(distance(sta[i].x, sta[i].y)) + 47);
				downlink = 20*log2(1+pow(10,(RSSI-noise)/10));
				r_mat[i][j] = downlink;
			}else if(i!=j){
				RSSI = ap->txPower + ap->antennaGain + sta->antennaGain - (30*log10(distance(sta[j].x, sta[j].y)) + 47);
				uplink = 20*log2(1+pow(10,(RSSI-noise)/10));
				RSSI = ap->txPower + ap->antennaGain + sta->antennaGain - (30*log10(distance(sta[i].x, sta[i].y)) + 47);
				inter = ap->txPower + ap->antennaGain + sta->antennaGain - (30*log10(distance(sta[i].x-sta[j].x, sta[i].y-sta[j].y)) + 47);
				snr = pow(10,(RSSI)/10)/(pow(10,(noise)/10)+pow(10,(inter)/10));
				downlink = 20*log2(1+snr);
				r_mat[i][j] = uplink+downlink;
			}else{
				r_mat[i][j] = 0;
			}
		}
	}

	for(i=0;i<NUM_STA+1;i++){
		for(j=0;j<NUM_STA+1;j++){
			r[i*(NUM_STA+1)+j] = r_mat[i][j];
		}
	}
}
