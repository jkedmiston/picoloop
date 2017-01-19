/*
 *  Parameter.h
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

#ifndef _PARAMETER_H
#define _PARAMETER_H

#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include "UpdateListener.h"

/**
 * @brief a Parameter holds a particular value for a slider, selector switch 
 * etc..
 *
 * This object is also responsible for keeping all audio/signal generation 
 * objects up to date with any controls which affect them.
 *
 * This object also easily enables non-linear relationships between the controls
 * (eg interface ParameterViews) and their effect on synthesis parameters. See
 * setType() for details.
 */

class Parameter {
public:
	enum ControlType
	{
		PARAM_DIRECT,	// controlValue = offset + base * value
		PARAM_EXP,		// controlValue = offset + base ^ value
		PARAM_POWER		// controlValue = offset + value ^ base
	};

					Parameter		(const std::string name = "unused", Param id = kAmsynthParameterCount,
									 float value = 0.0, float min = 0.0, float max = 1.0, float inc = 0.0,
									 ControlType = PARAM_DIRECT, float base = 1.0, float offset = 0.0,
									 const std::string label = "");

	// The raw value of this parameter. Objects in the signal generation 
	// path should not use this method, but getControlValue() instead.
	float			getValue		() const { return _value; }
	void			setValue		(float value);

	static float	valueFromString	(const std::string &str) {
		float value = 0.0f;
		// atof() and friends are affected by currently configured locale,
		// which can change the decimal point character.
		std::istringstream istr(str);
		static std::locale c_locale = std::locale("C");
		istr.imbue(c_locale); // be absolutely sure of the locale
		istr >> value;
		return value;
	}

	float			GetNormalisedValue	() const { return (getValue()-getMin())/(getMax()-getMin()); }
	void			SetNormalisedValue	(float val) { setValue (val*(getMax()-getMin())+getMin()); }

	// The control value for this parameter.
	// The control value is what the synthesis will use to get its values.
	inline float	getControlValue	() const { return _controlValue; }

	const std::string GetStringValue	() const { std::ostringstream o; o << _controlValue; return o.str(); }

	const std::string getName			() const { return _name; }
	Param			GetId			() const { return mParamId; }

	// UpdateListeners (eg one or more ParameterViews - part of the GUI) are 
	// notified and updated when this Parameter changes.
	void			addUpdateListener (UpdateListener& ul);
	void			removeUpdateListener (UpdateListener& ul);
	
	// min/max values apply for calls to setValue() not ControlValue
	float			getMin			() const { return _min; }
	float			getMax			() const { return _max; }

	// @return the increment value
	float			getStep			() const { return _step; }
	// @returns The number of discrete steps allowable in this Parameter.
	int				getSteps		() const { return _step ? (_max - _min) / _step : 0; }

	// Set this parameter to a random value (in it's allowable range)
	void			random_val		();

	// The label assocaited with this Parameter. (e.g. "seconds")
	const std::string getLabel		() const { return _label; }

	const char **   valueStrings    () const { return _valueStrings; }
	void            setValueStrings (const char **valueStrings) { _valueStrings = valueStrings; }

private:
	Param							mParamId;
	std::string						_name, _label;
	int								_controlMode;
	float							_value, _min, _max, _step, _controlValue, _base, _offset;
	std::vector<UpdateListener*>	_updateListeners;
	const char **					_valueStrings;
};

#endif
