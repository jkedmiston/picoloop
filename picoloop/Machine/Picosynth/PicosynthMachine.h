using namespace std;

#include "Machine.h"
#include "PicosynthVCO.h"
#include "PicosynthADSR.h"
#include "Filter.h"
#include "OneOscillator.h"

#ifndef __PICOSYNTHMACHINE____
#define __PICOSYNTHMACHINE____

class PicosynthMachine : public Machine
{
 public:
  PicosynthMachine();
  ~PicosynthMachine();

  void init();
  void reset();
  int  tick();

  void setI(int what,int val);
  void setF(int what,float val);
  int  getI(int what);

 protected:
  
  PicosynthADSR   & getADSRAmp();
  PicosynthADSR   & getADSRFltr();
  PicosynthVCO    & getVCO();

  int                   cutoff;
  int                   resonance;

  PicosynthVCO          vco;
  PicosynthADSR         adsr_amp;
  PicosynthADSR         adsr_fltr;
  Filter                filter;

  OneOscillator         one_osc;

  int                   sample_num;
  Sint16                last_sample;

  Sint16              * tanh_table;

  int                   note;
  int                   detune;
};

#endif
  
