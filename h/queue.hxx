/* Copyright Information
__________________________________________________________________________

Copyright (C) 2005 Jacob Kanev

This file is part of NeuroLab.

NeuroLab is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA.
__________________________________________________________________________
*/

#ifndef __QUEUE_HXX
#define __QUEUE_HXX

#include "ring.hxx"
#include <sys/types.h>

using namespace std;

/// Small queue.
/** Class for storing basic data types in a simple queue. Push and pop work via << and >>, and += is defined. If you want something better, use the STL queue. */
template<typename T>
class Queue : public Ring<T>
{
private:
	int queueSize;
	
public:
	Queue(int maxSize = 100) : Ring<T>(maxSize) {
		queueSize = 0;
	};
	
	~Queue() {};
	
	uint size() {
		return queueSize;
	}

	void operator<<(T input) {
		if (queueSize < Ring<T>::ringSize) {
			++queueSize;
			this->next(input);
		}
	}
	
	void operator>>(T &output) {
		if (queueSize) {
			output = first();
			--queueSize;
		}
	}
	
	void operator+=(T value) {
		for (int i = Ring<T>::ringCurrentIndex-queueSize+1; i<=Ring<T>::ringCurrentIndex; ++i) {
			int index = i;
			while (index >= Ring<T>::ringSize)
				index -= Ring<T>::ringSize;
			while (index < 0)
				index += Ring<T>::ringSize;
			Ring<T>::ringData[index] += value;
		}
	}
	
	T first() {
		return Ring<T>::operator[](1-queueSize);
	}

	/// get data point
	/** Retrieve data. @param index Index of data to use.
	 *	The index is always relative to the current position, and can be negative. */
	const T &operator[](const int &index) const {
		return Ring<T>::operator[](1-queueSize+index);
	};
	
};

#endif
