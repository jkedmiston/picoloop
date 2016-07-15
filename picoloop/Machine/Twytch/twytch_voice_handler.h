/* Copyright 2013-2016 Matt Tytel
 *
 * mopo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * mopo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with mopo.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#ifndef TWYTCH_VOICE_HANDLER_H
#define TWYTCH_VOICE_HANDLER_H

#include "twytch_note_handler.h"
#include "twytch_processor_router.h"
#include "twytch_value.h"

#include <map>
#include <list>

namespace mopotwytchsynth {

  struct VoiceState {
    VoiceEvent event;
    mopo_float note;
    mopo_float last_note;
    mopo_float velocity;
    int note_pressed;
    int channel;
  };

  class Voice {
    public:
      enum KeyState {
        kHeld,
        kSustained,
        kReleased,
        kNumStates
      };

      Voice(Processor* voice);

      Processor* processor() { return processor_; }
      const VoiceState& state() { return state_; }
      const KeyState key_state() { return key_state_; }
      int event_sample() { return event_sample_; }

      mopo_float aftertouch() { return aftertouch_; }
      mopo_float aftertouch_sample() { return aftertouch_sample_; }

      void activate(mopo_float note, mopo_float velocity,
                    mopo_float last_note, int note_pressed = 0,
                    int sample = 0, int channel = 0) {
        event_sample_ = sample;
        state_.event = kVoiceOn;
        state_.note = note;
        state_.velocity = velocity;
        state_.last_note = last_note;
        state_.note_pressed = note_pressed;
        state_.channel = channel;
        aftertouch_ = velocity;
        aftertouch_sample_ = sample;
        key_state_ = kHeld;
      }

      void sustain() {
        key_state_ = kSustained;
      }

      void deactivate(int sample = 0) {
        event_sample_ = sample;
        state_.event = kVoiceOff;
        key_state_ = kReleased;
      }

      void kill(int sample = 0) {
        event_sample_ = sample;
        state_.event = kVoiceKill;
      }

      bool hasNewEvent() {
        return event_sample_ >= 0;
      }

      void setAftertouch(mopo_float aftertouch, int sample = 0) {
        aftertouch_ = aftertouch;
        aftertouch_sample_ = sample;
      }

      bool hasNewAftertouch() {
        return aftertouch_sample_ >= 0;
      }

      void clearEvents() {
        event_sample_ = -1;
        aftertouch_sample_ = -1;
      }

    private:
      Voice() { }

      int event_sample_;
      VoiceState state_;
      KeyState key_state_;

      int aftertouch_sample_;
      mopo_float aftertouch_;

      Processor* processor_;
  };

  class VoiceHandler : public virtual ProcessorRouter, public NoteHandler {
    public:
      enum Inputs {
        kPolyphony,
        kNumInputs
      };

      VoiceHandler(size_t polyphony = 1);

      virtual Processor* clone() const override {
        TWYTCH_MOPO_ASSERT(false);
        return 0;
      }

      virtual void process() override;
      virtual void setSampleRate(int sample_rate) override;
      virtual void setBufferSize(int buffer_size) override;
      int getNumActiveVoices();
      std::list<mopo_float> getPressedNotes() { return pressed_notes_; }
      bool isNotePlaying(mopo_float note);

      void allNotesOff(int sample = 0) override;
      virtual void noteOn(mopo_float note, mopo_float velocity = 1,
                          int sample = 0, int channel = 0) override;
      virtual VoiceEvent noteOff(mopo_float note, int sample = 0) override;
      void setAftertouch(mopo_float note, mopo_float aftertouch, int sample = 0);
      void sustainOn();
      void sustainOff(int sample = 0);

      Output* voice_event() { return &voice_event_; }
      Output* note() { return &note_; }
      Output* last_note() { return &last_note_; }
      Output* note_pressed() { return &note_pressed_; }
      Output* channel() { return &channel_; }
      Output* velocity() { return &velocity_; }
      Output* aftertouch() { return &aftertouch_; }
      size_t polyphony() { return polyphony_; }
    
      mopo_float getLastActiveNote() const;

      virtual ProcessorRouter* getMonoRouter() override { return &global_router_; }
      virtual ProcessorRouter* getPolyRouter() override { return &voice_router_; }

      void addProcessor(Processor* processor) override;
      void removeProcessor(const Processor* processor) override;
      void addGlobalProcessor(Processor* processor);
      void removeGlobalProcessor(Processor* processor);
      Output* registerOutput(Output* output) override;
      Output* registerOutput(Output* output, int index) override;

      void setPolyphony(size_t polyphony);

      void setVoiceKiller(const Output* killer) {
        voice_killer_ = killer;
      }

      void setLegato(bool legato) {
        legato_ = legato;
      }

      void setVoiceKiller(const Processor* killer) {
        setVoiceKiller(killer->output());
      }

      bool isPolyphonic(const Processor* processor) const override;

    private:
      VoiceHandler() { }

      Voice* grabVoice();
      Voice* getVoiceToKill();
      Voice* createVoice();
      void prepareVoiceTriggers(Voice* voice);
      void processVoice(Voice* voice);
      void clearAccumulatedOutputs();
      void clearNonaccumulatedOutputs();
      void accumulateOutputs();
      void writeNonaccumulatedOutputs();
      bool shouldAccumulate(Output* output);

      size_t polyphony_;
      bool sustain_;
      bool legato_;
      std::vector<Output*> voice_outputs_;
      const Output* voice_killer_;
      mopo_float last_played_note_;
      Output voice_event_;
      Output note_;
      Output last_note_;
      Output note_pressed_;
      Output channel_;
      Output velocity_;
      Output aftertouch_;

      std::list<mopo_float> pressed_notes_;
      std::vector<Voice*> all_voices_;

      std::list<Voice*> free_voices_;
      std::list<Voice*> active_voices_;

      ProcessorRouter voice_router_;
      ProcessorRouter global_router_;
  };
} // namespace mopo

#endif // VOICE_HANDLER_H
