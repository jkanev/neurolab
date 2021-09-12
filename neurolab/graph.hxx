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
#ifndef GRAPH_HXX
#define GRAPH_HXX

#include <neurolab/matrix.hxx>

/// The graph of a function.
/** A graph is the set of points p, with p = f(a), a element A, where A is the domain of f.
This class is a matrix (containing a numerical representation of the graph)
with dimension n+1, where n is the number of inputs to f. It is organized so that
file << graph appends gnuplot-plottable matrix notation to 'file'. If used as a matrix, you have (for a function
with one parameter) graph[i] is the i'th data point, and graph[i][0] is the input a_i, and graph[i][1] is the output f(a_i).  The main difference to the normal matrix class is that all mathematical operations (integrate, differentiate, multiply, etc.) only work on the output values (i.e. on graph[..][1]). 
\todo
 Only two-dimensional graphs are implemented yet. Change this. */
class Graph : public Matrix
{

public:
	
	Graph(int length)
	: Matrix(length, 2)
	{};
	
	Graph(int lengthX, int lengthY)
	: Matrix(lengthX, lengthY, 2)
	{
		cout << "3d graphs still missing some functions" << endl;
	};
	
	Graph(const Matrix &m) 
	: Matrix(m)
	{};

	Graph(const Graph &m) 
	: Matrix(m)
	{};

	virtual double getSum() {
		transpose();
		double d = operator[](1).getSum();
		transpose();
		return d;
	};
	
	/// Multiply with a scalar.
	/** Multiplies the graph with a scalar. A reference to the graph itself is returned for convenience, so that you can write things like m.multipliy(3).add(4) */
	Graph &multiply( double d ) {
		transpose();
		operator[](1).multiply(d);
		transpose();
		return *this;
	};

	/// Add a scalar.
	/** Adds a scalar to each element of the graph. A reference to the graph itself is returned for convenience, so that you can write things like m.add( a ).multiply( b ) */
	Graph& add( double d ) {
		transpose();
		operator[](1).add(d);
		transpose();
		return *this;
	};

	/// Exponential.
	/** Transforms each element into its exponential.  A reference to the graph itself is returned for convenience, so that you can write things like m.exp().multipliy( a ).add( b ) */
	Graph& exp() {
		transpose();
		operator[](1).exp();
		transpose();
		return *this;
	};

	/// Sqare root for each element.
	/** Transforms each element into its square root.  A reference to the graph itself is returned for convenience, so that you can write things like m.sqrt().multipliy( a ).add( b ) */
	Graph& sqrt() {
		transpose();
		operator[](1).sqrt();
		transpose();
		return *this;
	};

	/// Integrate.
	/** This function integrates over the last dimension of the graph. A cumulative sum is produced, where m'[0] = m[0], m'[1] = m[0]+m[1], m'[n] = m[0] + m[1] + ... + m[n]. Doing this on a differentiated graph, m.differentiate().integrate(), retrieves the original graph, shifted so that m[0] = 0.0, and adding the original m[0] restores the original graph. */
	Graph& integrate() {
		transpose();
		operator[](1).integrate();
		transpose();
		return *this;
	};

	/// Differentiate.
	/** This function differentiates over the last dimension of the graph. A difference-graph is produced, where m'[0] = 0.0, m'[1] = m[1]-m[0], m'[n] = m[n]-m[n-1]. Doing this on an integrated graph, m.integrate().differentiate(), restores the original graph, but m[0] will be lost (set to 0.0). */
	Graph& differentiate() {
		transpose();
		operator[](1).differentiate();
		transpose();
		return *this;
	};
	
	/// Calculate the inverse.
	/** This function just swaps x and y of the graph, and calls sortX() so that all x values
	are in ascending order afterwards. */
	Graph& inverse() {
		// swap x and y
		for (int i=0; i<nSize(0); ++i) {
			double d = (*this)[i][0].to_d();
			(*this)[i][0] = (*this)[i][1];
			(*this)[i][1] = d;
		}
		
		// sort new x values
		sortX();
		
		return *this;
	}
	
	/// Sort the points according to the x values
	/** This function is used by the inverse() function, after swapping x and y. It sorts all points
	of the graph, so that the x values of all points will be in ascending order. The currently
	implemented method is merge sort. */
	void sortX(int start=0, int end=-1) {
		if (end==-1)
			end = nSize(0)-1;
		if (end - start < 1)
			return;
		
		// devide list in half
		int leftStart = start;
		int leftEnd = start + ((end - start) >> 1);
		int rightStart = leftEnd + 1;
		int rightEnd = end;
		
		// sort each half
		sortX(leftStart, leftEnd);
		sortX(rightStart, rightEnd);
		
		// merge both halfs
		pair<double,double> *results = new pair<double,double>[end-start+1];
		int iLeft=leftStart, iRight=rightStart;
		
		for (int iResults=0; iResults<end-start+1; ++iResults) {
		
			// read values
			double leftX=-1, leftY=-2, rightX=-3, rightY=-4;
			bool rightFinished=false, leftFinished = false;
			if (iLeft <= leftEnd) {
				leftX = (*this)[iLeft][0].to_d();
				leftY = (*this)[iLeft][1].to_d();
			} else
				leftFinished = true;
			if (iRight  <= rightEnd) {
				rightX = (*this)[iRight][0].to_d();
				rightY = (*this)[iRight][1].to_d();
			} else
				rightFinished = true;
				
			// use next value from left half
			if ( !leftFinished && ( rightFinished ||  leftX < rightX) ) {
				results[iResults].first = leftX;
				results[iResults].second = leftY;
				iLeft++;
			}
			// or use next value from right half
			else if (!rightFinished) {
				results[iResults].first = rightX;
				results[iResults].second = rightY;
				iRight++;
			}
		}
		
		// write merged result to matrix
		for (int i=0; i<end-start+1; ++i) {
			(*this)[start+i][0] = results[i].first;
			(*this)[start+i][1] = results[i].second;
		}
		
		delete[] results;
	}
};

#endif
