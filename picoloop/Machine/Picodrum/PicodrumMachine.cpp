#include "PicodrumMachine.h"



PicodrumMachine::PicodrumMachine() : adsr_amp(), vco(), filter()
{
  float fi;
  int   i;

  DPRINTF("PicodrumMachine::PicodrumMachine()\n");  
  cutoff=125;
  resonance=10;
  note=0;
  detune=64;
}



PicodrumMachine::~PicodrumMachine()
{
  DPRINTF("PicodrumMachine::~PicodrumMachine()\n");
}


void PicodrumMachine::init()
{
  adsr_amp.init();

  adsr_amp.setInput(&vco); 

  filter.init();
  sample_num=0;


  this->reset();
  this->getADSRAmp().init();

  this->getPicodrumVCO().init();
  this->getPicodrumVCO().setNoteDetune(0,0);      
  //this->getPicodrumVCO().setSynthFreq(0);      
  //this->getADSRAmp().setNoteADSR(0);

  note=0;
  detune=64;

}

int PicodrumMachine::checkI(int what,int val)
{
  switch (what)
    {
    case OSC1_TYPE:
      if (val<0) return 0;
      if (val>PICO_WAVETABLE_SIZE-2) return PICO_WAVETABLE_SIZE-1;
      return val;
      break;

    case OSC2_TYPE:
      if (val<0) return 0;
      if (val>PICO_WAVETABLE_SIZE-2) return PICO_WAVETABLE_SIZE-1;
      return val;
      break;

    case FILTER1_TYPE:
      if (val<0) return 0;
      if (val>3) return 3;
      return val;
      break;

    case FILTER1_ALGO:
      if (val<0) return 0;
      if (val>2) return 2;
      return val;
      break;



    default:
      if (val<0)   return 0;
      if (val>127) return 127;
      DPRINTF("WARNING: PicodrumMachine::checkI(%d,%d)\n",what,val);
      return val;
      break;      
    }
}


int PicodrumMachine::getI(int what)
{

  if (what==NOTE_ON) return this->getADSRAmp().getNoteOn();
}

void PicodrumMachine::setF(int what,float val)
{
  //  if (what==OSC1_FREQ)           this->getPicodrumVCO().setSynthFreq(val);
  if (what==LFO1_FREQ)           this->getPicodrumVCO().setLfoSpeed(val);
}


void PicodrumMachine::setI(int what,int val)
{
  float f_val_cutoff;
  float f_val_resonance;

  if (what==NOTE_ON && val==1) 
    { 
      this->getPicodrumVCO().setNoteDetune(note,detune);
      this->getADSRAmp().reset();
      this->getPicodrumVCO().reset();
      this->getADSRAmp().setNoteOn(); 
    }
  if (what==NOTE_ON && val==0) 
    { 
      this->getADSRAmp().setNoteOff(); 
    }

  if (what==OSC1_NOTE)           note=val;

  //if (what==OSC1_FREQ)           this->getPicodrumVCO().setSynthFreq(val);
  if (what==OSC1_TYPE)           { this->getPicodrumVCO().setOscillator(0,val); osc1_type=val; }
  if (what==OSC2_TYPE)           { this->getPicodrumVCO().setOscillator(1,val); osc2_type=val; }

  if (what==OSC12_MIX)           this->getPicodrumVCO().setPicodrumVCOMix(val);
  if (what==OSC1_PHASE)          this->getPicodrumVCO().setPicodrumVCOPhase(val);

  if (what==LFO1_DEPTH)          this->getPicodrumVCO().setLfoDepth(val);
  //if (what==LFO1_FREQ)           this->getPicodrumVCO().setLfoSpeed(val);


  if (what==ADSR_ENV0_ATTACK)    this->getADSRAmp().setAttack(val);
  if (what==ADSR_ENV0_RELEASE)   this->getADSRAmp().setRelease(val);



  if (what==FILTER1_CUTOFF)      
    { 
      cutoff=val;
      filter.setCutoff(cutoff);
      this->getADSRAmp().reset();

  }
  if (what==FILTER1_RESONANCE)         
    { 
      resonance=val;
      filter.setResonance(resonance);
      this->getADSRAmp().reset();
    }

  if (what==FILTER1_TYPE)     filter.setFilterType(val);
  if (what==FILTER1_ALGO)     filter.setFilterAlgo(val);

  
}


PicodrumADSR & PicodrumMachine::getADSRAmp()
{
  return adsr_amp;
}




PicodrumVCO & PicodrumMachine::getPicodrumVCO()
{
  //  DPRINTF("PicodrumMachine::getPicodrumVCO() this=0x%08.8X\n",this);
  //  return vco_pointer;
  return vco;
}

//Biquad & PicodrumMachine::getBiquad()
//{
//  return bq;
//}

const char * PicodrumMachine::getMachineParamCharStar(int machineParam,int paramValue)
{
  static const char * str_null       = "NULL ";

  static const char * str_sine       = "SINE ";
  static const char * str_saw        = "SAW  ";
  static const char * str_pulse      = "PULSE";
  static const char * str_trgl       = "TRGL ";

  static const char * str_smsine     = "SSIN ";
  static const char * str_smsaw      = "SSAW ";
  static const char * str_smpulse    = "SPULS";
  static const char * str_smtrgl     = "STRGL";

  static const char * str_lfsr_noise = "LFSR ";
  static const char * str_noise      = "NOISE";

  const        char * str_osc[PICO_WAVETABLE_SIZE];


  static const char * str_fltr_algo_nofilter = "NOFL";
  static const char * str_fltr_algo_biquad   = "BIQU";
  static const char * str_fltr_algo_amsynth  = "AMST";
  
  const        char * str_fltr_algo[FILTER_ALGO_SIZE];


  static const char * str_fltr_type_lp   = "LP";
  static const char * str_fltr_type_bp   = "BP";
  static const char * str_fltr_type_hp   = "HP";

  const        char * str_fltr_type[FILTER_TYPE_SIZE];

  str_osc[PICO_WAVETABLE_SINE]        = str_sine;
  str_osc[PICO_WAVETABLE_SAW]         = str_saw;
  str_osc[PICO_WAVETABLE_PULSE]       = str_pulse;
  str_osc[PICO_WAVETABLE_TRGL]        = str_trgl;

  str_osc[PICO_WAVETABLE_SMSINE]      = str_smsine;
  str_osc[PICO_WAVETABLE_SMSAW]       = str_smsaw;
  str_osc[PICO_WAVETABLE_SMPULSE]     = str_smpulse;
  str_osc[PICO_WAVETABLE_SMTRGL]      = str_smtrgl;

  str_osc[PICO_WAVETABLE_NOISE]       = str_noise;
  str_osc[PICO_WAVETABLE_LFSRNOISE]   = str_lfsr_noise;

  str_fltr_algo[FILTER_ALGO_NOFILTER] = str_fltr_algo_nofilter;
  str_fltr_algo[FILTER_ALGO_BIQUAD]   = str_fltr_algo_biquad;
  str_fltr_algo[FILTER_ALGO_AMSYNTH]  = str_fltr_algo_amsynth;

  str_fltr_type[FILTER_TYPE_LP]       = str_fltr_type_lp;
  str_fltr_type[FILTER_TYPE_BP]       = str_fltr_type_bp;
  str_fltr_type[FILTER_TYPE_HP]       = str_fltr_type_hp;

  switch (machineParam)
    {
    case OSC1_TYPE:
      return str_osc[paramValue];
    case OSC2_TYPE:
      return str_osc[paramValue];
     
    case FILTER1_ALGO:
      return str_fltr_algo[paramValue];

    case FILTER1_TYPE:
      return str_fltr_type[paramValue];

    }
  return str_null;
}




void PicodrumMachine::reset()
{
  sample_num=0;
  last_sample=0;
}


int PicodrumMachine::tick()
{
  float  f_in;
  float  f_out;
  Sint16 s_in;
  Sint16 s_out;


  s_in=adsr_amp.tick();
  s_in=s_in;


  s_out=filter.process(s_in);
  return s_out;

}
