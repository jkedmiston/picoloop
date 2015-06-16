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

#include "twytch_stutter.h"

namespace mopotwytchsynth {

  Stutter::Stutter(int size) : Processor(Stutter::kNumInputs, 1),
      offset_(0.0), resample_offset_(0.0), resampling_(true) {
    memory_ = new Memory(size);
  }

  Stutter::~Stutter() {
    delete memory_;
  }

  Stutter::Stutter(const Stutter& other) : Processor(other) {
    this->memory_ = new Memory(*other.memory_);
    this->offset_ = other.offset_;
    this->resample_offset_ = other.resample_offset_;
    this->resampling_ = other.resampling_;
  }

  void Stutter::process() {
    int i = 0;
    if (input(kReset)->source->triggered) {
      int offset = input(kReset)->source->trigger_offset;
      for (; i < offset; ++i)
        tick(i);

      resampling_ = true;
      offset_ = 0.0;
      resample_offset_ = 0.0;
    }

    for (; i < buffer_size_; ++i)
      tick(i);
  }
} // namespace mopo
