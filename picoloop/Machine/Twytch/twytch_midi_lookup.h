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
#ifndef TWYTCH_MIDI_LOOKUP_H
#define MIDI_LOOKUP_H

#include "twytch_common.h"
#include "twytch_utils.h"

#include <cmath>

namespace mopotwytchsynth {

  class MidiLookupSingleton {
    public:
      MidiLookupSingleton() {
        for (int i = 0; i < TWYTCH_MAX_CENTS + 2; ++i) {
          frequency_lookup_[i] = utils::midiCentsToFrequency(i);
        }
      }

      mopo_float centsLookup(mopo_float cents_from_0) const {
        mopo_float clamped_cents = CLAMP(cents_from_0, 0.0, TWYTCH_MAX_CENTS);
        int full_cents = clamped_cents;
        mopo_float fraction_cents = clamped_cents - full_cents;

        return INTERPOLATE(frequency_lookup_[full_cents],
                           frequency_lookup_[full_cents + 1], fraction_cents);
      }

    private:
      mopo_float frequency_lookup_[TWYTCH_MAX_CENTS + 2];
  };

  class MidiLookup {
    public:
      static mopo_float centsLookup(mopo_float cents_from_0) {
        return lookup_.centsLookup(cents_from_0);
      }

    private:
      static const MidiLookupSingleton lookup_;
  };
} // namespace mopo

#endif // MIDI_LOOKUP_H
