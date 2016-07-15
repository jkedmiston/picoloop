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

#include "twytch_portamento_slope.h"

#include "twytch_utils.h"

#include <cmath>
#include <iostream>

namespace mopotwytchsynth {

  PortamentoSlope::PortamentoSlope() : Processor(kNumInputs, 1) {
    last_value_ = 0.0;
  }

  void PortamentoSlope::processBypass(int start) {
    mopo_float* dest = output(0)->buffer;
    const mopo_float* src = input(kTarget)->source->buffer;
    memcpy(dest + start, src, (buffer_size_ - start) * sizeof(mopo_float));
    last_value_ = dest[buffer_size_ - 1];
  }

  void PortamentoSlope::processTriggers() {
    output()->clearTrigger();

    if (input(kTriggerJump)->source->triggered) {
      int offset = input(kTriggerJump)->source->trigger_offset;
      output()->trigger(input(kTarget)->at(offset), offset);
    }
    else if (input(kTriggerStart)->source->triggered) {
      float value = input(kTriggerStart)->source->trigger_value;
      output()->trigger(value, input(kTriggerStart)->source->trigger_offset);
    }
  }

  void PortamentoSlope::process() {
    processTriggers();
    int state = static_cast<int>(input(kPortamentoType)->at(0));
    if (state == kPortamentoOff) {
      processBypass(0);
      return;
    }

    int i = 0;
    int note_number = static_cast<int>(input(kNoteNumber)->source->trigger_value);

    if (state == kPortamentoAuto && note_number <= 1 && input(kTriggerJump)->source->triggered) {
      int trigger_offset = input(kTriggerJump)->source->trigger_offset;
      for (; i < trigger_offset; ++i)
        tick(i);

      last_value_ = input(kTarget)->at(trigger_offset);
    }
    else if (input(kTriggerStart)->source->triggered) {
      int trigger_offset = input(kTriggerStart)->source->trigger_offset;
      for (; i < trigger_offset; ++i)
        tick(i);

      last_value_ = input(kTriggerStart)->source->trigger_value;
    }

    if (last_value_ == input(kTarget)->at(0) &&
        last_value_ == input(kTarget)->at(buffer_size_ - 1)) {
      processBypass(i);
    }
    else {
      for (; i < buffer_size_; ++i)
        tick(i);
    }
  }

  inline void PortamentoSlope::tick(int i) {
    mopo_float target = input(kTarget)->at(i);
    if (twytchutils::closeToZero(input(kRunSeconds)->at(i)))
      last_value_ = target;

    mopo_float increment = 0.4 / (sample_rate_ * input(kRunSeconds)->at(0));
    if (target <= last_value_)
      last_value_ = twytchutils::clamp(last_value_ - increment, target, last_value_);
    else
      last_value_ = twytchutils::clamp(last_value_ + increment, last_value_, target);

    mopo_float movement = target - last_value_;
    movement *= fabs(movement);
    last_value_ += 0.07 * movement / (sample_rate_ * input(kRunSeconds)->at(0));
    output()->buffer[i] = last_value_;
  }
} // namespace mopo
