#ifndef _perModel_h
#define _perModel_h

#include "nodeInfo.h"
#include "setting.h"

void calculateRSSI(apInfo*, staInfo*);
double distance(staInfo*, int, int);
void calculateDelay(staInfo*);
void calculatePhyRate(apInfo*, staInfo* /*, int*, int*, bool*/);
#endif
