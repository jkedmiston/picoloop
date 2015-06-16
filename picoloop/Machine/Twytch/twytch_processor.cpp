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

#include "twytch_processor.h"

#include "twytch_feedback.h"
#include "twytch_processor_router.h"

namespace mopotwytchsynth {

  const Processor::Output Processor::null_source_;

  Processor::Processor(int num_inputs, int num_outputs) :
      sample_rate_(DEFAULT_SAMPLE_RATE), buffer_size_(DEFAULT_BUFFER_SIZE),
      control_rate_(false),
      inputs_(new std::vector<Input*>()), outputs_(new std::vector<Output*>()),
      router_(0) {
    for (int i = 0; i < num_inputs; ++i) {
      Input* input = new Input();
      owned_inputs_.push_back(input);

      // All inputs start off with null input.
      input->source = &Processor::null_source_;
      registerInput(input);
    }

    for (int i = 0; i < num_outputs; ++i) {
      Output* output = new Output();
      owned_outputs_.push_back(output);

      // All outputs are owned by this Processor.
      output->owner = this;
      registerOutput(output);
    }
  }

  bool Processor::isPolyphonic() const {
    if (router_)
      return router_->isPolyphonic(this);
    return false;
  }

  void Processor::plug(const Output* source) {
    plug(source, 0);
  }

  void Processor::plug(const Output* source, unsigned int input_index) {
    TWYTCH_MOPO_ASSERT(input_index < inputs_->size());
    TWYTCH_MOPO_ASSERT(source);
    TWYTCH_MOPO_ASSERT(inputs_->at(input_index));

    inputs_->at(input_index)->source = source;

    if (router_)
      router_->connect(this, source, input_index);
  }

  void Processor::plug(const Processor* source) {
    plug(source, 0);
  }

  void Processor::plug(const Processor* source, unsigned int input_index) {
    plug(source->output(), input_index);
  }

  void Processor::plugNext(const Output* source) {
    for (size_t i = 0; i < inputs_->size(); ++i) {
      Input* input = inputs_->at(i);
      if (input && input->source == &Processor::null_source_) {
        plug(source, i);
        return;
      }
    }

    // If there are no empty inputs, create another.
    Input* input = new Input();
    owned_inputs_.push_back(input);
    input->source = source;
    registerInput(input);
  }

  void Processor::plugNext(const Processor* source) {
    plugNext(source->output());
  }

  void Processor::unplugIndex(unsigned int input_index) {
    if (inputs_->at(input_index))
      inputs_->at(input_index)->source = &Processor::null_source_;
  }

  void Processor::unplug(const Output* source) {
    if (router_)
      router_->disconnect(this, source);

    for (unsigned int i = 0; i < inputs_->size(); ++i) {
      if (inputs_->at(i) && inputs_->at(i)->source == source)
        inputs_->at(i)->source = &Processor::null_source_;
    }
  }

  void Processor::unplug(const Processor* source) {
    if (router_) {
      for (int i = 0; i < source->numOutputs(); ++i)
        router_->disconnect(this, source->output(i));
    }
    for (unsigned int i = 0; i < inputs_->size(); ++i) {
      if (inputs_->at(i) && inputs_->at(i)->source->owner == source)
        inputs_->at(i)->source = &Processor::null_source_;
    }
  }

  ProcessorRouter* Processor::getTopLevelRouter() const {
    ProcessorRouter* top_level = 0;
    ProcessorRouter* current_level = router_;

    while (current_level) {
      top_level = current_level;
      current_level = current_level->router();
    }
    return top_level;
  }

  void Processor::registerInput(Input* input) {
    inputs_->push_back(input);

    if (router_ && input->source != &Processor::null_source_)
      router_->connect(this, input->source, inputs_->size() - 1);
  }

  void Processor::registerOutput(Output* output) {
    outputs_->push_back(output);
  }

  void Processor::registerInput(Input* input, int index) {
    while (inputs_->size() <= index)
      inputs_->push_back(0);

    inputs_->at(index) = input;

    if (router_ && input->source != &Processor::null_source_)
      router_->connect(this, input->source, index);
  }

  void Processor::registerOutput(Output* output, int index) {
    while (outputs_->size() <= index)
      outputs_->push_back(0);

    outputs_->at(index) = output;
  }
} // namespace mopo
