#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <time.h>
#include "setting.h"
#include "macro.h"

extern simSpec gSpec;
extern std11 gStd;

static struct option options[] = {
	{"help", no_argument, NULL, 'h'},
	{"debug", no_argument, NULL, 'd'},
	{"fd", no_argument, NULL, 'f'},
	{"ofdma", no_argument, NULL, 'o'},
	{"std", required_argument, NULL, 's'},
	{"numSta", required_argument, NULL, 'n'},
	{"simTime", required_argument, NULL, 't'},
	{"traffic", required_argument, NULL, 'l'},
	{"trial", required_argument, NULL, 'r'},
	{"lambdaSta", required_argument, NULL, 'm'},
	{"delay", required_argument, NULL, 'a'},
	{"output",required_argument, NULL, 'u'},
	{"area", required_argument, NULL, 'b'},
	{"opt", required_argument, NULL, 'p'},
	{"position", required_argument, NULL, 'x'},
	{"power", required_argument, NULL, 'w'},
	{0, 0, 0, 0}
};

void simSetting(int argc, char **argv){
	int opt;
	int index;

	gSpec.fDebug = false;
	gSpec.simTime = 1;   //sec
	gSpec.fFd = false;
	gSpec.fOfdma = false;
	gStd.std = NULL;
	gSpec.numSta = NUM_STA;
	gSpec.trafficPattern = 0;
	gSpec.numTrial = 1;
	gSpec.lambdaSta = 0.1;
	gSpec.delayMode = 0;   //from arriving at buffer to transmitting
	gSpec.areaSize = 100;   //m
	gSpec.fOutput = false;
	gSpec.proMode = 0;
	gSpec.position = 0;
	gSpec.delayPower = 1;
	memset(gSpec.filename, '\0', strlen(gSpec.filename));

	time_t timer;
	struct tm *local;
	timer = time(NULL);
	local = localtime(&timer);

	while((opt = getopt_long(argc, argv, "hdfos:n:t:l:r:m:a:u:b:p:x:w:", options, &index)) != -1){
		switch(opt){
			case 'h':
				printf(
					"   -h, --help: Show this help.\n"
					"   -d, --debug: Debug mode.\n"
					"   -f, --fd: Full-duplex mode.\n"
					"   -o, --ofdma: OFDMA mode.\n"
					"   -s, --std: Select standard from a/n/ac.\n"
					"   -n, --numSta: Number of STAs.\n"
					"   -t, --simTime: Simulation time (sec).\n"
					"   -l, --traffic: Traffic Pattern.\n"
					"   -r, --trial: number of Simulation runs.\n"
					"   -m, --lambdaSta: Lambda of STA (/us).\n"
					"   -a, --delay: delayMode (0/1).\n"
					"   -u, --output: Output filename.\n"
					"   -b, --area: Area size (m).\n"
					"   -x, --position: Node's position.\n"
					"   -w, --power: delayPower.\n"
					"      0; AP is (0, 0) and STAs are deployed concentrically.\n"
					"      1; AP is (0, 0) and STAs are randomly deployed.\n"
					"      2; AP and STAs are randomly deployed.\n"
					"   -p, --opt: Evaluation function mode.\n"
					"      0: Data rate.\n"
					"      1: Data rate x delay.\n"
					"      2: delay is used to subject.\n"
				);
				exit(1);
				break;
			case 'd':
				gSpec.fDebug = true;
				break;
			case 'f':
				gSpec.fFd = true;
				break;
			case 'o':
				gSpec.fOfdma = true;
				break;
			case 's':
				gStd.std = optarg;
				break;
			case 'n':
				gSpec.numSta = NUM_STA;//atoi(optarg);
				break;
			case 't':
				gSpec.simTime = atoi(optarg);
				break;
			case 'l':
				gSpec.trafficPattern = atoi(optarg);
				break;
			case 'm':
				gSpec.lambdaSta = atof(optarg);
				break;
			case 'r':
				gSpec.numTrial = atoi(optarg);
				break;
			case 'a':
				gSpec.delayMode = atoi(optarg);
				break;
			case 'u':
				if(optarg!=NULL){
					gSpec.fOutput = true;
					sprintf(gSpec.filename, "data/%s.txt", optarg);
				}
				break;
			case 'b':
				gSpec.areaSize = atoi(optarg);
				break;
			case 'p':
				gSpec.proMode = atoi(optarg);
				break;
			case 'x':
				gSpec.position = atoi(optarg);
				break;
			case 'w':
				gSpec.delayPower = atof(optarg);
				break;
			default:
				printf("Illegal options! \'%c\' \'%c\'\n", opt, optopt);
				exit(1);
		}
	}

	printf("-----Settings-----\n");
	if(gSpec.fDebug==true){
		printf("   Debug mode.\n");
	}
	if(gSpec.fFd==true){
		printf("   Full-duplex mode.\n");
	}
	if(gSpec.fOfdma==true){
		printf("   OFDMA mode.\n");
	}
	if(gStd.std!=NULL){
		printf("   Standard is 11%s.\n", gStd.std);
	}
	printf("   Number of STA is %d.\n", gSpec.numSta);
	printf("   Simulation time is %d sec.\n", gSpec.simTime);
	printf("   Simulation runs %d times.\n", gSpec.numTrial);
	printf("   Lambda of STA is %f /us.\n", gSpec.lambdaSta);
	if(gSpec.trafficPattern==0){
		printf("   Traffic pattern is 1500/1500.\n");
		printf("   Offered load of STA is %f Mbit/s.\n", gSpec.lambdaSta*1500*8);
	}else if(gSpec.trafficPattern==1){
		printf("   Traffic pattern is 1500/500.\n");
		printf("   Offered load of STA is %f Mbit/s.\n", gSpec.lambdaSta*500*8);
	}
	printf("   Area size is %d m.\n", gSpec.areaSize);
	printf("   Delay Mode is %d.\n", gSpec.delayMode);
	if(gSpec.fOutput==false){
		printf("   No output files.\n");
	}else{
		printf("   Output to %s.\n", gSpec.filename);
	}
	printf("   PRO_MODE is %d.\n", gSpec.proMode);
	if(gSpec.position==0){
		printf("   AP is (0, 0) and STAs are deployed concentrically.\n");
	}else if(gSpec.position==1){
		printf("   AP is (0, 0) and STAs are randomly deployed.\n");
	}else if(gSpec.position==2){
		printf("   AP and STAs are randomly deployed.\n");
	}else{
		printf("   Position option is failed.\n");
		exit(1);
	}
	printf("   delayPower is %f.\n", gSpec.delayPower);
	printf("------------------\n");

	if(gSpec.fOutput==true){
		if((gSpec.output=fopen(gSpec.filename, "a"))==NULL){
			printf("Output file cannot be opened!\n");
			exit(33);
		}
	}

	if(gSpec.fOutput==true){
		fprintf(gSpec.output, "Simulation ran at %02d/%02d/%02d_%02d:%02d:%02d.\n", local->tm_year-100, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
		fprintf(gSpec.output, "-----Settings-----\n");
		if(gSpec.fDebug==true){
			fprintf(gSpec.output, "   Debug mode.\n");
		}
		if(gSpec.fFd==true){
			fprintf(gSpec.output, "   Full-duplex mode.\n");
		}
		if(gSpec.fOfdma==true){
			fprintf(gSpec.output, "   OFDMA mode.\n");
		}
		if(gStd.std!=NULL){
			fprintf(gSpec.output, "   Standard is 11%s.\n", gStd.std);
		}
		fprintf(gSpec.output, "   Number of STA is %d.\n", gSpec.numSta);
		fprintf(gSpec.output, "   Simulation time is %d sec.\n", gSpec.simTime);
		fprintf(gSpec.output, "   Simulation runs %d times.\n", gSpec.numTrial);
		fprintf(gSpec.output, "   Lambda of STA is %f /us.\n", gSpec.lambdaSta);
		if(gSpec.trafficPattern==0){
			fprintf(gSpec.output, "   Traffic pattern is 1500/1500.\n");
			fprintf(gSpec.output, "   Offered load of STA is %f Mbit/s.\n", gSpec.lambdaSta*1500*8);
		}else if(gSpec.trafficPattern==1){
			fprintf(gSpec.output, "   Traffic pattern is 1500/500.\n");
			fprintf(gSpec.output, "   Offered load of STA is %f Mbit/s.\n", gSpec.lambdaSta*500*8);
		}
		fprintf(gSpec.output, "   Delay Mode is %d.\n", gSpec.delayMode);
		fprintf(gSpec.output, "   Output to %s.\n", gSpec.filename);
		fprintf(gSpec.output, "------------------\n");
	}

	if(gSpec.fOfdma==true && gSpec.numSta<2){
		printf("Connot OFDMA mode.\n");
		exit(10);
	}

	gStd.dataRate = 54;
	gStd.ackRate = 24;
	gStd.rtsRate = 54;
	gStd.ctsRate = 24;
	gStd.ackLength = 10;
	gStd.rtsLength = 16;
	gStd.ctsLength = 10;
	gStd.phyHeader = 20;
	gStd.macService = 16;
	gStd.macHeader = 24;
	gStd.macFcs = 4;
	gStd.macTail = 6;
	gStd.timeAck = gStd.phyHeader + 4 * ((gStd.macService + 8 * (gStd.ackLength + gStd.macFcs) + gStd.macTail + (4 * gStd.ackRate - 1)) / (4 * gStd.ackRate));
	gStd.timeRts = gStd.phyHeader + 4 * ((gStd.macService + 8 * (gStd.rtsLength + gStd.macFcs) + gStd.macTail + (4 * gStd.rtsRate - 1)) / (4 * gStd.rtsRate));
	gStd.timeCts = gStd.phyHeader + 4 * ((gStd.macService + 8 * (gStd.ctsLength + gStd.macFcs) + gStd.macTail + (4 * gStd.ctsRate - 1)) / (4 * gStd.ctsRate));
	gStd.sifs = 16;
	gStd.difs = 34;
	gStd.eifs = gStd.sifs + gStd.timeAck + gStd.difs;
	gStd.slot = 9;
	gStd.afterColl = gStd.difs;
	gStd.afterSucc = gStd.difs;
	gStd.ackTimeout = gStd.sifs + gStd.timeAck + gStd.slot;
	gStd.ctsTimeout = gStd.sifs + gStd.timeCts + gStd.slot;
	gStd.retryLimit = 6;
	gStd.cwMin = 15;
	gStd.cwMax = 1023;

	gSpec.bufferSizeByte = 200;
	gSpec.lambdaAp = 0.1;
	gSpec.SIC = 110;   //Self-interference cancelation [dB]
	gSpec.ICIth = 5;
	gSpec.noise = -91.63;
}
