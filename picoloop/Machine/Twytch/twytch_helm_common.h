/* Copyright 2013-2015 Matt Tytel
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
#ifndef TWYTCH_HELM_COMMON_H
#define HELM_COMMON_H

#include "twytch_mopo.h"
#include "twytch_value.h"
#include "twytch_operators.h"

#include <map>
#include <string>

namespace mopotwytchsynth {

  struct ValueDetails {
    enum DisplaySkew {
      kLinear,
      kQuadratic,
      kExponential
    };

    std::string name;
    mopo_float min;
    mopo_float max;
    int steps;
    mopo_float default_value;

    mopo_float display_multiply;
    DisplaySkew display_skew;
    std::string display_units;
  } typedef ValueDetails;

  namespace strings {

    const std::string off_on[] = {
      "off",
      "on"
    };

    const std::string off_auto_on[] = {
      "off",
      "auto",
      "on"
    };

    const std::string arp_patterns[] = {
      "up",
      "down",
      "up-down",
      "as played",
      "random"
    };

    const std::string freq_sync_styles[] = {
      "Hertz",
      "Tempo",
      "Tempo Dotted",
      "Tempo Triplets"
    };

    const std::string filter_types[] = {
      "low pass",
      "high pass",
      "band pass",
      "low shelf",
      "high shelf",
      "band shelf",
      "all pass"
    };

    const std::string waveforms[] = {
      "sin",
      "triangle",
      "square",
      "saw up",
      "saw down",
      "3 step",
      "4 step",
      "8 step",
      "3 pyramid",
      "5 pyramid",
      "9 pyramid",
      "white noise"
    };

    const std::string synced_frequencies[] = {
      "32/1",
      "16/1",
      "8/1",
      "4/1",
      "2/1",
      "1/1",
      "1/2",
      "1/4",
      "1/8",
      "1/16",
      "1/32",
      "1/64",
    };
  } // namespace strings

  const mopo_float MAX_STEPS = 32;
  const int NUM_FORMANTS = 4;

  typedef std::map<std::string, Value*> control_map;
  typedef std::map<char, std::string> midi_learn_map;
  typedef std::map<std::string, Processor*> input_map;
  typedef std::map<std::string, Processor::Output*> output_map;

  const mopotwytchsynth::Value synced_freq_ratios[] = {
    Value(1.0 / 128.0),
    Value(1.0 / 64.0),
    Value(1.0 / 32.0),
    Value(1.0 / 16.0),
    Value(1.0 / 8.0),
    Value(1.0 / 4.0),
    Value(1.0 / 2.0),
    Value(1.0),
    Value(2.0),
    Value(4.0),
    Value(8.0),
    Value(16.0),
  };

  struct ModulationConnection {
    ModulationConnection(std::string from, std::string to) :
        source(from), destination(to) { }

    std::string source;
    std::string destination;
    Value amount;
    Multiply modulation_scale;
  };

  class ValueDetailsLookup {
    public:
      ValueDetailsLookup();
      const ValueDetails& getDetails(std::string name) {
        TWYTCH_MOPO_ASSERT(details_lookup_.count(name));
        return details_lookup_[name];
      }

    private:
      static const ValueDetails parameter_list[];
      std::map<std::string, ValueDetails> details_lookup_;
  };

  class Parameters {
    public:
      static const ValueDetails& getDetails(std::string name) { return lookup_.getDetails(name); }
    private:
      static ValueDetailsLookup lookup_;
  };
} // namespace mopo

#endif // HELM_COMMON_H
