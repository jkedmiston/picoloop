/* Copyright 2013-2015 Matt Tytel
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
#ifndef TWYTCH_STUTTER_H
#define STUTTER_H

#include "twytch_memory.h"
#include "twytch_processor.h"

namespace mopotwytchsynth {

  // A signal delay processor with wet/dry, delay time and feedback controls.
  // Handles fractional delay amounts through interpolation.
  class Stutter : public Processor {
    public:
      enum Inputs {
        kAudio,
        kStutterFrequency,
        kResampleFrequency,
        kReset,
        kNumInputs
      };

      Stutter(int size);
      Stutter(const Stutter& other);
      virtual ~Stutter();

      virtual Processor* clone() const { return new Stutter(*this); }
      virtual void process();

      void tick(int i) {
        offset_ -= 1.0;
        resample_offset_ -= 1.0;
        if (resample_offset_ <= 0.0) {
          resampling_ = true;
          resample_offset_ += sample_rate_ / input(kResampleFrequency)->at(i);
          offset_ = sample_rate_ / input(kStutterFrequency)->at(i);
        }
        else if (offset_ <= 0.0) {
          resampling_ = false;
          offset_ += sample_rate_ / input(kStutterFrequency)->at(i);
        }

        if (resampling_) {
          mopo_float audio = input(kAudio)->at(i);
          memory_->push(audio);
          output(0)->buffer[i] = audio;
        }
        else {
          output(0)->buffer[i] = memory_->get(offset_);
        }
      }

    protected:
      Memory* memory_;
      mopo_float offset_;
      mopo_float resample_offset_;
      bool resampling_;
  };
} // namespace mopo

#endif // STUTTER_H
