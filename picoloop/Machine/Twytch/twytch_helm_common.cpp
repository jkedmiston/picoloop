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

#include "twytch_helm_common.h"

namespace mopotwytchsynth {

  const ValueDetails ValueDetailsLookup::parameter_list[] = {
    { "amp_attack", 0.0, 4.0, 0, 0.1, 1.0,
      ValueDetails::kQuadratic, "secs", "Amp Attack" },
    { "amp_decay", 0.0, 4.0, 0, 1.5, 1.0,
      ValueDetails::kQuadratic, "secs", "Amp Decay" },
    { "amp_release", 0.0, 4.0, 0, 0.3, 1.0,
      ValueDetails::kQuadratic, "secs", "Amp Release" },
    { "amp_sustain", 0.0, 1.0, 0, 1.0, 1.0,
      ValueDetails::kLinear, "", "Amp Sustain" },
    { "arp_frequency", -1.0, 4.0, 0, 2.0, 1.0,
      ValueDetails::kExponential, "Hz", "Arp Frequency" },
    { "arp_gate", 0.0, 1.0, 0, 0.5, 1.0,
      ValueDetails::kLinear, "", "Arp Gate" },
    { "arp_octaves", 1.0, 4.0, 4, 1.0, 1.0,
      ValueDetails::kLinear, "octaves", "Arp Octaves" },
    { "arp_on", 0.0, 1.0, 2, 0.0, 1.0,
      ValueDetails::kLinear, "", "Arp Switch" },
    { "arp_pattern", 0.0, 4.0, 5, 0.0, 1.0,
      ValueDetails::kLinear, "", "Arp Pattern"},
    { "arp_sync", 0.0, 3.0, 4, 1.0, 1.0,
      ValueDetails::kLinear, "", "Arp Sync" },
    { "arp_tempo", 0.0, 11.0, 12, 9.0, 1.0,
      ValueDetails::kLinear, "", "Arp Tempo" },
    { "beats_per_minute", 20.0, 300.0, 0, 120.0, 1.0,
      ValueDetails::kLinear, "bpm", "BPM" },
    { "cross_modulation", 0.0, 0.5, 0, 0.0, 1.0,
      ValueDetails::kLinear, "", "Cross Mod" },
    { "cutoff", 28.0, 127.0, 0, 80.0, 1.0,
      ValueDetails::kLinear, "", "Filter Cutoff" },
    { "delay_dry_wet", 0.0, 1.0, 0, 0.5, 1.0,
      ValueDetails::kLinear, "", "Delay Wetness" },
    { "delay_feedback", -1.0, 1.0, 0, 0.4, 100.0,
      ValueDetails::kLinear, "%", "Delay Feedback" },
    { "delay_frequency", -2.0, 5.0, 0, 2.0, 1.0,
      ValueDetails::kExponential, "Hz", "Delay Frequency" },
    { "delay_on", 0.0, 1.0, 2, 0.0, 1.0,
      ValueDetails::kLinear, "", "Delay Switch" },
    { "delay_sync", 0.0, 3.0, 4, 1.0, 1.0,
      ValueDetails::kLinear, "", "Delay Sync" },
    { "delay_tempo", 0.0, 11.0, 12, 9.0, 1.0,
      ValueDetails::kLinear, "", "Delay Tempo" },
    { "fil_attack", 0.0, 4.0, 0, 0.0, 1.0,
      ValueDetails::kQuadratic, "secs", "Filter Attack" },
    { "fil_decay", 0.0, 4.0, 0, 1.5, 1.0,
      ValueDetails::kQuadratic, "secs", "Filter Decay" },
    { "fil_env_depth", -128.0, 128.0, 0, 0.0, 1.0,
      ValueDetails::kLinear, "", "Filter Env Depth" },
    { "fil_release", 0.0, 4.0, 0, 1.5, 1.0,
      ValueDetails::kQuadratic, "secs", "Filter Release" },
    { "fil_sustain", 0.0, 1.0, 0, 0.5, 1.0,
      ValueDetails::kLinear, "", "Filter Sustain" },
    { "filter_saturation", -60.0, 60.0, 0, 0.0, 1.0,
      ValueDetails::kLinear, "dB", "Saturation" },
    { "filter_type", 0.0, 6.0, 7, 6.0, 1.0,
      ValueDetails::kLinear, "", "Filter Type" },
    { "formant_on", 0.0, 1.0, 2, 0.0, 1.0,
      ValueDetails::kLinear, "", "Formant Switch" },
    { "formant_x", 0.0, 1.0, 0, 0.5, 1.0,
      ValueDetails::kLinear, "", "Formant X" },
    { "formant_y", 0.0, 1.0, 0, 0.5, 1.0,
      ValueDetails::kLinear, "", "Formant Y" },
    { "keytrack", -1.0, 1.0, 0, 0.0, 100.0,
      ValueDetails::kLinear, "%", "Filter Key Track" },
    { "legato", 0.0, 1.0, 2, 0.0, 1.0,
      ValueDetails::kLinear, "", "Legato" },
    { "mod_attack", 0.0, 4.0, 0, 0.0, 1.0,
      ValueDetails::kQuadratic, "secs", "Mod Env Attack" },
    { "mod_decay", 0.0, 4.0, 0, 1.5, 1.0,
      ValueDetails::kQuadratic, "secs", "Mod Env Decay" },
    { "mod_release", 0.0, 4.0, 0, 1.5, 1.0,
      ValueDetails::kQuadratic, "secs", "Mod Env Release" },
    { "mod_sustain", 0.0, 1.0, 0, 0.5, 1.0,
      ValueDetails::kLinear, "", "Mod Env Sustain" },
    { "mono_lfo_1_amplitude", -1.0, 1.0, 0, 1.0, 1.0,
      ValueDetails::kLinear, "", "Mono LFO 1 Amp" },
    { "mono_lfo_1_frequency", -7.0, 6.0, 0, 1.0, 1.0,
      ValueDetails::kExponential, "Hz", "Mono LFO 1 Frequency" },
    { "mono_lfo_1_retrigger", 0.0, 2.0, 3, 0.0, 1.0,
      ValueDetails::kLinear, "", "Mono LFO 1 Retrigger" },
    { "mono_lfo_1_sync", 0.0, 3.0, 4, 1.0, 1.0,
      ValueDetails::kLinear, "", "Mono LFO 1 Sync" },
    { "mono_lfo_1_tempo", 0.0, 11.0, 12, 6.0, 1.0,
      ValueDetails::kLinear, "", "Mono LFO 1 Tempo" },
    { "mono_lfo_1_waveform", 0.0, 12.0, 13, 0.0, 1.0,
      ValueDetails::kLinear, "", "Mono LFO 1 Waveform" },
    { "mono_lfo_2_amplitude", -1.0, 1.0, 0, 1.0, 1.0,
      ValueDetails::kLinear, "", "Mono LFO 2 Amp" },
    { "mono_lfo_2_frequency", -7.0, 6.0, 0, 1.0, 1.0,
      ValueDetails::kExponential, "Hz", "Mono LFO 2 Frequency" },
    { "mono_lfo_2_retrigger", 0.0, 2.0, 3, 0.0, 1.0,
      ValueDetails::kLinear, "", "Mono LFO 2 Retrigger" },
    { "mono_lfo_2_sync", 0.0, 3.0, 4, 1.0, 1.0,
      ValueDetails::kLinear, "", "Mono LFO 2 Sync" },
    { "mono_lfo_2_tempo", 0.0, 11.0, 12, 7.0, 1.0,
      ValueDetails::kLinear, "", "Mono LFO 2 Tempo" },
    { "mono_lfo_2_waveform", 0.0, 12.0, 13, 0.0, 1.0,
      ValueDetails::kLinear, "", "Mono LFO 2 Waveform" },
    { "noise_volume", 0.0, 1.0, 0, 0.0, 1.0,
      ValueDetails::kQuadratic, "", "Noise Volume" },
    { "num_steps", 1.0, 32.0, 32, 8.0, 1.0,
      ValueDetails::kLinear, "", "Num Steps" },
    { "osc_1_transpose", -48.0, 48.0, 97, 0.0, 1.0,
      ValueDetails::kLinear, "semitones", "Osc 1 Transpose" },
    { "osc_1_tune", -1.0, 1.0, 0, 0.0, 100.0,
      ValueDetails::kLinear, "cents", "Osc 1 Tune" },
    { "osc_1_unison_detune", 0.0, 100.0, 0, 0.0, 1.0,
      ValueDetails::kLinear, "cents", "Osc 1 Unison Detune" },
    { "osc_1_unison_voices", 1.0, 15.0, 8, 1.0, 1.0,
      ValueDetails::kLinear, "v", "Osc 1 Unison Voices" },
    { "osc_1_volume", 0.0, 1.0, 0, 0.4, 1.0,
      ValueDetails::kQuadratic, "", "Osc 1 Volume" },
    { "osc_1_waveform", 0.0, 10.0, 11, 0.0, 1.0,
      ValueDetails::kLinear, "", "Osc 1 Waveform" },
    { "osc_2_transpose", -48.0, 48.0, 97, 0.0, 1.0,
      ValueDetails::kLinear, "semitones", "Osc 2 Transpose" },
    { "osc_2_tune", -1.0, 1.0, 0, 0.0, 100.0,
      ValueDetails::kLinear, "cents", "Osc 2 Tune" },
    { "osc_2_unison_detune", 0.0, 100.0, 0, 0.0, 1.0,
      ValueDetails::kLinear, "cents", "Osc 2 Unison Detune" },
    { "osc_2_unison_voices", 1.0, 15.0, 8, 1.0, 1.0,
      ValueDetails::kLinear, "v", "Osc 2 Unison Voices" },
    { "osc_2_volume", 0.0, 1.0, 0, 0.4, 1.0,
      ValueDetails::kQuadratic, "", "Osc 2 Volume" },
    { "osc_2_waveform", 0.0, 10.0, 11, 0.0, 1.0,
      ValueDetails::kLinear, "", "Osc 2 Waveform" },
    { "osc_feedback_amount", -1.0, 1.0, 0, 0.0, 100.0,
      ValueDetails::kLinear, "%", "Osc Feedback Amount" },
    { "osc_feedback_transpose", -24.0, 24.0, 49, 0.0, 1.0,
      ValueDetails::kLinear, "semitones", "Osc Feedback Transpose" },
    { "osc_feedback_tune", -1.0, 1.0, 0, 0.0, 100.0,
      ValueDetails::kLinear, "cents", "Osc Feedback Tune" },
    { "osc_mix", 0.0, 1.0, 0, 0.5, 1.0,
      ValueDetails::kLinear, "", "Osc Mix" },
    { "pitch_bend_range", 0.0, 24.0, 25, 2.0, 1.0,
      ValueDetails::kLinear, "semitones", "Pitch Bend Range" },
    { "poly_lfo_amplitude", -1.0, 1.0, 0, 1.0, 1.0,
      ValueDetails::kLinear, "", "Poly LFO Amp" },
    { "poly_lfo_frequency", -7.0, 6.0, 0, 1.0, 1.0,
      ValueDetails::kExponential, "Hz", "Poly LFO Frequency" },
    { "poly_lfo_sync", 0.0, 3.0, 4, 1.0, 1.0,
      ValueDetails::kLinear, "", "Poly LFO Sync" },
    { "poly_lfo_tempo", 0.0, 11.0, 12, 7.0, 1.0,
      ValueDetails::kLinear, "", "Poly LFO Tempo" },
    { "poly_lfo_waveform", 0.0, 12.0, 13, 0.0, 1.0,
      ValueDetails::kLinear, "", "Poly LFO Waveform" },
    { "polyphony", 1.0, 32.0, 32, 4.0, 1.0,
      ValueDetails::kLinear, "voices", "Polyphony" },
    { "portamento", -9.0, -1.0, 0, -9.0, 12.0,
      ValueDetails::kExponential, "s/oct", "Portamento" },
    { "portamento_type", 0.0, 2.0, 3, 0.0, 1.0,
      ValueDetails::kLinear, "", "Portamento Type" },
    { "resonance", 0.0, 1.0, 0, 0.0, 1.0,
      ValueDetails::kLinear, "", "Filter Resonance" },
    { "reverb_damping", 0.0, 1.0, 0, 0.5, 1.0,
      ValueDetails::kLinear, "", "Reverb Damping" },
    { "reverb_dry_wet", 0.0, 1.0, 0, 0.5, 1.0,
      ValueDetails::kLinear, "", "Reverb Wetness" },
    { "reverb_feedback", 0.8, 1.0, 0, 0.9, 100.0,
      ValueDetails::kLinear, "%", "Reverb Feedback" },
    { "reverb_on", 0.0, 1.0, 2, 0.0, 1.0,
      ValueDetails::kLinear, "", "Reverb Switch" },
    { "step_frequency", -5.0, 6.0, 0, 2.0, 1.0,
      ValueDetails::kExponential, "Hz", "Step Frequency" },
    { "step_seq_00", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 1" },
    { "step_seq_01", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 2" },
    { "step_seq_02", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 3" },
    { "step_seq_03", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 4" },
    { "step_seq_04", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 5" },
    { "step_seq_05", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 6" },
    { "step_seq_06", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 7" },
    { "step_seq_07", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 8" },
    { "step_seq_08", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 9" },
    { "step_seq_09", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 10" },
    { "step_seq_10", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 11" },
    { "step_seq_11", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 12" },
    { "step_seq_12", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 13" },
    { "step_seq_13", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 14" },
    { "step_seq_14", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 15" },
    { "step_seq_15", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 16" },
    { "step_seq_16", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 17" },
    { "step_seq_17", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 18" },
    { "step_seq_18", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 19" },
    { "step_seq_19", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 20" },
    { "step_seq_20", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 21" },
    { "step_seq_21", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 22" },
    { "step_seq_22", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 23" },
    { "step_seq_23", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 24" },
    { "step_seq_24", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 25" },
    { "step_seq_25", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 26" },
    { "step_seq_26", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 27" },
    { "step_seq_27", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 28" },
    { "step_seq_28", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 29" },
    { "step_seq_29", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 30" },
    { "step_seq_30", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 31" },
    { "step_seq_31", -1.0, 1.0, 0, 0.0, 1.0, ValueDetails::kLinear, "", "Step 32" },
    { "step_sequencer_retrigger", 0.0, 2.0, 3, 0.0, 1.0,
      ValueDetails::kLinear, "", "Step Retrigger" },
    { "step_sequencer_sync", 0.0, 3.0, 4, 1.0, 1.0,
      ValueDetails::kLinear, "", "Step Sync" },
    { "step_sequencer_tempo", 0.0, 11.0, 12, 7.0, 1.0,
      ValueDetails::kLinear, "", "Step Tempo" },
    { "step_smoothing", 0.0, 0.5, 0, 0.0, 1.0,
      ValueDetails::kLinear, "", "Step Smoothing" },
    { "stutter_frequency", 0.0, 7.0, 0, 3.0, 1.0,
      ValueDetails::kExponential, "Hz", "Stutter Frequency" },
    { "stutter_sync", 0.0, 3.0, 4, 1.0, 1.0,
      ValueDetails::kLinear, "", "Stutter Sync" },
    { "stutter_tempo", 6.0, 11.0, 6, 8.0, 1.0,
      ValueDetails::kLinear, "", "Stutter Tempo" },
    { "stutter_on", 0.0, 1.0, 2, 0.0, 1.0,
      ValueDetails::kLinear, "", "Stutter Switch" },
    { "stutter_resample_frequency", -7.0, 4.0, 0, 1.0, 1.0,
      ValueDetails::kExponential, "Hz", "Resample Frequency" },
    { "stutter_resample_sync", 0.0, 3.0, 4, 1.0, 1.0,
      ValueDetails::kLinear, "", "Stutter Sync" },
    { "stutter_resample_tempo", 0.0, 11.0, 12, 6.0, 1.0,
      ValueDetails::kLinear, "", "Stutter Tempo" },
    { "stutter_softness", 0.0, 1.0, 0, 0.2, 100.0,
      ValueDetails::kLinear, "%", "Stutter Softness" },
    { "sub_shuffle", 0.0, 1.0, 0, 0.0, 100.0,
      ValueDetails::kLinear, "%", "Sub Osc Shuffle" },
    { "sub_transpose", -12.0, 12.0, 25, 0.0, 1.0,
      ValueDetails::kLinear, "semitones", "Sub Osc Transpose" },
    { "sub_tune", -1.0, 1.0, 0, 0.0, 100.0,
      ValueDetails::kLinear, "cents", "Sub Osc Tune" },
    { "sub_volume", 0.0, 1.0, 0, 0.0, 1.0,
      ValueDetails::kQuadratic, "", "Sub Osc Volume" },
    { "sub_waveform", 0.0, 10.0, 11, 2.0, 1.0,
      ValueDetails::kLinear, "", "Sub Osc Waveform" },
    { "unison_1_harmonize", 0.0, 1.0, 2, 0.0, 1.0,
      ValueDetails::kLinear, "Osc 1 Harmonize" },
    { "unison_2_harmonize", 0.0, 1.0, 2, 0.0, 1.0,
      ValueDetails::kLinear, "Osc 2 Harmonize" },
    { "velocity_track", -1.0, 1.0, 0, 0.0, 100.0,
      ValueDetails::kLinear, "%", "Velocity Track" },
    { "volume", 0.0, 1.41421356237, 0, 1.0, 1.0,
      ValueDetails::kQuadratic, "", "Volume" },
  };

  ModulationConnectionBank::ModulationConnectionBank() {
    allocateMoreConnections();
  }

  ModulationConnectionBank::~ModulationConnectionBank() {
    for (ModulationConnection* connection : all_connections_)
      delete connection;
  }

  ModulationConnection* ModulationConnectionBank::get(const std::string& from,
                                                      const std::string& to) {
    if (available_connections_.size() == 0)
      allocateMoreConnections();

    ModulationConnection* connection = available_connections_.front();
    available_connections_.pop_front();
    connection->resetConnection(from, to);
    return connection;
  }

  void ModulationConnectionBank::recycle(ModulationConnection* connection) {
    available_connections_.push_back(connection);
  }

  void ModulationConnectionBank::allocateMoreConnections() {
    for (int i = 0; i < DEFAULT_MODULATION_CONNECTIONS; ++i) {
      ModulationConnection* connection = new ModulationConnection();
      available_connections_.push_back(connection);
      all_connections_.push_back(connection);
    }
  }

  ModulationConnectionBank* ModulationConnectionBank::instance() {
    static ModulationConnectionBank instance;
    return &instance;
  }

  ValueDetailsLookup::ValueDetailsLookup() {
    int num_parameters = sizeof(parameter_list) / sizeof(ValueDetails);
    for (int i = 0; i < num_parameters; ++i) {
      details_lookup_[parameter_list[i].name] = parameter_list[i];

      TWYTCH_MOPO_ASSERT(parameter_list[i].default_value <= parameter_list[i].max);
      TWYTCH_MOPO_ASSERT(parameter_list[i].default_value >= parameter_list[i].min);
    }
  }

  ValueDetailsLookup Parameters::lookup_;

} // namespace mopo
