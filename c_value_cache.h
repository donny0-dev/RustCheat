#pragma once

#include <vector>
#include <string>

template <typename type>
class c_cached_value
{
public:
	type original_value;
	type current_value;
private:

	c_cached_value(type _original)
	{
		this->original_value = _original;
	}

	type get_original() { return original_value; }
	void set_current(type set) { current_value = set; }

};