/* Copyright 2013-2017 Matt Tytel
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
#ifndef TWITCHHELM_FIXED_POINT_OSCILLATOR_H
#define TWITCHHELM_FIXED_POINT_OSCILLATOR_H

#include "twytchhelm_mopo.h"
#include "twytchhelm_fixed_point_wave.h"

namespace twytchhelmmopo {

  class FixedPointOscillator : public Processor {
    public:
    
      enum Inputs {
        kWaveform,
        kPhaseInc,
        kReset,
        kShuffle,
        kAmplitude,
        kLowOctave,
        kNumInputs
      };

      FixedPointOscillator();

      virtual void process();
      virtual Processor* clone() const { return new FixedPointOscillator(*this); }

    protected:
      unsigned int phase_;
  };
} // namespace twytchhelmmopo

#endif // FIXED_POINT_OSCILLATOR_H
