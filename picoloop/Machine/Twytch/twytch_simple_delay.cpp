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

#include "twytch_simple_delay.h"

namespace mopotwytchsynth {

  SimpleDelay::SimpleDelay(int size) : Processor(SimpleDelay::kNumInputs, 1) {
    memory_ = new Memory(size);
  }

  SimpleDelay::SimpleDelay(const SimpleDelay& other) : Processor(other) {
    this->memory_ = new Memory(*other.memory_);
  }

  void SimpleDelay::process() {
    for (int i = 0; i < buffer_size_; ++i)
      tick(i);
  }
} // namespace mopo
