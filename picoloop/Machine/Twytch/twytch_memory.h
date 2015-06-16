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
#ifndef TWYTCH_MEMORY_H
#define MEMORY_H

#include "twytch_common.h"

#include <algorithm>
#include <cmath>

namespace mopotwytchsynth {

  // A processor utility to store a stream of data for later lookup.
  class Memory {
    public:
      Memory(int size);
      Memory(const Memory& other);
      ~Memory();

      void push(mopo_float sample) {
        offset_ = (offset_ + 1) & bitmask_;
        memory_[offset_] = sample;
      }

      mopo_float getIndex(int index) const {
        return memory_[(offset_ - index) & bitmask_];
      }

      mopo_float get(mopo_float past) const {
        mopo_float float_index;
        mopo_float sample_fraction = modf(past, &float_index);
        int index = std::max<int>(float_index, 1);

        // TODO(mtytel): Quadratic or all-pass interpolation is better.
        mopo_float from = getIndex(index - 1);
        mopo_float to = getIndex(index);
        return INTERPOLATE(from, to, sample_fraction);
      }

      const mopo_float* getPointer(int past) const {
        return memory_ + ((offset_ - past) & bitmask_);
      }

      const mopo_float* getBuffer() const {
        return memory_;
      }

      int getOffset() const {
        return offset_;
      }

      int getSize() const {
        return size_;
      }

    protected:
      mopo_float* memory_;
      unsigned int size_;
      unsigned int bitmask_;
      unsigned int offset_;
  };
} // namespace mopo

#endif // MEMORY_H
