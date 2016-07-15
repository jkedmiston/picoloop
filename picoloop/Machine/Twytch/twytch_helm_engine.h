/* Copyright 2013-2016 Matt Tytel
 *
 * helm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * helm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with helm.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#ifndef TWYTCH_HELM_ENGINE_H
#define TWYTCH_HELM_ENGINE_H

#include "twytch_mopo.h"
#include "twytch_helm_common.h"
#include "twytch_helm_module.h"

namespace mopotwytchsynth {
  class Arpeggiator;
  class HelmVoiceHandler;
  class HelmLfo;
  class Value;

  // The overall helm engine. All audio processing is contained in here.
  class HelmEngine : public HelmModule, public NoteHandler {
    public:
      HelmEngine();

      void init() override;

      void process() override;

      std::set<ModulationConnection*> getModulationConnections() { return mod_connections_; }
      bool isModulationActive(ModulationConnection* connection);
      std::list<mopotwytchsynth::mopo_float> getPressedNotes();
      void connectModulation(ModulationConnection* connection);
      void disconnectModulation(ModulationConnection* connection);
      int getNumActiveVoices();
      mopo_float getLastActiveNote() const;

      // Keyboard events.
      void allNotesOff(int sample = 0) override;
      void noteOn(mopo_float note, mopo_float velocity = 1.0,
                  int sample = 0, int channel = 0) override;
      VoiceEvent noteOff(mopo_float note, int sample = 0) override;
      void setModWheel(mopo_float value, int channel = 0);
      void setPitchWheel(mopo_float value, int channel = 0);
      void setBpm(mopo_float bpm);
      void correctToTime(mopo_float samples) override;
      void setAftertouch(mopo_float note, mopo_float value, int sample = 0);

      // Sustain pedal events.
      void sustainOn();
      void sustainOff();

    private:
      HelmVoiceHandler* voice_handler_;
      Arpeggiator* arpeggiator_;
      Value* arp_on_;
      bool was_playing_arp_;

      Value* lfo_1_retrigger_;
      Value* lfo_2_retrigger_;
      Value* step_sequencer_retrigger_;
      HelmLfo* lfo_1_;
      HelmLfo* lfo_2_;
      StepGenerator* step_sequencer_;

      std::set<ModulationConnection*> mod_connections_;
  };
} // namespace mopo

#endif // HELM_ENGINE_H
