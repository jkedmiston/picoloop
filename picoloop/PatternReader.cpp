#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Pattern.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "Master.h"
#include "PatternReader.h"
#include "SYSTEM.h"

#include <sys/types.h>
#include <sys/stat.h>


using namespace std;

PatternReader::PatternReader() : twoDPVector(MAX_PATTERN_BY_PROJECT,vector <Pattern>(TRACK_MAX)), 
				 loadedData(MAX_PATTERN_BY_PROJECT, vector     <int>(TRACK_MAX))

				 //, 
				 //				 savedData(16,vector       <int>(TRACK_MAX))
{
  DPRINTF("PatternReader::PatternReader()\n");
  bank=0;
}

PatternReader::~PatternReader()
{
  DPRINTF("PatternReader::~PatternReader()\n");
}

void PatternReader::init()
{
  int x;
  int y;
  
  for (x=0;x<MAX_PATTERN_BY_PROJECT;x++)
    for (y=0;y<TRACK_MAX;y++)
      {
	loadedData[x][y]=DATA_HAS_NOT_BEEN_CHECK;
	//	savedData[x][y]=0;
	twoDPVector[x][y].init();
      }
  bank=0;
}

void PatternReader::setBank(int b)
{
  if (b>=0 && b < 256)
    bank=b;
}

int PatternReader::getBank()
{
  return bank;
}


int PatternReader::saveSong(SongSequencer & SS)
{
  char filename[1024];
  unsigned char line[MAX_PATTERN_BY_PROJECT];
  int i;
  int j;
  sprintf(filename,"bank/bank%d/song.pic",bank);
  fd=fopen(filename,"w");
  if (fd==0)
    {
      return 0;
    }
  for (j=0;j<TRACK_MAX;j++)
    {
      for (i=0;i<MAX_PATTERN_BY_PROJECT;i++)
	{
	  line[i]=SS.songVector[i][j];
	}
      fwrite(line,sizeof(unsigned char),sizeof(unsigned char)*MAX_PATTERN_BY_PROJECT,fd);
    }
  fclose(fd);
}

int PatternReader::loadSong(SongSequencer & SS)
{
  char  filename[1024];
  unsigned char line[MAX_PATTERN_BY_PROJECT]={0};
  int i;
  int j;
  sprintf(filename,"bank/bank%d/song.pic",bank);
  fd=fopen(filename,"r");
  if (fd==0)
    {
      for (j=0;j<TRACK_MAX;j++)
	for (i=0;i<MAX_PATTERN_BY_PROJECT;i++)
	  {
	    SS.songVector[i][j]=line[i];
	  }
      return 0;
    }
  for (j=0;j<TRACK_MAX;j++)
    {
      fread(line,sizeof(unsigned char),sizeof(unsigned char)*MAX_PATTERN_BY_PROJECT,fd);
      for (i=0;i<MAX_PATTERN_BY_PROJECT;i++)
	{
	  SS.songVector[i][j]=line[i];
	}
      //fwrite(line,sizeof(char),sizeof(char)*MAX_PATTERN_BY_PROJECT,fd);
    }
  fclose(fd);
}

bool PatternReader::PatternRemove(int PatternNumber,int TrackNumber)
{
  char filename[1024];
  sprintf(filename,"bank/bank%d/dataP%dT%d.pic",bank,PatternNumber,TrackNumber);
  if (unlink(filename)==0)
    {
      loadedData[PatternNumber][TrackNumber]=DATA_DOES_NOT_EXIST_ON_STORAGE;
      return true;
    }
  else
    return false;
}


bool PatternReader::PatternDataExist(int PatternNumber,int TrackNumber)
{
  int match=0;
  //char * line;
  int PatNum,TrackNum,PatSize;
  bool retcode=true;
  bool found=false;
  char line[1024];
  char filename[1024];

  //printf("?EXIST (%d %d)\n");

  if (loadedData[PatternNumber][TrackNumber]==DATA_EXIST_ON_STORAGE | 
      loadedData[PatternNumber][TrackNumber]==DATA_LOADED_FROM_STORAGE)
    return true;
  if (loadedData[PatternNumber][TrackNumber]==DATA_DOES_NOT_EXIST_ON_STORAGE)
    return false;

  //line=(char*)malloc(1024);
  //fd=fopen(fn.c_str(),"r+");
  sprintf(filename,"bank/bank%d/dataP%dT%d.pic",bank,PatternNumber,TrackNumber);
  fd=fopen(filename,"r+");
  if (fd==0)
    {
      loadedData[PatternNumber][TrackNumber]=DATA_DOES_NOT_EXIST_ON_STORAGE;
      return false;
    }

  while (match==0 && retcode==true)
    {
      char * catcheof;
      catcheof=fgets(line,512,fd);
      //printf("[%s]\n",st);
      //sleep(1);

      //match('Pattern 1 Track 1 Param PatternSize 16')
      sscanf(line,"Pattern %d Track %d Param PatternSize %d",
	     &PatNum,&TrackNum,&PatSize);
      if (PatNum    ==PatternNumber &&
	  TrackNum  ==TrackNumber)
	match=1;

      if ( catcheof==NULL)
	retcode=false;
    }

  if (match)
    found=true;

  if (found==true)
    loadedData[PatternNumber][TrackNumber]=DATA_EXIST_ON_STORAGE;
  else
    loadedData[PatternNumber][TrackNumber]=DATA_DOES_NOT_EXIST_ON_STORAGE;


  //free(line);
  fclose(fd);
  return found;
}

bool PatternReader::writePatternDataLine(int PatternNumber,int TrackNumber, Pattern & P, char * line, int machineParam)
{
  for (int i=0; i< P.getSize();i++)
    {
      if (i==0)
	sprintf(line,"Pattern %d Track %d Param %s ",PatternNumber,TrackNumber,this->getParameterCharStar(machineParam));
      sprintf(line+strlen(line),"%d ",P.getPatternElement(i).get(machineParam));
    }
  sprintf(line+strlen(line),"\n");
}


bool PatternReader::readPatternDataLine(int PatternNumber,int TrackNumber, Pattern & P, char * line, int machineParam)
{
  PatternElement Pe;
  bool retcode=true;
  int n[16];
  char strParam[64];
  int PatNum;
  int TrackNum;
  int i;
  //int PatSize;


  //match('Pattern 1 Track 1 Param Note 41 0 52 0 22 0 12 0 21 11 14 12 43 12 54')
  sscanf(line,
	 "Pattern %d Track %d Param %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
	 &PatNum,&TrackNum,strParam,
	 &n[0], &n[1], &n[2], &n[3],
	 &n[4], &n[5], &n[6], &n[7],
	 &n[8], &n[9], &n[10],&n[11],
	 &n[12],&n[13],&n[14],&n[15]);
  
  // printf("READ    : %s\n",line);
  // printf("COMPARE : Pattern %d Track %d Param %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",	PatNum,TrackNum,strParam,
  // 	 n[0], n[1], n[2], n[3],
  // 	 n[4], n[5], n[6], n[7],
  // 	 n[8], n[9], n[10],n[11],
  // 	 n[12],n[13],n[14],n[15]);

  // printf("[%s] [%s] [%d]\n",
  // 	 strParam,
  // 	 this->getParameterCharStar(machineParam),
  // 	 strcmp(strParam,this->getParameterCharStar(machineParam))
  // 	 );

  if (PatNum    ==PatternNumber &&
      TrackNum  ==TrackNumber   &&
      strcmp(strParam,this->getParameterCharStar(machineParam))==0)
    for (i=0;i<P.getSize();i++)
      {
	//printf("MATCHED\n");
	Pe=P.getPatternElement(i);      
	Pe.set(machineParam,n[i]);
	P.setPatternElement(i,Pe);      
      }
  else
    retcode=false;

  return retcode;
}

bool PatternReader::readPatternData(int PatternNumber,int TrackNumber, Pattern & P)
{
  PatternElement Pe;
  FILE *fd;
  int PatNum;
  int TrackNum;
  int PatSize;
  int PatBPM;
  int PatBPMDivider;
  int PatSwing;
  int n[16];
  int t[16];
  int i;
  bool retcode=true;
  int match=0;
  //char * line;
  //line=(char*)malloc(1024);
  char line[1024];
  char filename[1024];
  char strParam[64];
  int  machineParam;


  if (loadedData[PatternNumber][TrackNumber]==DATA_LOADED_FROM_STORAGE)
    {
      P=twoDPVector[PatternNumber][TrackNumber];
      return true;
    }

  sprintf(filename,"bank/bank%d/dataP%dT%d.pic",bank,PatternNumber,TrackNumber);


  //check if file name can be open
  //fd=fopen(fn.c_str(),"r+");
  fd=fopen(filename,"r+");
  if (fd==0)
    {
      DPRINTF("[data file %s not found]\n",fn.c_str());
      loadedData[PatternNumber][TrackNumber]=DATA_DOES_NOT_EXIST_ON_STORAGE;
      //twoDPVector[PatternNumber][TrackNumber].init();
      P.init();
      twoDPVector[PatternNumber][TrackNumber]=P;
      return false;
    }
  
  while (match==0 && retcode==true)
    {
      char * catcheof;
      catcheof=fgets(line,512,fd);
      //printf("[%s]\n",st);
      //sleep(1);
      
      //match('Pattern 1 Track 1 Param PatternSize 16')
      sscanf(line,"Pattern %d Track %d Param PatternSize %d",
	     &PatNum,&TrackNum,&PatSize);
      //printf("%d %d %d",PatNum,TrackNum,PatSize);
      if (PatNum    ==PatternNumber &&
	  TrackNum  ==TrackNumber)
	match=1;
      if ( catcheof==NULL)
	retcode=false;
    }

  if (retcode==false)
    {
      P.setPatternSize(16);
      return false;
    }
  
  P.setPatternSize(PatSize);
  
  if (fgets(line,512,fd))
    {
      //match('Pattern 1 Track 1 Param PatternBPM 80')
      sscanf(line,"Pattern %d Track %d Param PatternBPM %d",
	     &PatNum,&TrackNum,&PatBPM);
      if (PatNum    ==PatternNumber &&
	  TrackNum  ==TrackNumber)
	{
	  P.setBPM((float)PatBPM);
	}
    }

  if (fgets(line,512,fd))
    {
      //match('Pattern 1 Track 1 Param PatternBPMDivider 4')
      sscanf(line,"Pattern %d Track %d Param PatternBPMDivider %d",
	     &PatNum,&TrackNum,&PatBPMDivider);
      if (PatNum    ==PatternNumber &&
	  TrackNum  ==TrackNumber)
	{
	  P.setBPMDivider(PatBPMDivider);
	}
    }


  if (fgets(line,512,fd))
    {
      //match('Pattern 1 Track 1 Param PatternBPMDivider 4')
      sscanf(line,"Pattern %d Track %d Param PatternSwing %d",
	     &PatNum,&TrackNum,&PatSwing);
      if (PatNum    ==PatternNumber &&
	  TrackNum  ==TrackNumber)
	{
	  P.setSwing(PatSwing);
	}
    }

  machineParam=LFO1_DEPTH;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);

  machineParam=LFO1_FREQ;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);


  machineParam=AMP;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);



  machineParam=NOTE;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);


  machineParam=NOTE_ON;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);

  


  machineParam=VCO_MIX;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);

  






  machineParam=ADSR_AMP_ATTACK;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);



  machineParam=ADSR_AMP_DECAY;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);




  machineParam=ADSR_AMP_SUSTAIN;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);



  machineParam=ADSR_AMP_RELEASE;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);


  machineParam=OSC1_TYPE;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);


  machineParam=OSC2_TYPE;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);



  machineParam=FILTER1_CUTOFF;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);



  machineParam=FILTER1_RESONANCE;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);



  machineParam=OSC1_PHASE;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);



  machineParam=ADSR_FLTR_ATTACK;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);



  machineParam=ADSR_FLTR_DECAY;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);




  machineParam=ADSR_FLTR_SUSTAIN;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);



  machineParam=ADSR_FLTR_RELEASE;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);




  machineParam=NOTE_ADSR;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);




  machineParam=MACHINE_TYPE;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);



  machineParam=FILTER1_ALGO;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);


  machineParam=FILTER1_TYPE;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);


  machineParam=OSC1_AMP;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);




  machineParam=OSC2_AMP;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);


  machineParam=FX1_DEPTH;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);


  machineParam=FX1_SPEED;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);



  machineParam=FM_TYPE;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);



  machineParam=TRIG_TIME_DURATION;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);


  machineParam=PITCHBEND_DEPTH;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);


  machineParam=PITCHBEND_SPEED;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);



  machineParam=LFO_TYPE;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);


  machineParam=OSC1_DETUNE;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);


  machineParam=LFO1_WAVEFORM;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);

  machineParam=LFO2_WAVEFORM;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);

  machineParam=LFO1_ENV_AMOUNT;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);

  machineParam=LFO2_ENV_AMOUNT;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);

  machineParam=LFO2_DEPTH;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);

  machineParam=LFO2_FREQ;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);


  machineParam=VELOCITY;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);

  machineParam=OSC1_MOD;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);

  machineParam=OSC1_UNISON;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);

  machineParam=OSC2_UNISON;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);

  machineParam=OSC1_UNISONDT;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);

  machineParam=OSC2_UNISONDT;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);


  machineParam=FILTER1_SATURATION;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);

  machineParam=FILTER1_FEEDBACK;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);


  machineParam=OSC3_AMP;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);

  machineParam=OSC4_AMP;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);

  machineParam=OSC3_TYPE;
  fgets(line,512,fd);
  this->readPatternDataLine(PatternNumber,TrackNumber,P,line,machineParam);


  if (retcode==true)
    {
      loadedData[PatternNumber][TrackNumber]=DATA_LOADED_FROM_STORAGE;
      twoDPVector[PatternNumber][TrackNumber]=P;
    }
  
  //free(line);
  fclose(fd);
  return retcode;
}


bool PatternReader::writePattern(int PatternNumber, int TrackNumber, Pattern & P)
{  
  FILE         * fd=NULL;
  int            PatNum=0;
  int            TrackNum=0;
  int            PatSize=0;  
  int            match=0;
  vector<string> data;
  bool           retcode=true;
  bool           found=false;
  //char         * line=NULL;
  //char         * line2=NULL;
  char         * catcheof=NULL;
  //line=(char*)malloc(1024);
  //line2=(char*)malloc(1024);
  char line[1024];
  char line2[1024];
  char filename[1024];
  char path[1024];
  mode_t mode;

  //mode=0666;
  //mode=S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IWGRP|S_IROTH|S_IWOTH;

  //mode=0755;
  
  sprintf(path,    "bank");
  //mkdir(path,mode);
  MKDIR(path);

  sprintf(path,    "bank/bank%d",bank);
  //mkdir(path,mode);
  MKDIR(path);

  sprintf(filename,"bank/bank%d/dataP%dT%d.pic",bank,PatternNumber,TrackNumber);

  //  printf("SIZE:%d\n",P.getSize());
  //  exit(1);

  //  loadedData[PatternNumber][TrackNumber]==0;
  //savedData[PatternNumber][TrackNumber]==0;

  
  //fd=fopen(fn.c_str(),"r+");
  fd=fopen(filename,"r+");
  if (fd==0)
    {
      DPRINTF("[data file %s not found]\n",fn.c_str());
      retcode=false;
      //exit(213);
    }
  else
    {

      while (retcode==true)
	{
	  catcheof=fgets(line,512,fd);
	  sscanf(line,"Pattern %d Track %d ",
		 &PatNum,&TrackNum);
	  if (PatNum    ==  PatternNumber &&
	      TrackNum  ==  TrackNumber)
	    found=true;
	  else
	    {
	      data.insert(data.end(),line);
	    }
	  if ( catcheof==NULL)
	    retcode=false;
	}
      fclose(fd);
    }
  //  data.insert(data.end(),"\n");

  //insert 'Pattern 2 Track 2 Param PatternSize 16'
  sprintf(line,"Pattern %d Track %d Param PatternSize %d\n",
	  PatternNumber,
	  TrackNumber, 
	  P.getSize());
  data.insert(data.end(),line);


  sprintf(line,"Pattern %d Track %d Param PatternBPM %d\n",
	  PatternNumber,
	  TrackNumber, 
	  P.getBPM());
  data.insert(data.end(),line);

  sprintf(line,"Pattern %d Track %d Param PatternBPMDivider %d\n",
	  PatternNumber,
	  TrackNumber, 
	  P.getBPMDivider());
  data.insert(data.end(),line);


  sprintf(line,"Pattern %d Track %d Param PatternSwing %d\n",
	  PatternNumber,
	  TrackNumber, 
	  P.getSwing());
  data.insert(data.end(),line);


  //line=""; //Weird ?


  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,LFO1_DEPTH);
  data.insert(data.end(),line);

  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,LFO1_FREQ);
  data.insert(data.end(),line);


  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,AMP);
  data.insert(data.end(),line);


  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,NOTE);
  data.insert(data.end(),line);




  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,NOTE_ON);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,VCO_MIX);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,ADSR_AMP_ATTACK);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,ADSR_AMP_DECAY);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,ADSR_AMP_SUSTAIN);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,ADSR_AMP_RELEASE);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,OSC1_TYPE);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,OSC2_TYPE);
  data.insert(data.end(),line);




  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,FILTER1_CUTOFF);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,FILTER1_RESONANCE);
  data.insert(data.end(),line);




  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,OSC1_PHASE);
  data.insert(data.end(),line);




  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,ADSR_FLTR_ATTACK);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,ADSR_FLTR_DECAY);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,ADSR_FLTR_SUSTAIN);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,ADSR_FLTR_RELEASE);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,NOTE_ADSR);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,MACHINE_TYPE);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,FILTER1_ALGO);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,FILTER1_TYPE);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,OSC1_AMP);
  data.insert(data.end(),line);




  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,OSC2_AMP);
  data.insert(data.end(),line);




  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,FX1_DEPTH);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,FX1_SPEED);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,FM_TYPE);
  data.insert(data.end(),line);




  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,TRIG_TIME_DURATION);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,PITCHBEND_DEPTH);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,PITCHBEND_SPEED);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,LFO_TYPE);
  data.insert(data.end(),line);


  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,OSC1_DETUNE);
  data.insert(data.end(),line);



  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,LFO1_WAVEFORM);
  data.insert(data.end(),line);

  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,LFO2_WAVEFORM);
  data.insert(data.end(),line);

  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,LFO1_ENV_AMOUNT);
  data.insert(data.end(),line);

  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,LFO2_ENV_AMOUNT);
  data.insert(data.end(),line);

  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,LFO2_DEPTH);
  data.insert(data.end(),line);

  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,LFO2_FREQ);
  data.insert(data.end(),line);

  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,VELOCITY);
  data.insert(data.end(),line);

  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,OSC1_MOD);
  data.insert(data.end(),line);

  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,OSC1_UNISON);
  data.insert(data.end(),line);

  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,OSC2_UNISON);
  data.insert(data.end(),line);

  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,OSC1_UNISONDT);
  data.insert(data.end(),line);

  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,OSC2_UNISONDT);
  data.insert(data.end(),line);

  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,FILTER1_SATURATION);
  data.insert(data.end(),line);

  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,FILTER1_FEEDBACK);
  data.insert(data.end(),line);

  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,OSC3_AMP);
  data.insert(data.end(),line);

  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,OSC4_AMP);
  data.insert(data.end(),line);

  this->writePatternDataLine(PatternNumber,TrackNumber,P,line,OSC3_TYPE);
  data.insert(data.end(),line);



  sprintf(line,"\n");
  data.insert(data.end(),line);

  sprintf(line,"\n");
  data.insert(data.end(),line);

  //fd=fopen(fn.c_str(),"w");
  fd=fopen(filename,"w");
  for (int i=0; i< data.size();i++)
    {
      fprintf(fd,"%s",data[i].c_str());
    }
  fclose(fd);

  //loadedData[PatternNumber][TrackNumber]=DATA_EXIST_ON_STORAGE;
  //twoDPVector[PatternNumber][TrackNumber]=P;

  //if (retcode==true)
  //{
  loadedData[PatternNumber][TrackNumber]=DATA_LOADED_FROM_STORAGE;
  twoDPVector[PatternNumber][TrackNumber]=P;
      ///}
}


/*
void PatternReader::setFileName(string filename)
{
  fn=filename;
}
*/


const char * PatternReader::getParameterCharStar(int param)
{
  static const char * ret="NULL";

  static const char * adsr_env0_attack="Attack";         
  static const char * adsr_env0_decay="Decay";          
  static const char * adsr_env0_sustain="Sustain";        
  static const char * adsr_env0_release="Release";        
  
  static const char * osc1_type="OscOneType";     
  static const char * osc2_type="OscTwoType";     

  static const char * osc3_type="OscThreeType";     
  
  static const char * filter1_cutoff="Cutoff";         
  static const char * filter1_resonance="Resonance";      

  static const char * filter1_saturation="Filter1Saturation";         
  static const char * filter1_feedback="Filter1Feedback";      

  
  static const char * osc1_phase="PhaseOsc1";      
  
  static const char * adsr_env1_attack="AttackFltr";     
  static const char * adsr_env1_decay="DecayFltr";      
  static const char * adsr_env1_sustain="SustainFltr";    
  static const char * adsr_env1_release="ReleaseFltr";    
  
  static const char * note_adsr="NoteADSR";       
  
  static const char * machine_type="MachineType";    
  static const char * filter1_algo="FilterAlgo";     
  static const char * filter1_type="FilterType";     
  
  static const char * osc1_amp="Osc1Amp";        
  static const char * osc2_amp="Osc2Amp";        

  static const char * osc3_amp="Osc3Amp";        
  static const char * osc4_amp="Osc4Amp";        
  
  static const char * fx1_depth="FxDepth";        
  static const char * fx1_speed="FxSpeed";        
  
  static const char * fm_type="FmType";         
  
  static const char * trig_time_duration="TrigTime";       
  
  static const char * pitchbend_depth="PitchBendDepth"; 
  static const char * pitchbend_speed="PitchBendSpeed"; 
  
  static const char * lfo_type="LfoType";

  static const char * lfo1_depth="LFO1Depth";
  static const char * lfo1_freq="LFO1Speed";

  static const char * lfo2_depth="LFO2Depth";
  static const char * lfo2_freq="LFO2Speed";


  static const char * amp="Amp";

  static const char * note="Note";
    
  static const char * trig="Trig";

  static const char * vcomix="VCOMix";

  static const char * osc1_detune="OSC1Detune";

  static const char * lfo1_waveform="LFO1Waveform";
  static const char * lfo2_waveform="LFO2Waveform";

  static const char * lfo1_env_amount="LFO1EnvAmount";
  static const char * lfo2_env_amount="LFO2EnvAmount";

  static const char * velocity="Velocity";

  static const char * osc1_mod="OSC1Mod";

  static const char * osc1_unison="Osc1Unison";        
  static const char * osc2_unison="Osc2Unison";        

  static const char * osc1_unisondt="Osc1UnisonDetune";        
  static const char * osc2_unisondt="Osc2UnisonDetune";        


  switch (param)
    {
    case ADSR_ENV0_ATTACK:   return adsr_env0_attack;         break;
    case ADSR_ENV0_DECAY:    return adsr_env0_decay;          break;
    case ADSR_ENV0_SUSTAIN:  return adsr_env0_sustain;        break;
    case ADSR_ENV0_RELEASE:  return adsr_env0_release;        break;

    case OSC1_TYPE:          return osc1_type;                break;
    case OSC2_TYPE:          return osc2_type;                break;

    case OSC3_TYPE:          return osc3_type;                break;
      
    case FILTER1_CUTOFF:     return filter1_cutoff;           break;
    case FILTER1_RESONANCE:  return filter1_resonance;        break;

    case FILTER1_SATURATION: return filter1_saturation;       break;
    case FILTER1_FEEDBACK:   return filter1_feedback;        break;
    
    case OSC1_PHASE:         return osc1_phase;               break;
    
    case ADSR_ENV1_ATTACK:   return adsr_env1_attack;         break;
    case ADSR_ENV1_DECAY:    return adsr_env1_decay;          break;
    case ADSR_ENV1_SUSTAIN:  return adsr_env1_sustain;        break;
    case ADSR_ENV1_RELEASE:  return adsr_env1_release;        break;

    case NOTE_ADSR:          return note_adsr;                break;

    case MACHINE_TYPE:       return machine_type;             break;
    case FILTER1_ALGO:       return filter1_algo;             break;
    case FILTER1_TYPE:       return filter1_type;             break;

    case OSC1_AMP:           return osc1_amp;                 break;
    case OSC2_AMP:           return osc2_amp;                 break;

    case OSC3_AMP:           return osc3_amp;                 break;
    case OSC4_AMP:           return osc4_amp;                 break;


    case OSC1_UNISON:        return osc1_unison;              break;
    case OSC2_UNISON:        return osc2_unison;              break;

    case OSC1_UNISONDT:      return osc1_unisondt;            break;
    case OSC2_UNISONDT:      return osc2_unisondt;            break;


    case FX1_DEPTH:          return fx1_depth;                break;
    case FX1_SPEED:          return fx1_speed;                break;

    case FM_TYPE:            return fm_type;                  break;

    case TRIG_TIME_DURATION: return trig_time_duration;       break;

    case PITCHBEND_DEPTH:    return pitchbend_depth;          break;
    case PITCHBEND_SPEED:    return pitchbend_speed;          break;

    case LFO_TYPE:           return lfo_type;                 break;

    case LFO1_DEPTH:         return lfo1_depth;               break;
    case LFO1_FREQ:          return lfo1_freq;                break;

    case LFO2_DEPTH:         return lfo2_depth;               break;
    case LFO2_FREQ:          return lfo2_freq;                break;

    case AMP:                return amp;                      break;

    case NOTE:               return note;                     break;

    case NOTE_ON:            return trig;                     break;

    case VCO_MIX:            return vcomix;                   break;

    case OSC1_DETUNE:        return osc1_detune;              break;

    case LFO1_WAVEFORM:      return lfo1_waveform;            break;
    case LFO2_WAVEFORM:      return lfo2_waveform;            break;

    case LFO1_ENV_AMOUNT:    return lfo1_env_amount;          break;
    case LFO2_ENV_AMOUNT:    return lfo2_env_amount;          break;

    case VELOCITY:           return velocity;                 break;

    case OSC1_MOD:           return osc1_mod;                 break;
    
    default:                 printf("PatternReader::getParameterCharStar(%d) not found => exit\n"); exit(1); break;
    }
  return ret;
}
