/*
 *  MidiController.h
 *
 *  Copyright (c) 2001-2012 Nick Dowell
 *
 *  This file is part of amsynth.
 *
 *  amsynth is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  amsynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with amsynth.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _MIDICONTROLLER_H
#define _MIDICONTROLLER_H

#include "Config.h"
#include "PresetController.h"
#include "Parameter.h"
#include "Thread.h"


#define MAX_CC 128

typedef unsigned char uchar;

class MidiDriver;

class MidiEventHandler
{
public:
	virtual ~MidiEventHandler() {}
	
	virtual void HandleMidiNoteOn(int /*note*/, float /*velocity*/) {}
	virtual void HandleMidiNoteOff(int /*note*/, float /*velocity*/) {}
	virtual void HandleMidiPitchWheel(float /*value*/) {}
	virtual void HandleMidiPitchWheelSensitivity(uchar semitones) {}
	virtual void HandleMidiAllSoundOff() {}
	virtual void HandleMidiAllNotesOff() {}
	virtual void HandleMidiSustainPedal(uchar /*value*/) {}
	virtual void HandleMidiPan(float left, float right) {}
};

class MidiController
{
public:
	MidiController( Config & config );
	virtual ~MidiController();

	void	setPresetController	(PresetController & pc) { presetController = &pc; }
	void	SetMidiEventHandler(MidiEventHandler* h) { _handler = h; }
	
	void	HandleMidiData(const unsigned char *bytes, unsigned numBytes);

	void	clearControllerMap();
	void	loadControllerMap();
	void	saveControllerMap();

	int		getControllerForParameter(int paramId);
	void	setControllerForParameter(int paramId, int cc);

	Parameter & getLastControllerParam() { return last_active_controller; };

	int		get_midi_channel	() { return channel; }
	void	set_midi_channel	( int ch );
	
	void	setMidiDriver		(MidiDriver *driver) { _midiDriver = driver; }
	int     sendMidi_values		();
	void	send_changes		(bool force=false);

	void	timer_callback		();

private:
	void dispatch_note(unsigned char ch,
		       unsigned char note, unsigned char vel);
    void controller_change(unsigned char controller, unsigned char value);
    void pitch_wheel_change(float val);

    PresetController *presetController;
	Config *config;
    unsigned char status, data, channel;
	Parameter last_active_controller;
	unsigned char _midi_cc_vals[MAX_CC];
	MidiEventHandler* _handler;
	MidiDriver *_midiDriver;
	unsigned char _rpn_msb, _rpn_lsb;

	int _cc_to_param_map[MAX_CC];
	int _param_to_cc_map[kAmsynthParameterCount];

	bool _config_needs_save;
};
#endif
