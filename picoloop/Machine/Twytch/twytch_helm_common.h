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
#ifndef TWYTCH_HELM_COMMON_H
#define TWYTCH_HELM_COMMON_H

#include "twytch_mopo.h"
#include "twytch_value.h"
#include "twytch_operators.h"

#include <map>
#include <string>

namespace mopotwytchsynth {

  class ModulationConnection;

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
    std::string display_name;
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


    const std::string freq_retrigger_styles[] = {
      "Free",
      "Retrigger",
      "Sync to Playhead"
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
      "sample and hold",
      "sample and glide",
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
  const int NUM_CHANNELS = 2;
  const int MEMORY_SAMPLE_RATE = 2205;
  const int MEMORY_RESOLUTION = 52;
  const mopo_float STUTTER_MAX_SAMPLES = 192000.0;
  const int DEFAULT_MODULATION_CONNECTIONS = 256;

  const int DEFAULT_KEYBOARD_OFFSET = 48;
  const std::wstring DEFAULT_KEYBOARD = L"awsedftgyhujkolp;'";
  const wchar_t DEFAULT_KEYBOARD_OCTAVE_UP = 'x';
  const wchar_t DEFAULT_KEYBOARD_OCTAVE_DOWN = 'z';

  const std::string PATCH_EXTENSION = "helm";

  typedef std::map<std::string, Value*> control_map;
  typedef std::pair<Value*, mopo_float> control_change;
  typedef std::pair<ModulationConnection*, mopo_float> modulation_change;
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
    ModulationConnection() : ModulationConnection("", "") { }

    ModulationConnection(std::string from, std::string to) :
        source(from), destination(to) {
      amount.setControlRate();
    }

    void resetConnection(const std::string& from, const std::string& to) {
      source = from;
      destination = to;
      modulation_scale.router(nullptr);
    }

    std::string source;
    std::string destination;
    Value amount;
    cr::Multiply modulation_scale;
  };

  class ModulationConnectionBank {
    public:
      ModulationConnectionBank();
      ~ModulationConnectionBank();
      ModulationConnection* get(const std::string& from, const std::string& to);
      void recycle(ModulationConnection* connection);

      static ModulationConnectionBank* instance();

    private:
      void allocateMoreConnections();
      std::list<ModulationConnection*> available_connections_;
      std::vector<ModulationConnection*> all_connections_;
  };

  class StringLayout {
    public:
      std::wstring getLayout() { return layout_; }
      void setLayout(std::wstring layout) { layout_ = layout; }

      wchar_t getUpKey() { return up_key_; }
      void setUpKey(wchar_t up_key) { up_key_ = up_key; }

      wchar_t getDownKey() { return down_key_; }
      void setDownKey(wchar_t down_key) { down_key_ = down_key; }

    protected:
      std::wstring layout_;
      int up_key_;
      int down_key_;
  };

  class ValueDetailsLookup {
    public:
      ValueDetailsLookup();
      const bool isParameter(const std::string& name) const {
        return details_lookup_.count(name);
      }

      const ValueDetails& getDetails(const std::string& name) const {
        auto details = details_lookup_.find(name);
        TWYTCH_MOPO_ASSERT(details != details_lookup_.end());
        return details->second;
      }

    private:
      static const ValueDetails parameter_list[];
      std::map<std::string, ValueDetails> details_lookup_;
  };

  class Parameters {
    public:
      static const ValueDetails& getDetails(const std::string& name) {
        return lookup_.getDetails(name);
      }

      static const bool isParameter(const std::string& name) {
        return lookup_.isParameter(name);
      }

    private:
      static ValueDetailsLookup lookup_;
  };
} // namespace mopo

#endif // HELM_COMMON_H
