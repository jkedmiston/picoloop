#include "AudioEngine.h"




PulseSync::PulseSync()
{
  nb_tick=0;
  nb_tick_before_step_change=0;
  nb_tick_before_step_change_div_six=0;
  tick_length_high=4;
  tick_length_low=tick_length_high*2;
  tick_height_std=0;
  tick_height_high=20000;
  //tick_height_low=-8000;
  tick_height_low=-20000;
}

int PulseSync::setNbTickBeforeStepChange(int val)
{
  nb_tick_before_step_change=val;
  nb_tick_before_step_change_div_six=val;
}

int PulseSync::tick()
{
  Sint16 out=tick_height_std;
  nb_tick++;
  if (nb_tick<tick_length_high)
    {
      if (nb_tick<tick_length_low)
	out=tick_height_high;
      // else
      // 	out=tick_height_low;
    }
    else
      out=tick_height_std;

    if (nb_tick>nb_tick_before_step_change_div_six)
      nb_tick=0;

  return out;
}


//#include "SineOscillator.h"

void sdlcallback(void *unused, Uint8 *stream, int len);
int   rtcallback( 
	       void *outputBuffer, 
	       void *inputBuffer, 
	       unsigned int nBufferFrames,
	       double streamTime, 
	       RtAudioStreamStatus status, 
	       void *user_data );

//AudioEngine::AudioEngine() : inst(), AM()
AudioEngine::AudioEngine() : AM(),
			     AD(),
			     buffer_out_left(  new Sint16[INTERNAL_BUFFER_SIZE]),
			     buffer_out_right( new Sint16[INTERNAL_BUFFER_SIZE])

			     //buffer_out( new Sint16[BUFFER_FRAME])
{
  freq=DEFAULTFREQ;
  samples=DEFAULTSAMPLES;
  channels=DEFAULTCHANNELS;
  polyphony=DEFAULTPOLYPHONY;
  tick_left=0;
  tick_right=0;

  FORMAT=RTAUDIO_SINT16;
  bufferFrames = 512;          // Weird ?
  bufferFrames = BUFFER_FRAME; // Weird ?
  nbCallback=0;

  // Debug audio allow to dump audio to audioout file which is a raw file
  #ifndef DUMP_AUDIO
  dump_audio=0;
  #else
  dump_audio=DUMP_AUDIO;
  #endif


  if (dump_audio) 
    {
      fd = fopen("audioout","w+");
    }
  //buffer_out=(Sint16*)malloc(sizeof(Sint16)*BUFFER_FRAME);
  bufferGenerated=0;

  nb_tick=0;
  nb_tick_before_step_change=0;
  seqCallback=0;

  #ifdef __SDL_AUDIO__
  AD.sdlAudioSpecWanted->callback=sdlcallback;
  AD.sdlAudioSpecWanted->userdata=this;
  #endif

  #ifdef __RTAUDIO__
  AD.internal_callback=rtcallback;
  AD.userdata=this;
  #endif
}



int AudioEngine::getNbCallback()
{
  return nbCallback;
}

// related to  sequencer callbackw
int AudioEngine::setNbTickBeforeStepChange(int val) 
{
  nb_tick_before_step_change=val;
  PS.setNbTickBeforeStepChange(val);
}


// a function which need to be trigged when "nb_tick_before_step_change" sample are generated
void AudioEngine::setupSequencerCallback(void (*ptrfunc)(void)) 
{
  seqCallback=ptrfunc;
}

// process BUFFER_FRAME sample and call the callback when needed
void AudioEngine::processBuffer(int len)
{
  //for (int i=0;i<BUFFER_FRAME;i++)
  for (int i=0;i<len;i++)
    {
      nb_tick++;
      if (nb_tick<nb_tick_before_step_change)
	{
	  //buffer_out_left[i]=AM.tick();
	  //buffer_out_right[i]=buffer_out_left[i];
	  //buffer_out_right[i]=PS.tick();
	  buffer_out_left[i]=PS.tick();
	  buffer_out_right[i]=buffer_out_left[i];

	}
      else
        {
          //printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!CALLL\n");                                                                                                                      
          //PP->seq_callback_update_step();
	  if (seqCallback)
	    (*seqCallback)();
          nb_tick=0;
	  buffer_out_left[i]=PS.tick();
	  buffer_out_right[i]=buffer_out_left[i];
	  //buffer_out_left[i]=AM.tick();
	  //buffer_out_right[i]=buffer_out_left[i];
	  //buffer_out_right[i]=PS.tick();
        }
    }
  bufferGenerated=0;
}


int AudioEngine::bufferIsGenerated()
{
  return bufferGenerated;
}

Sint16 * AudioEngine::getBufferOutLeft()
{
  return buffer_out_left;
}

Sint16 * AudioEngine::getBufferOutRight()
{
  return buffer_out_right;
}



/*
void AudioEngine::setSynthFreq(int sfreq)
{
  S.setFreq(sfreq);
}
*/

AudioMixer & AudioEngine::getAudioMixer()
{
  return AM;
}

void AudioEngine::setDefault()
{
  freq=DEFAULTFREQ;
  samples=DEFAULTSAMPLES;
  channels=DEFAULTCHANNELS;
  polyphony=DEFAULTPOLYPHONY;
}

int AudioEngine::getTickLeft()
{
  return tick_left;
}

int AudioEngine::getTickRight()
{
  return tick_right;
}





//void AudioEngine::sdlcallback(void *unused, Uint8 *stream, int len)
void AudioEngine::callback(void *unused, Uint8 *stream, int len)
{
  //  printf("AudioEngine::calback() begin nBufferFrame=%d nbCallback=%d\n",nBufferFrames,nbCallback);
  //printf("AudioEngine::callback() len=%d\n",len);
  int     buffer_size;
  //int     buffer_size=len;

  //Workaround a linux sdl 1.2 audio bug 
  //                   sdl seem to have a bug on this...
  #ifdef __SDL_AUDIO__
  //buffer_size=BUFFER_FRAME;
  buffer_size=AD.getBufferFrame();
  //buffer_size=len;
  #ifdef PSP
  //buffer_size=len;
  //buffer_size=64;
  buffer_size=AD.getBufferFrame();
  #endif
  #endif

  //Should be the "Normal case" because we use ...,int len) provided by the caller
  #ifdef __RTAUDIO__
  buffer_size=len;
  #endif

  typedef Sint16 MY_TYPE;
  MY_TYPE *buffer = (MY_TYPE *) stream;
  
  //  this->bufferGenerated=0;
  if (bufferGenerated==0)
    //this->processBuffer(BUFFER_FRAME);
    this->processBuffer(buffer_size);

  #ifdef DUMP_AUDIO
  if (dump_audio)
    fwrite(buffer_out_right,buffer_size,sizeof(Sint16),fd);
  #endif

    
    //for (int i=0;i<len;i++)
    for (int i=0;i<buffer_size;i++)
    {
      //int tick = S.tick();
      //      int tick = AM.tick();
      //Sint16 tick = AM.tick();
      tick_left=buffer_out_left[i];
      tick_right=buffer_out_right[i];
      //      printf("%d\n",tick);
      /*
      #ifdef LINUX_DESKTOP
      if (debug_audio)
	{	  
	  fwrite(&tick,1,sizeof(Sint16),fd);
	  fwrite(&tick,1,sizeof(Sint16),fd);
	  //printf("%d\t",tick);
	}
      #endif
      */
      buffer[(2*i)]=    tick_left;
      buffer[(2*i)+1]=  tick_right;

      //buffer[i+1]=tick;
      //buffer[i+1]=0;
      i++;
    }
  //if (debug_audio)
  //fwrite(buffer,sizeof(MY_TYPE)*nBufferFrames,sizeof(MY_TYPE),fd);

  //  printf("AudioEngine::calback() end\n");
  //bufferGenerated=0;
  //return nbCallback++;  
}

void sdlcallback(void *unused, Uint8 *stream, int len)
{
  ((AudioEngine*)unused)->callback(unused,stream,len);
}


int rtcallback( 
	       void *outputBuffer, 
	       void *inputBuffer, 
	       unsigned int nBufferFrames,
	       double streamTime, 
	       RtAudioStreamStatus status, 
	       void *user_data )
{
  //printf("callback\n");
  ((AudioEngine*)user_data)->callback(user_data,
				      (Uint8*)outputBuffer,
				      nBufferFrames);

  return 0;
}



 int AudioEngine::startAudio()
 {
  //return 1;
  //SDL_PauseAudio(0);
   AD.startAudio();
}

// enable audio callback
//int AudioEngine::startAudio()
 //{
  //dac.startStream();
 //}

// disable audio callback
int AudioEngine::stopAudio()
{
  //  dac.stopStream();

}


// disable audio callback
int AudioEngine::stopAudioSdl()
{
  //SDL_CloseAudio();
  //return 1;
  //SDL_PauseAudio(1);
  AD.stopAudio();
}

int AudioEngine::openAudio()
{
  AD.openAudio();
}

int AudioEngine::closeAudio()
{
  //dac.closeStream();  
  AD.closeAudio();
}

int AudioEngine::closeAudioSdl()
{
  //SDL_LockAudio();
  //SDL_CloseAudio();
  //SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
