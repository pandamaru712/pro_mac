#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "success.h"
#include "setting.h"
#include "bufferManager.h"
#include "idle.h"
#include "probability.h"
#include "limits.h"

extern double gElapsedTime;
extern simSpec gSpec;
extern std11 gStd;

void transmission(staInfo sta[], apInfo *ap){
	/*
	APは非飽和の場合は考慮していない
	フレームの長さや伝送速度の違いは考慮していない
	*/
	bool fUpColl = false;
	bool fNoUplink = false;
	bool fNoDownlink = false;
	int minBackoff;
	int i;
	int txFrameLength = 0;
	int txTimeFrameLength;
	int totalTime;
	//int rxSta = INT_MAX;
	minBackoff = selectNode(sta, &fUpColl, &fNoUplink, &fNoDownlink);

	if(fNoUplink==true && fNoDownlink==true){
		printf("Error! (106)\n");
	}

	if(fUpColl==false){
		//Uplinl successed.
		if(fNoDownlink==false){
			ap->sumFrameLengthInBuffer -= ap->buffer[0].lengthMsdu;
			ap->byteSuccFrame += ap->buffer[0].lengthMsdu;
			txFrameLength = ap->buffer[0].lengthMsdu;
			/*if(txFrameLength<ap->buffer[0].lengthMsdu){
				txFrameLength = ap->buffer[0].lengthMsdu;
			}*/
			ap->buffer[0].lengthMsdu = 0;
			ap->sumDelay += (gElapsedTime - ap->buffer[0].timeStamp);
			ap->buffer[0].timeStamp = 0;
			ap->numSuccFrame++;
			ap->numTxFrame++;
			//ap->numPrimFrame++;
			swapAp(ap);
		}

		if(fNoUplink==false){
			for(i=0; i<gSpec.numSta; i++){
				if(sta[i].fTx==true){
					//sta[i].backoffCount = rand() % (sta[i].cw + 1);
					sta[i].fTx = false;
					sta[i].sumFrameLengthInBuffer -= sta[i].buffer[0].lengthMsdu;
					sta[i].byteSuccFrame += sta[i].buffer[0].lengthMsdu;
					if(txFrameLength<sta[i].buffer[0].lengthMsdu){
						txFrameLength = sta[i].buffer[0].lengthMsdu;
					}
					sta[i].buffer[0].lengthMsdu = 0;
					//printf("%f\n", sta[i].buffer[0].timeStamp);
					sta[i].sumDelay += (gElapsedTime - sta[i].buffer[0].timeStamp);
					sta[i].buffer[0].timeStamp = 0;
					sta[i].numTxFrame++;
					sta[i].numSuccFrame++;
					swapSta(&sta[i]);
				}else{
					sta[i].fTx = false;
					/*if((sta[i].buffer[0].lengthMsdu!=0)&&(sta[i].backoffCount!=0)){
						sta[i].backoffCount--;
					}*/
				}
			}
		}
		txTimeFrameLength = gStd.phyHeader + 4 * ((gStd.macService + 8* (gStd.macHeader + txFrameLength + gStd.macFcs) + gStd.macTail + (4 * gStd.dataRate - 1)) / (4 * gStd.dataRate));
		totalTime = (double)minBackoff * gStd.slot + txTimeFrameLength + gStd.sifs + gStd.timeAck;
		gElapsedTime += (double)totalTime;
		arriveAp(ap, (double)totalTime);
		for(i=0; i<gSpec.numSta; i++){
			arriveSta(&sta[i], (double)totalTime);
			sta[i].fRx = false;
		}
	}else{
		//Uplink failed.
		if(fNoDownlink==false){
			ap->sumFrameLengthInBuffer -= ap->buffer[0].lengthMsdu;
			ap->byteSuccFrame += ap->buffer[0].lengthMsdu;
			txFrameLength = ap->buffer[0].lengthMsdu;
			/*if(txFrameLength<ap->buffer[0].lengthMsdu){
				txFrameLength = ap->buffer[0].lengthMsdu;
			}*/
			ap->buffer[0].lengthMsdu = 0;
			ap->sumDelay += (gElapsedTime - ap->buffer[0].timeStamp);
			ap->buffer[0].timeStamp = 0;
			ap->numSuccFrame++;
			ap->numTxFrame++;
			//ap->numPrimFrame++;
			swapAp(ap);
		}
		for(i=0; i<gSpec.numSta; i++){
			if(sta[i].fTx==true){
				//sta[i].backoffCount = rand() % (sta[i].cw + 1);
				sta[i].fTx = false;
				sta[i].numTxFrame++;
				sta[i].numCollFrame++;
				if(txFrameLength<sta[i].buffer[0].lengthMsdu){
					txFrameLength = sta[i].buffer[0].lengthMsdu;
				}
			}else{
				sta[i].fTx = false;
				/*if((sta[i].buffer[0].lengthMsdu!=0)&&(sta[i].backoffCount!=0)){
					sta[i].backoffCount--;
					sta[i].fRx = false;
				}*/
			}
		}
		txTimeFrameLength = gStd.phyHeader + 4 * ((gStd.macService + 8* (gStd.macHeader + txFrameLength + gStd.macFcs) + gStd.macTail + (4 * gStd.dataRate - 1)) / (4 * gStd.dataRate));
		//totalTime = txTimeFrameLength + gStd.sifs + gStd.timeAck;
		gElapsedTime += (double)txTimeFrameLength + (double)minBackoff*gStd.slot;
		arriveAp(ap, (double)txTimeFrameLength);
		for(i=0; i<gSpec.numSta; i++){
			arriveSta(&sta[i], (double)txTimeFrameLength);
			sta[i].fRx = false;
		}
	}
}
