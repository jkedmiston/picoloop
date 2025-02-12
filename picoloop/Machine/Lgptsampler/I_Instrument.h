#ifndef _I_INSTRUMENT_H_
#define _I_INSTRUMENT_H_

#include "VariableContainer.h"
#include "Observable.h"
#include "lgptfixed.h"

#include "TablePlayback.h"

enum InstrumentType {
	IT_SAMPLE=0,
	IT_MIDI,
	IT_LAST
} ;

class I_Instrument:public VariableContainer, public Observable {
      
public:
	I_Instrument() {} ;
	virtual ~I_Instrument() {} ;

	  // Initialisation routine

	  virtual bool Init()=0 ;

	  // Start & stop the instument
      virtual bool Start(int channel,unsigned char note,bool retrigger=true)=0 ;
      virtual void Stop(int channel)=0 ;

	  // Engine playback  start callback

	  virtual void OnStart()=0 ;

      // size refers to the number of samples
      // should always fill interleaved stereo / 16bit
      
      virtual bool Render(int channel,lgptfixed *buffer,int size,bool updateTick)=0 ;

      virtual bool IsInitialized()=0 ;

	  virtual bool IsEmpty()=0 ;

	  virtual InstrumentType GetType()=0 ;

	  virtual const char *GetName()=0 ; 
	 
	  virtual void ProcessCommand(int channel,FourCC cc,ushort value)=0 ;

	  virtual void Purge()=0 ;

	  virtual int GetTable()=0 ;
	  virtual bool GetTableAutomation()=0 ;

	  virtual void GetTableState(TableSaveState &state)=0 ;	 
	  virtual void SetTableState(TableSaveState &state)=0 ;	 

};
#endif
