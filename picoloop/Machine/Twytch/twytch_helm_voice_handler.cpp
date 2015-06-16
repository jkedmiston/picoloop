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

#include "twytch_helm_voice_handler.h"

#include "twytch_fixed_point_oscillator.h"
#include "twytch_noise_oscillator.h"
#include "twytch_resonance_cancel.h"
#include "twytch_helm_lfo.h"
#include "twytch_helm_oscillators.h"

#include <sstream>

#define PITCH_MOD_RANGE 12
#define MIN_GAIN_DB -24.0
#define MAX_GAIN_DB 24.0

#define MAX_FEEDBACK_SAMPLES 20000
#define MAX_POLYPHONY 32

namespace mopotwytchsynth {

  namespace {
    struct FormantValues {
      Value* gain;
      Value* resonance;
      Value* midi_cutoff;
    };

    static const Value formant_filter_types[NUM_FORMANTS] = {
      Value(Filter::kHighPass),
      Value(Filter::kBandShelf),
      Value(Filter::kBandShelf),
      Value(Filter::kLowPass)
    };

    static const FormantValues formant_a[NUM_FORMANTS] = {
      {new Value(1.0), new Value(4.75), new Value(77.9534075105)},
      {new Value(9.68), new Value(12.0), new Value(84.8631371387)},
      {new Value(-13.1), new Value(2.04), new Value(89.764715283)},
      {new Value(1.0), new Value(4.75), new Value(99.1453432239)},
    };

    static const FormantValues formant_e[NUM_FORMANTS] = {
      {new Value(1.0), new Value(4.86), new Value(66.9116472028)},
      {new Value(15.0), new Value(12.0), new Value(95.126316023)},
      {new Value(-15.0), new Value(2.04), new Value(76.7388604942)},
      {new Value(1.0), new Value(5.04), new Value(99.4190618188)},
    };

    static const FormantValues formant_i[NUM_FORMANTS] = {
      {new Value(1.0), new Value(4.86), new Value(60.5454706024)},
      {new Value(15.0), new Value(12.0), new Value(97.5572660336)},
      {new Value(-15.0), new Value(0.63), new Value(79.9872937526)},
      {new Value(1.0), new Value(5.04), new Value(102.80031344)},
    };

    static const FormantValues formant_o[NUM_FORMANTS] = {
      {new Value(1.0), new Value(4.86), new Value(72.2218660312)},
      {new Value(15.0), new Value(12.0), new Value(93.7695640491)},
      {new Value(-15.0), new Value(2.04), new Value(90.5754746888)},
      {new Value(1.0), new Value(5.04), new Value(98.9371763011)},
    };

    static const FormantValues formant_u[NUM_FORMANTS] = {
      {new Value(1.0), new Value(4.86), new Value(62.3695077237)},
      {new Value(15.0), new Value(12.0), new Value(80.8021425266)},
      {new Value(-9.19), new Value(2.04), new Value(75.9143074932)},
      {new Value(1.0), new Value(7.61), new Value(97.1750796411)},
    };

    static const FormantValues formant_uu[NUM_FORMANTS] = {
      {new Value(1.0), new Value(4.86), new Value(77.764715283)},
      {new Value(15.0), new Value(12.0), new Value(80.1946296498)},
      {new Value(-15.0), new Value(2.04), new Value(121.117016958)},
      {new Value(1.0), new Value(7.61), new Value(98.6556686272)},
    };

  } // namespace

  HelmVoiceHandler::HelmVoiceHandler(Processor* beats_per_second) :
      ProcessorRouter(VoiceHandler::kNumInputs, 0), VoiceHandler(MAX_POLYPHONY),
      beats_per_second_(beats_per_second) {
    output_ = new Multiply();
    registerOutput(output_->output());
  }

  void HelmVoiceHandler::init() {
    // Create modulation and pitch wheels.
    mod_wheel_amount_ = new SmoothValue(0);
    pitch_wheel_amount_ = new SmoothValue(0);

    mod_sources_["pitch_wheel"] = pitch_wheel_amount_->output();
    mod_sources_["mod_wheel"] = mod_wheel_amount_->output();

    // Create all synthesizer voice components.
    createArticulation(note(), velocity(), voice_event());
    createOscillators(current_frequency_->output(),
                      amplitude_envelope_->output(Envelope::kFinished));
    createModulators(amplitude_envelope_->output(Envelope::kFinished));
    createFilter(osc_feedback_->output(0), note_from_center_->output(),
                 amplitude_envelope_->output(Envelope::kFinished), voice_event());

    Value* aftertouch_value = new Value();
    aftertouch_value->plug(aftertouch());

    addProcessor(aftertouch_value);
    mod_sources_["aftertouch"] = aftertouch_value->output();

    output_->plug(formant_container_, 0);
    output_->plug(amplitude_, 1);

    addProcessor(output_);
    addGlobalProcessor(pitch_wheel_amount_);
    addGlobalProcessor(mod_wheel_amount_);

    setVoiceKiller(amplitude_envelope_->output(Envelope::kValue));
    
    HelmModule::init();
  }

  void HelmVoiceHandler::createOscillators(Output* midi, Output* reset) {
    // Pitch bend.
    Processor* pitch_bend_range = createPolyModControl("pitch_bend_range", false);
    Multiply* pitch_bend = new Multiply();
    pitch_bend->setControlRate();
    pitch_bend->plug(pitch_wheel_amount_, 0);
    pitch_bend->plug(pitch_bend_range, 1);
    Add* bent_midi = new Add();
    bent_midi->plug(midi, 0);
    bent_midi->plug(pitch_bend, 1);

    addProcessor(pitch_bend);
    addProcessor(bent_midi);

    // Oscillator 1.
    HelmOscillators* oscillators = new HelmOscillators();
    Processor* oscillator1_waveform = createPolyModControl("osc_1_waveform", true);
    Processor* oscillator1_transpose = createPolyModControl("osc_1_transpose", false);
    Processor* oscillator1_tune = createPolyModControl("osc_1_tune", false);
    Processor* oscillator1_unison_voices = createPolyModControl("osc_1_unison_voices", true);
    Processor* oscillator1_unison_detune = createPolyModControl("osc_1_unison_detune", true);
    Processor* oscillator1_unison_harmonize = createBaseControl("unison_1_harmonize");

    Add* oscillator1_transposed = new Add();
    oscillator1_transposed->plug(bent_midi, 0);
    oscillator1_transposed->plug(oscillator1_transpose, 1);
    Add* oscillator1_midi = new Add();
    oscillator1_midi->plug(oscillator1_transposed, 0);
    oscillator1_midi->plug(oscillator1_tune, 1);

    MidiScale* oscillator1_frequency = new MidiScale();
    oscillator1_frequency->plug(oscillator1_midi);
    FrequencyToPhase* oscillator1_phase_inc = new FrequencyToPhase();
    oscillator1_phase_inc->plug(oscillator1_frequency);

    oscillators->plug(oscillator1_waveform, HelmOscillators::kOscillator1Waveform);
    oscillators->plug(reset, HelmOscillators::kReset);
    oscillators->plug(oscillator1_phase_inc, HelmOscillators::kOscillator1PhaseInc);
    oscillators->plug(oscillator1_unison_detune, HelmOscillators::kUnisonDetune1);
    oscillators->plug(oscillator1_unison_voices, HelmOscillators::kUnisonVoices1);
    oscillators->plug(oscillator1_unison_harmonize, HelmOscillators::kHarmonize1);

    Processor* cross_mod = createPolyModControl("cross_modulation", false, true);
    oscillators->plug(cross_mod, HelmOscillators::kCrossMod);

    addProcessor(oscillator1_transposed);
    addProcessor(oscillator1_midi);
    addProcessor(oscillator1_frequency);
    addProcessor(oscillator1_phase_inc);
    addProcessor(oscillators);

    // Oscillator 2.
    Processor* oscillator2_waveform = createPolyModControl("osc_2_waveform", true);
    Processor* oscillator2_transpose = createPolyModControl("osc_2_transpose", false);
    Processor* oscillator2_tune = createPolyModControl("osc_2_tune", false);
    Processor* oscillator2_unison_voices = createPolyModControl("osc_2_unison_voices", true);
    Processor* oscillator2_unison_detune = createPolyModControl("osc_2_unison_detune", true);
    Processor* oscillator2_unison_harmonize = createBaseControl("unison_2_harmonize");

    Add* oscillator2_transposed = new Add();
    oscillator2_transposed->plug(bent_midi, 0);
    oscillator2_transposed->plug(oscillator2_transpose, 1);
    Add* oscillator2_midi = new Add();
    oscillator2_midi->plug(oscillator2_transposed, 0);
    oscillator2_midi->plug(oscillator2_tune, 1);

    MidiScale* oscillator2_frequency = new MidiScale();
    oscillator2_frequency->plug(oscillator2_midi);
    FrequencyToPhase* oscillator2_phase_inc = new FrequencyToPhase();
    oscillator2_phase_inc->plug(oscillator2_frequency);

    oscillators->plug(oscillator2_waveform, HelmOscillators::kOscillator2Waveform);
    oscillators->plug(oscillator2_phase_inc, HelmOscillators::kOscillator2PhaseInc);
    oscillators->plug(oscillator2_unison_detune, HelmOscillators::kUnisonDetune2);
    oscillators->plug(oscillator2_unison_voices, HelmOscillators::kUnisonVoices2);
    oscillators->plug(oscillator2_unison_harmonize, HelmOscillators::kHarmonize2);

    addProcessor(oscillator2_transposed);
    addProcessor(oscillator2_midi);
    addProcessor(oscillator2_frequency);
    addProcessor(oscillator2_phase_inc);

    // Oscillator mix.
    Processor* oscillator_mix_amount = createPolyModControl("osc_mix", false, true);
    Clamp* clamp_mix = new Clamp(0, 1);
    clamp_mix->plug(oscillator_mix_amount);
    oscillators->plug(clamp_mix, HelmOscillators::kMix);
    addProcessor(clamp_mix);

    // Sub Oscillator.
    Add* sub_midi = new Add();
    Value* sub_transpose = new Value(-TWYTCH_NOTES_PER_OCTAVE);
    sub_midi->plug(bent_midi, 0);
    sub_midi->plug(sub_transpose, 1);

    MidiScale* sub_frequency = new MidiScale();
    sub_frequency->plug(sub_midi);
    FrequencyToPhase* sub_phase_inc = new FrequencyToPhase();
    sub_phase_inc->plug(sub_frequency);

    Processor* sub_waveform = createPolyModControl("sub_waveform", true);
    FixedPointOscillator* sub_oscillator = new FixedPointOscillator();
    sub_oscillator->plug(sub_phase_inc, FixedPointOscillator::kPhaseInc);
    sub_oscillator->plug(sub_waveform, FixedPointOscillator::kWaveform);
    sub_oscillator->plug(reset, FixedPointOscillator::kReset);

    Processor* sub_volume = createPolyModControl("sub_volume", false, true);
    Multiply* scaled_sub_oscillator = new Multiply();
    scaled_sub_oscillator->plug(sub_oscillator, 0);
    scaled_sub_oscillator->plug(sub_volume, 1);

    addProcessor(sub_midi);
    addProcessor(sub_frequency);
    addProcessor(sub_phase_inc);
    addProcessor(sub_oscillator);
    addProcessor(scaled_sub_oscillator);

    Add *oscillator_sum = new Add();
    oscillator_sum->plug(oscillators, 0);
    oscillator_sum->plug(scaled_sub_oscillator, 1);

    addProcessor(oscillator_sum);

    // Noise Oscillator.
    NoiseOscillator* noise_oscillator = new NoiseOscillator();

    Processor* noise_volume = createPolyModControl("noise_volume", false);
    Multiply* scaled_noise_oscillator = new Multiply();
    scaled_noise_oscillator->plug(noise_oscillator, 0);
    scaled_noise_oscillator->plug(noise_volume, 1);

    addProcessor(noise_oscillator);
    addProcessor(scaled_noise_oscillator);

    Add *oscillator_noise_sum = new Add();
    oscillator_noise_sum->plug(oscillator_sum, 0);
    oscillator_noise_sum->plug(scaled_noise_oscillator, 1);

    addProcessor(oscillator_noise_sum);

    // Oscillator feedback.
    Processor* osc_feedback_transpose = createPolyModControl("osc_feedback_transpose", false, true);
    Processor* osc_feedback_amount = createPolyModControl("osc_feedback_amount", false);
    Processor* osc_feedback_tune = createPolyModControl("osc_feedback_tune", true);
    Add* osc_feedback_transposed = new Add();
    osc_feedback_transposed->setControlRate();
    osc_feedback_transposed->plug(bent_midi, 0);
    osc_feedback_transposed->plug(osc_feedback_transpose, 1);
    Add* osc_feedback_midi = new Add();
    osc_feedback_midi->setControlRate();
    osc_feedback_midi->plug(osc_feedback_transposed, 0);
    osc_feedback_midi->plug(osc_feedback_tune, 1);

    MidiScale* osc_feedback_frequency = new MidiScale();
    osc_feedback_frequency->setControlRate();
    osc_feedback_frequency->plug(osc_feedback_midi);

    FrequencyToSamples* osc_feedback_samples = new FrequencyToSamples();
    osc_feedback_samples->plug(osc_feedback_frequency);

    SampleAndHoldBuffer* osc_feedback_samples_audio = new SampleAndHoldBuffer();
    osc_feedback_samples_audio->plug(osc_feedback_samples);

    addProcessor(osc_feedback_transposed);
    addProcessor(osc_feedback_midi);
    addProcessor(osc_feedback_frequency);
    addProcessor(osc_feedback_samples);
    addProcessor(osc_feedback_samples_audio);

    Clamp* osc_feedback_amount_clamped = new Clamp();
    osc_feedback_amount_clamped->plug(osc_feedback_amount);

    osc_feedback_ = new SimpleDelay(MAX_FEEDBACK_SAMPLES);
    osc_feedback_->plug(oscillator_noise_sum, SimpleDelay::kAudio);
    osc_feedback_->plug(osc_feedback_samples_audio, SimpleDelay::kSampleDelay);
    osc_feedback_->plug(osc_feedback_amount_clamped, SimpleDelay::kFeedback);
    addProcessor(osc_feedback_);
    addProcessor(osc_feedback_amount_clamped);
  }

  void HelmVoiceHandler::createModulators(Output* reset) {
    // Poly LFO.
    Processor* lfo_waveform = createPolyModControl("poly_lfo_waveform", true);
    Processor* lfo_free_frequency = createPolyModControl("poly_lfo_frequency", true, false);
    Processor* lfo_free_amplitude = createPolyModControl("poly_lfo_amplitude", true);
    Processor* lfo_frequency = createTempoSyncSwitch("poly_lfo", lfo_free_frequency,
                                                     beats_per_second_, true);
    HelmLfo* lfo = new HelmLfo();
    lfo->plug(reset, HelmLfo::kReset);
    lfo->plug(lfo_waveform, HelmLfo::kWaveform);
    lfo->plug(lfo_frequency, HelmLfo::kFrequency);

    Multiply* scaled_lfo = new Multiply();
    scaled_lfo->setControlRate();
    scaled_lfo->plug(lfo, 0);
    scaled_lfo->plug(lfo_free_amplitude, 1);

    addProcessor(lfo);
    addProcessor(scaled_lfo);
    mod_sources_["poly_lfo"] = scaled_lfo->output();
  }

  void HelmVoiceHandler::createFilter(
      Output* audio, Output* keytrack, Output* reset, Output* note_event) {
    // Filter envelope.
    Processor* filter_attack = createPolyModControl("fil_attack", false, false);
    Processor* filter_decay = createPolyModControl("fil_decay", true, false);
    Processor* filter_sustain = createPolyModControl("fil_sustain", false);
    Processor* filter_release = createPolyModControl("fil_release", true, false);

    TriggerFilter* note_off = new TriggerFilter(kVoiceOff);
    note_off->plug(note_event);
    TriggerCombiner* filter_env_trigger = new TriggerCombiner();
    filter_env_trigger->plug(note_off, 0);
    filter_env_trigger->plug(reset, 1);

    filter_envelope_ = new Envelope();
    filter_envelope_->plug(filter_attack, Envelope::kAttack);
    filter_envelope_->plug(filter_decay, Envelope::kDecay);
    filter_envelope_->plug(filter_sustain, Envelope::kSustain);
    filter_envelope_->plug(filter_release, Envelope::kRelease);
    filter_envelope_->plug(filter_env_trigger, Envelope::kTrigger);

    Processor* filter_envelope_depth = createPolyModControl("fil_env_depth", false);
    Multiply* scaled_envelope = new Multiply();
    scaled_envelope->setControlRate();
    scaled_envelope->plug(filter_envelope_, 0);
    scaled_envelope->plug(filter_envelope_depth, 1);

    addProcessor(filter_envelope_);
    addProcessor(note_off);
    addProcessor(filter_env_trigger);
    addProcessor(scaled_envelope);

    // Filter.
    Processor* filter_type = createBaseControl("filter_type");
    Processor* keytrack_amount = createPolyModControl("keytrack", false);
    Multiply* current_keytrack = new Multiply();
    current_keytrack->setControlRate();
    current_keytrack->plug(keytrack, 0);
    current_keytrack->plug(keytrack_amount, 1);

    Processor* base_cutoff = createPolyModControl("cutoff", true, true);
    Add* keytracked_cutoff = new Add();
    keytracked_cutoff->setControlRate();
    keytracked_cutoff->plug(base_cutoff, 0);
    keytracked_cutoff->plug(current_keytrack, 1);

    Add* midi_cutoff = new Add();
    midi_cutoff->setControlRate();
    midi_cutoff->plug(keytracked_cutoff, 0);
    midi_cutoff->plug(scaled_envelope, 1);

    MidiScale* frequency_cutoff = new MidiScale();
    frequency_cutoff->setControlRate();
    frequency_cutoff->plug(midi_cutoff);

    Processor* resonance = createPolyModControl("resonance", true);
    ResonanceScale* scaled_resonance = new ResonanceScale();
    scaled_resonance->setControlRate();
    scaled_resonance->plug(resonance);

    ResonanceCancel* final_resonance = new ResonanceCancel();
    final_resonance->setControlRate();
    final_resonance->plug(scaled_resonance, ResonanceCancel::kResonance);
    final_resonance->plug(filter_type, ResonanceCancel::kFilterType);

    Value* min_db = new Value(MIN_GAIN_DB);
    Value* max_db = new Value(MAX_GAIN_DB);
    Interpolate* decibals = new Interpolate();
    decibals->setControlRate();
    decibals->plug(min_db, Interpolate::kFrom);
    decibals->plug(max_db, Interpolate::kTo);
    decibals->plug(resonance, Interpolate::kFractional);
    MagnitudeScale* final_gain = new MagnitudeScale();
    final_gain->setControlRate();
    final_gain->plug(decibals);

    Processor* filter_saturation = createPolyModControl("filter_saturation", true);
    MagnitudeScale* saturation_magnitude = new MagnitudeScale();
    saturation_magnitude->setControlRate();
    saturation_magnitude->plug(filter_saturation);

    LinearSmoothBuffer* smooth_saturation_magnitude = new LinearSmoothBuffer();
    smooth_saturation_magnitude->plug(saturation_magnitude);

    Multiply* saturated_audio = new Multiply();
    saturated_audio->plug(audio, 0);
    saturated_audio->plug(smooth_saturation_magnitude, 1);

    Filter* filter = new Filter();
    filter->plug(saturated_audio, Filter::kAudio);
    filter->plug(filter_type, Filter::kType);
    filter->plug(reset, Filter::kReset);
    filter->plug(frequency_cutoff, Filter::kCutoff);
    filter->plug(final_resonance, Filter::kResonance);
    filter->plug(final_gain, Filter::kGain);

    distorted_filter_ = new Distortion();
    Value* distortion_type = new Value(Distortion::kTanh);
    Value* distortion_threshold = new Value(0.5);
    distorted_filter_->plug(filter, Distortion::kAudio);
    distorted_filter_->plug(distortion_type, Distortion::kType);
    distorted_filter_->plug(distortion_threshold, Distortion::kThreshold);

    addProcessor(current_keytrack);
    addProcessor(saturated_audio);
    addProcessor(keytracked_cutoff);
    addProcessor(midi_cutoff);
    addProcessor(scaled_resonance);
    addProcessor(final_resonance);
    addProcessor(decibals);
    addProcessor(final_gain);
    addProcessor(frequency_cutoff);
    addProcessor(filter);

    addProcessor(saturation_magnitude);
    addProcessor(smooth_saturation_magnitude);
    addProcessor(distorted_filter_);

    mod_sources_["fil_envelope"] = filter_envelope_->output();

    // Stutter.
    BypassRouter* stutter_container = new BypassRouter();
    Processor* stutter_on = createBaseControl("stutter_on");
    stutter_container->plug(stutter_on, BypassRouter::kOn);
    stutter_container->plug(distorted_filter_, BypassRouter::kAudio);

    Stutter* stutter = new Stutter(44100);
    Processor* stutter_frequency = createPolyModControl("stutter_frequency", false);
    Processor* resample_frequency = createPolyModControl("stutter_resample_frequency", false);
    stutter_container->addProcessor(stutter);
    stutter_container->registerOutput(stutter->output());

    stutter->plug(distorted_filter_, Stutter::kAudio);
    stutter->plug(stutter_frequency, Stutter::kStutterFrequency);
    stutter->plug(resample_frequency, Stutter::kResampleFrequency);
    stutter->plug(reset, Stutter::kReset);

    addProcessor(stutter_container);

    // Formant Filter.
    formant_container_ = new BypassRouter();
    Processor* formant_on = createBaseControl("formant_on");
    formant_container_->plug(formant_on, BypassRouter::kOn);
    formant_container_->plug(stutter_container, BypassRouter::kAudio);

    formant_filter_ = new FormantManager(NUM_FORMANTS);
    formant_filter_->plug(stutter_container, FormantManager::kAudio);
    formant_filter_->plug(reset, FormantManager::kReset);

    Processor* formant_x = createPolyModControl("formant_x", false, true);
    Processor* formant_y = createPolyModControl("formant_y", false, true);

    for (int i = 0; i < NUM_FORMANTS; ++i) {
      BilinearInterpolate* formant_gain = new BilinearInterpolate();
      formant_gain->setControlRate();
      BilinearInterpolate* formant_q = new BilinearInterpolate();
      formant_q->setControlRate();
      BilinearInterpolate* formant_midi = new BilinearInterpolate();
      formant_midi->setControlRate();

      formant_gain->plug(formant_u[i].gain, BilinearInterpolate::kTopLeft);
      formant_gain->plug(formant_a[i].gain, BilinearInterpolate::kTopRight);
      formant_gain->plug(formant_i[i].gain, BilinearInterpolate::kBottomLeft);
      formant_gain->plug(formant_o[i].gain, BilinearInterpolate::kBottomRight);

      formant_q->plug(formant_u[i].resonance, BilinearInterpolate::kTopLeft);
      formant_q->plug(formant_a[i].resonance, BilinearInterpolate::kTopRight);
      formant_q->plug(formant_i[i].resonance, BilinearInterpolate::kBottomLeft);
      formant_q->plug(formant_o[i].resonance, BilinearInterpolate::kBottomRight);

      formant_midi->plug(formant_u[i].midi_cutoff, BilinearInterpolate::kTopLeft);
      formant_midi->plug(formant_a[i].midi_cutoff, BilinearInterpolate::kTopRight);
      formant_midi->plug(formant_i[i].midi_cutoff, BilinearInterpolate::kBottomLeft);
      formant_midi->plug(formant_o[i].midi_cutoff, BilinearInterpolate::kBottomRight);

      formant_gain->plug(formant_x, BilinearInterpolate::kXPosition);
      formant_q->plug(formant_x, BilinearInterpolate::kXPosition);
      formant_midi->plug(formant_x, BilinearInterpolate::kXPosition);

      formant_gain->plug(formant_y, BilinearInterpolate::kYPosition);
      formant_q->plug(formant_y, BilinearInterpolate::kYPosition);
      formant_midi->plug(formant_y, BilinearInterpolate::kYPosition);

      MagnitudeScale* formant_magnitude = new MagnitudeScale();
      formant_magnitude->plug(formant_gain);
      formant_magnitude->setControlRate();

      MidiScale* formant_frequency = new MidiScale();
      formant_frequency->plug(formant_midi);
      formant_frequency->setControlRate();

      formant_filter_->getFormant(i)->plug(&formant_filter_types[i], Filter::kType);
      formant_filter_->getFormant(i)->plug(formant_magnitude, Filter::kGain);
      formant_filter_->getFormant(i)->plug(formant_q, Filter::kResonance);
      formant_filter_->getFormant(i)->plug(formant_frequency, Filter::kCutoff);

      addProcessor(formant_gain);
      addProcessor(formant_magnitude);
      addProcessor(formant_q);
      addProcessor(formant_midi);
      addProcessor(formant_frequency);
    }

    formant_container_->addProcessor(formant_filter_);
    formant_container_->registerOutput(formant_filter_->output());

    addProcessor(formant_container_);
  }

  void HelmVoiceHandler::createArticulation(
      Output* note, Output* velocity, Output* trigger) {
    // Legato.
    Processor* legato = createBaseControl("legato");
    LegatoFilter* legato_filter = new LegatoFilter();
    legato_filter->plug(legato, LegatoFilter::kLegato);
    legato_filter->plug(trigger, LegatoFilter::kTrigger);

    addProcessor(legato_filter);

    // Amplitude envelope.
    Processor* amplitude_attack = createPolyModControl("amp_attack", false, false);
    Processor* amplitude_decay = createPolyModControl("amp_decay", true, false);
    Processor* amplitude_sustain = createPolyModControl("amp_sustain", false);
    Processor* amplitude_release = createPolyModControl("amp_release", true, false);

    amplitude_envelope_ = new Envelope();
    amplitude_envelope_->plug(legato_filter->output(LegatoFilter::kRetrigger),
                              Envelope::kTrigger);
    amplitude_envelope_->plug(amplitude_attack, Envelope::kAttack);
    amplitude_envelope_->plug(amplitude_decay, Envelope::kDecay);
    amplitude_envelope_->plug(amplitude_sustain, Envelope::kSustain);
    amplitude_envelope_->plug(amplitude_release, Envelope::kRelease);
    addProcessor(amplitude_envelope_);

    // Voice and frequency resetting logic.
    TriggerCombiner* note_change_trigger = new TriggerCombiner();
    note_change_trigger->plug(legato_filter->output(LegatoFilter::kRemain), 0);
    note_change_trigger->plug(amplitude_envelope_->output(Envelope::kFinished), 1);

    TriggerWait* note_wait = new TriggerWait();
    Value* current_note = new Value();
    note_wait->plug(note, TriggerWait::kWait);
    note_wait->plug(note_change_trigger, TriggerWait::kTrigger);
    current_note->plug(note_wait);

    Value* max_midi_invert = new Value(1.0 / (MIDI_SIZE - 1));
    Multiply* note_percentage = new Multiply();
    note_percentage->setControlRate();
    note_percentage->plug(max_midi_invert, 0);
    note_percentage->plug(current_note, 1);

    addProcessor(note_change_trigger);
    addProcessor(note_wait);
    addProcessor(current_note);

    // Key tracking.
    Value* center_adjust = new Value(-MIDI_SIZE / 2);
    note_from_center_ = new Add();
    note_from_center_->plug(center_adjust, 0);
    note_from_center_->plug(current_note, 1);

    addProcessor(note_from_center_);
    addProcessor(note_percentage);
    addGlobalProcessor(center_adjust);

    // Velocity tracking.
    TriggerWait* velocity_wait = new TriggerWait();
    Value* current_velocity = new Value();
    velocity_wait->plug(velocity, TriggerWait::kWait);
    velocity_wait->plug(note_change_trigger, TriggerWait::kTrigger);
    current_velocity->plug(velocity_wait);

    addProcessor(velocity_wait);
    addProcessor(current_velocity);

    Processor* velocity_track_amount = createPolyModControl("velocity_track", false);
    Interpolate* velocity_track_mult = new Interpolate();
    velocity_track_mult->plug(&utils::value_one, Interpolate::kFrom);
    velocity_track_mult->plug(current_velocity, Interpolate::kTo);
    velocity_track_mult->plug(velocity_track_amount, Interpolate::kFractional);
    addProcessor(velocity_track_mult);

    // Current amplitude using envelope and velocity.
    amplitude_ = new Multiply();
    amplitude_->plug(amplitude_envelope_->output(Envelope::kValue), 0);
    amplitude_->plug(velocity_track_mult, 1);

    addProcessor(amplitude_);

    // Portamento.
    Processor* portamento = createPolyModControl("portamento", false, false);
    Processor* portamento_type = createBaseControl("portamento_type");
    PortamentoFilter* portamento_filter = new PortamentoFilter();
    portamento_filter->plug(portamento_type, PortamentoFilter::kPortamento);
    portamento_filter->plug(note_change_trigger, PortamentoFilter::kFrequencyTrigger);
    portamento_filter->plug(trigger, PortamentoFilter::kVoiceTrigger);
    addProcessor(portamento_filter);

    current_frequency_ = new LinearSlope();
    current_frequency_->plug(current_note, LinearSlope::kTarget);
    current_frequency_->plug(portamento, LinearSlope::kRunSeconds);
    current_frequency_->plug(portamento_filter, LinearSlope::kTriggerJump);

    addProcessor(current_frequency_);

    mod_sources_["amp_envelope"] = amplitude_envelope_->output();
    mod_sources_["note"] = note_percentage->output();
    mod_sources_["velocity"] = current_velocity->output();
  }

  void HelmVoiceHandler::setModWheel(mopo_float value) {
    mod_wheel_amount_->set(value);
  }

  void HelmVoiceHandler::setPitchWheel(mopo_float value) {
    pitch_wheel_amount_->set(value);
  }
} // namespace mopo
