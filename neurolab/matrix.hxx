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

#ifndef __MATRIX_HXX_
#define __MATRIX_HXX_

#include <neurolab/physical.hxx>
#include <boost/shared_array.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>

using namespace std;
using namespace boost;

/// a matrix class
/** Quick matrix class to implement matrices which know their own dimension. Supports any dimension, slicing, subindexing, as well as the matlab-known funktions cumsum, cummult and convolve (not implemented yet). Dimensions 0 up to 4 have shortcuts. The data inside the matrix are physical values and therefore have a unit (use Physical::getUnit() from the Physical class), and each dimension has a separate unit as well (use Matrix::getUnit(n)). Example: if you have voltage data, which you collected over time and length, you have a two-dimensional matrix; the matrix unit should be voltage, and one dimension should have cm, mm or whatever, and the other dimension ms, s etc. 
\todo
Implement Matrix multiplication and the (Matlab-known) functions cumsum, cummult and convolve for n-dimensional matrixes.*/
class Matrix: public Physical
{
private:
    bool bReference;	 ///< wether we're a reference or not
    double *pData;	 ///< the data
	shared_array<double> matrixParent; ///< pointer to parent data, if this is a reference. Used to inc the refcount of the parent if the data  of this matrix is only part of the data of the parent matrix (i.e. both pointers are different)
	int nData; ///< size of pData
    int *pSize;	 ///< shape of matrix
    int *pMult;	 ///< multipliers for data retrieval
	Physical *pPhysicals; ///< physical name for each dim.
    int nMatrixDimension;	 ///< which dimensionality
    string sMatrixName;	 ///< a user-defined name
	
	// private constructor
	Matrix(double *data, shared_array<double> parent,  int* sizes, int dimension);
	
	// initialize the multipliers
	void init(bool createStorage = true);
	
	// check shape equallity with another matrix
	bool bShapeEqual( const Matrix &m );

public:
	/// default constructor: constructs a zero-dimensional matrix (a scalar).
	Matrix();
	/// constructs a one-dimensional matrix
	Matrix(int i);
	/// constructs a two-dimensional matrix
	Matrix(int i, int j);
	/// constructs a three-dimensional matrix
	Matrix(int i, int j, int k);
	/// constructs a four-dimensional matrix
	Matrix(int i, int j, int k, int l);
	/// constructs an n-dimensional matrix
	Matrix(int dimensions, int* sizes);
	
	/// destructs the matrix
	virtual ~Matrix();
	
	/// set name
	Matrix &setName( const string& );
	/// get name
	string sName();
	
	/// Get number of dimensions
	int nDimension();
	
	/// Get size for dimension
	int nSize(int n);
	
	/// Unit of dimension n
	Unit getUnit(int n);
	
	/// Set physical dimension of dimension n
	/** Set a dimension n to a physical dimension, like f.i. 'acceleration'. If the matrix is plotted with a Display object, physical name and unit are shown on the appropriate axis. 
	\param n Dimension
	\param p Physical dimension*/
	void setPhysical(int n, Physical p);
	
	/// Set main physical dimension
	/** Set the data inside the matrix to a physical dimension, like 'capacity'.  If the matrix is plotted with a Display object, physical name and unit are shown on the appropriate axis. 
	\param p Physical dimension */
	void setPhysical(Physical p);
	
	/// Get physical dimension for dimension n
	/** Get the physical dimension of a dimension of the matrix.
	\param n The dimension */
	Physical pPhysical(int n);
	
	/// retrieve slices or values
	/** (Note: these are references to the original matrix data.) */
	Matrix operator[](int i);
	const Matrix read(int i) const;

	/// remove one or more dimensions
	/** input vector contains removal information: -1 means "do not remove", positive integer n means "remove dimension n". Example: if m is a (2,4,3) matrix, then m.remove([-1,-1,2]) will return a (2,4,2) matrix, where all m[x,x,2] data has been removed.*/
	Matrix &remove(vector<int>);
	
	/// increment a zero-dimensional matrix
	void operator++(int i);
	
	/// assignment
	/** This assigns the elements of the matrix. For assigning ranges use a for-loop. */
	void operator=(double d);
	
	/// addings
	/** Adds another matrix. Shapes must be equal, otherwise nothing happens. */
	Matrix &operator+=(const Matrix &);
	
	/// retrieve ranges - not implemented yet!
	/** (Note: these are references to the original matrix data.) Retrieves a matrix from i to j. Example: a = Matrix(6,6,6); (a will be a 6x6x6-Matrix). To get something like a[2..3][4..5][1..2] you write: a(2,3)(4,5)(1,2). */
	Matrix operator()(int i, int j);

	/// convert to double
	/** This is mainly useful for zero-dimension matrixes. */
	double to_d();
	
	/// Sum of elements
	/** This returns a sum of all elements. */
	virtual double getSum();
	
	/// get maximal value
	double getMaxValue();
	
	/// get minimal value
	double getMinValue();
	
	/// Replaces NaNs.
	/** Replace all NaN-valued members in the matrix with a given value. 
	\param d Value which should replace any NaN-valued member. */
	void replaceNan( double d );
	
	/// Multiply with a scalar.
	/** Multiplies the matrix with a scalar. A reference to the matrix itself is returned for convenience, so that you can write things like m.multipliy(3).add(4) */
	virtual Matrix &multiply( double d );
	
	/// Add a scalar.
	/** Adds a scalar to each element of the matrix. A reference to the matrix itself is returned for convenience, so that you can write things like m.add( a ).multiply( b ) */
	virtual Matrix& add( double d );
	
	/// Exponential.
	/** Transforms each element into its exponential.  A reference to the matrix itself is returned for convenience, so that you can write things like m.exp().multipliy( a ).add( b ) */
	virtual Matrix& exp();
	
	/// Sqare root for each element.
	/** Transforms each element into its square root.  A reference to the matrix itself is returned for convenience, so that you can write things like m.sqrt().multipliy( a ).add( b ) */
	virtual Matrix& sqrt();
	
	/// Transpose matrix.
	/** This changes the matrix data, so that m[x1][x2][x3]...[xn] becomes m[xn][xn-1][xn-2]...[x1].  A reference to the matrix itself is returned for convenience, so that you can write things like m.transpose().multipliy( a ).add( b ) */
	Matrix& transpose();
	
	/// Integrate.
	/** This function integrates over the last dimension of the matrix. A cumulative sum is produced, where m'[0] = m[0], m'[1] = m[0]+m[1], m'[n] = m[0] + m[1] + ... + m[n]. Doing this on a differentiated matrix, m.differentiate().integrate(), retrieves the original matrix, shifted so that m[0] = 0.0, and adding the original m[0] restores the original matrix. */
	virtual Matrix& integrate();
	
	/// Differentiate.
	/** This function differentiates over the last dimension of the matrix. A difference-matrix is produced, where m'[0] = 0.0, m'[1] = m[1]-m[0], m'[n] = m[n]-m[n-1]. Doing this on an integrated matrix, m.integrate().differentiate(), restores the original matrix, but m[0] will be lost (set to 0.0). */
	virtual Matrix& differentiate();
	
	/// append matrix to a stream
	friend ostream& operator<<(ostream& s, const Matrix& m);
	
	/// subract two matrices
	friend Matrix operator-(const Matrix &m0, const Matrix &m1);
	
	/// add two matrices
	friend Matrix operator+(const Matrix &m0, const Matrix &m1);
	
	/// assignment
	Matrix &operator=(const Matrix &m);
	
	/// Copy constructor
	/** This creates a shallow copy, where the new Matrix takes over the memory, i.e. copy() is called with deep=false. */
	Matrix( const Matrix& m );
	void copy( const Matrix& m, bool deep=true );
	
	friend class MatrixDataDeleter;
};

class MatrixDataDeleter
{
private:
    bool bReference;
public:
	MatrixDataDeleter( Matrix *m ) {
        bReference = m->bReference;
	}
	void operator()(double *data) {
        if (!bReference)
            delete[] data;
    }
};


#endif
