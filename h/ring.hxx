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

#ifndef __RING_HXX
#define __RING_HXX

/// A simple ring buffer.
/** 
All functions are inline, since functions where so short, there was no need for an extra .cc file. */
using namespace std;

template<class T>
class Ring
{
public:
	
	/// New ring
	/** Creates a new ring. @param l Length of ring. */
	Ring(int l) {
		ringData = new T[l];
		ringSize = l;
		clear();
	};

	/// delete ring
	~Ring() {
		delete[] ringData;
	};

	/// clear ring
	/** This writes initializes the ring with 0.0 and sets the initialized property to false. */
	void clear() {
		ringIsInitialised = false;
		init((T)0.0);
		ringCurrentIndex = 0;
	};
	
	/// initialise ring with value
	/** Write a value into every element of the ring. @param d Value to use. */
	void init(T d) {
	for(int i=0; i<ringSize; i++)
		ringData[i] = d;
	};
 
	/// next data for ring
	/** Insert a new data point and proceed ring pointer. @param d Data to store. */
	void next(T d) {
		if(++ringCurrentIndex >= ringSize) {
			ringCurrentIndex = 0;
			ringIsInitialised = true;
		}
		ringData[ringCurrentIndex] = d;
	};

	/// is ring initialized?
	/** returns true if all parts of the ring have been filled (by prepareNextState()) at least once. */
	bool isInitialized() {
		return ringIsInitialised;
	};
	
	/// get data point
	/** Retrieve data. @param index Index of data to use.
	The index is always relative to the current position, and can be negative. */
	const T &operator[](const int &index) const {
		int i = ringCurrentIndex+index;
		while (i >= ringSize)
			i -= ringSize;
		while (i < 0)
			i += ringSize;
		return ringData[i];
	};

	/// add two rings
	/** @param r The ring which is to be added to this one. As a ring has infinite length, no length check is needed. If the added Ring is shorter than this ring, it will be added repeatedly, if it is longer than this ring, the first part of it will be added. */
	void operator+=(const Ring<T> &r) {
		for(int i=0; i<ringSize; i++)
		ringData[i] += r[i];
	};

	/// add an element to all elements
	/** @param n The element which is to be added to the ring. */
	void operator+=(T n) {
		for(int i=0; i<ringSize; i++)
			ringData[i] += n;
	};
	
	/// get length
	/** @returns the length of the ring */
	int length() {
		return ringCurrentIndex;
	};
   
protected:
	T *ringData;  // the data
	int ringCurrentIndex;          // the current point
	int ringSize;     // the length
	bool ringIsInitialised;
};

#endif

