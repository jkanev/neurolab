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


#include "../h/matrix.hxx"
#include <algorithm>

Matrix::Matrix(double *data, shared_array<double> parent, int* sizes, int dimension) : sMatrixName("reference")
{
	bReference = true;
	pData = data;
	matrixParent = parent;
	pSize = sizes;
	nMatrixDimension = dimension;
	init(false);
}
	
// initialize the multipliers
void Matrix::init(bool createStorage) 
{
	nData = 1;
    int dimension = nMatrixDimension ? nMatrixDimension : 1;
    pMult = new int[ulong(dimension)];
    pPhysicals = new Physical[ulong(dimension)]();
    for (ulong i=ulong(dimension)-1; i+1; i--) {
		pMult[i] = nData;
		nData *= pSize[i];
	}
	if (createStorage) {
        pData = new double[ulong(nData)];
		matrixParent = shared_array<double>(pData, MatrixDataDeleter(this));
	}
}

//________________________________________________________
// constructors
/// default constructor: constructs a zero-dimensional matrix (scalar).

Matrix::Matrix() : sMatrixName("data") 
{ 
	nMatrixDimension = 0;
	pSize = new int[1];
	pSize[0] = 1;
	bReference = false;
	init();
	pData[0] = 0.0;
}

/// copy constructor
Matrix::Matrix( const Matrix &m ) : sMatrixName( m.sMatrixName )
{
	// do a shallow copy so we don't shift memory each time a fcn returns a matrix
    copy( m, true );
}

void Matrix::copy( const Matrix &m, bool deep )
{
	// copy main variables
	sMatrixName = m.sMatrixName;
	nMatrixDimension = m.nMatrixDimension;
	nData = m.nData;
	bReference = deep ? false : true;
	physicalDescription = m.physicalDescription;
	sMatrixName = m.sMatrixName;
	physicalUnit = m.physicalUnit;
	
	pPhysicals = new Physical[nMatrixDimension]();
	for( int i=0; i<nMatrixDimension; i++)
		pPhysicals[i] = Physical(m.pPhysicals[i]);

	pSize = new int[nMatrixDimension];
	for( int i=0; i<nMatrixDimension; i++ )
		pSize[i] = m.pSize[i];

	pMult = new int[nMatrixDimension];
	for( int i=0; i<nMatrixDimension; i++ )
		pMult[i] = m.pMult[i];		

	// copy all content if deep copy is wanted
	if( !bReference ) {
		pData = new double[nData];
		matrixParent =  shared_array<double>(pData, MatrixDataDeleter(this));
		for( int i=0; i<nData; i++ )
			pData[i] = m.pData[i];
	}
	// else copy only pointers
	else {
		pData = m.pData;
		matrixParent = m.matrixParent;
	}
}

/// constructs a one-dimensional matrix
Matrix::Matrix(int i) : sMatrixName("data") 
 { 
	nMatrixDimension = 1;
	pSize = new int[nMatrixDimension];
	pSize[0] = i;
	bReference = false;
	init();
}

/// constructs a two-dimensional matrix
Matrix::Matrix(int i, int j) : sMatrixName("data") 
 { 
	nMatrixDimension = 2;
	pSize = new int[nMatrixDimension];
	pSize[0] = i;
	pSize[1] = j;
	bReference = false;
	init();
}

/// constructs a three-dimensional matrix
Matrix::Matrix(int i, int j, int k) : sMatrixName("data") 
 { 
	nMatrixDimension = 3;
	pSize = new int[nMatrixDimension];
	pSize[0] = i;
	pSize[1] = j;
	pSize[2] = k;
	bReference = false;
	init();
}

/// constructs a four-dimensional matrix
Matrix::Matrix(int i, int j, int k, int l) : sMatrixName("data") 
 { 
	nMatrixDimension = 4;
	pSize = new int[nMatrixDimension];
	pSize[0] = i;
	pSize[1] = j;
	pSize[2] = k;
	pSize[3] = l;
	bReference = false;
	init();
}

/// constructs an n-dimensional matrix
Matrix::Matrix(int dimensions, int* sizes) : sMatrixName("data") 
 { 
	nMatrixDimension = dimensions;
	pSize = new int[nMatrixDimension];
	for(int i=0; i<nMatrixDimension; i++)
		pSize[i] = sizes[i];
	bReference = false;
	init();
}

/// destructs the matrix
Matrix::~Matrix()
{
	delete[] pSize;
	delete[] pMult;
	delete[] pPhysicals;
}

int Matrix::nDimension()
{
	return nMatrixDimension;
}

int Matrix::nSize(int n)
{
	if( n>=nMatrixDimension ) {
		cout << "Matrix::nSize(int): int ("
			<< n
			<< ") larger than number of dimensions." << endl;
		return -1;
	}
	return pSize[n];
}
			
Physical Matrix::pPhysical(int n)
{
	if( n<nMatrixDimension )
		return pPhysicals[n];
	else {
		// cout << "Matrix::getUnit(int n): error: n > matrix dimensions" << endl;
		return *(new Physical());
	}
}

void Matrix::setPhysical(int n, Physical p)
{
	pPhysicals[n] = p;
}

void Matrix::setPhysical(Physical p)
{
	physicalUnit = p.getUnit();
	physicalDescription = p.getDescription();
}
	
Matrix &Matrix::remove(vector<int> removal)
{
	// create new data store and new shape vector
	int *newShape = new int[nMatrixDimension];
	int newSize = 1;
	for (int i=0; i<nMatrixDimension; ++i) {
		if (removal[i] >= 0 && pSize[i]>1)
			newShape[i] = pSize[i]-1;
		else
			newShape[i] = pSize[i];
		newSize *= newShape[i];
	}
	double *newData = new double[newSize];
	
	// vector with current index coordinates
	int *index = new int[nMatrixDimension];
	for (int i=0; i<nMatrixDimension; ++i)
		index[i] = 0;
	
	// copy each data point
	int n=0; // single index into new data
	for (int i=0; i<nData; ++i) {
		
		// compare index vector to removal vector
		bool copyData = true;
		for (int r=0; r<nMatrixDimension; ++r)
			if (removal[r] != -1 && index[r] == removal[r]) {
				copyData = false;
				break;
			}
		
		// copy if index does not match removal vector
		if (copyData)
			newData[n++] = pData[i];
		
		// increment  index vector
		index[nMatrixDimension-1]++;
		for (int k=nMatrixDimension-1; k; --k)
			if (index[k] >= pSize[k]) {
				index[k] = 0;
				index[k-1]++;
			}		
	}
	
	// assign new data
	delete[] pMult;
	delete[] pPhysicals;
	delete[] pSize;
	pSize = newShape;
	init();
	
	return *this;
}

Matrix Matrix::operator[](int i)
{
    uint dimension = uint(nMatrixDimension==1 ? 1 : nMatrixDimension-1);
    int *sizes = new int[dimension];
    sizes[0] = 1;
    for (uint s=1; s<uint(nMatrixDimension); s++)
		sizes[s-1] = pSize[s];
    return Matrix( &(pData[i*pMult[0]]), matrixParent, sizes, dimension );
}

const Matrix Matrix::read(int i) const
{
    uint dimension = uint(nMatrixDimension==1 ? 1 : nMatrixDimension-1);
    int *sizes = new int[dimension];
    sizes[0] = 1;
    for (uint s=1; s<uint(nMatrixDimension); s++)
         sizes[s-1] = pSize[s];
    return Matrix( &(pData[i*pMult[0]]), matrixParent, sizes, dimension );
}

void Matrix::operator=(double d)
{
    for(int i=0; i<nData; i++)
        pData[i] = d;
}

Matrix Matrix::operator()(int i, int j)
{
	return Matrix();
}

void Matrix::operator++(int i)
{
    for(int n=0; n<nData; n++)
        pData[n] += i;
}

double Matrix::to_d()
{
	return pData[0];
}
	
Matrix &Matrix::setName( const string& name )
{
	sMatrixName = name;
	return *this;
}

string Matrix::sName()
{
	return string(sMatrixName);
}

// replace nans
void Matrix::replaceNan( double d )
{
/*	int n = 0;
	for(int i=0; i<nMatrixDimension; i++) 
		n += pSize[i];
	for(int i=0; i<n; i++) {
		union { double d; long long unsigned int n;} val;
		long long unsigned int mask = 0xffffffff;
		long long unsigned int nan = 0x80000000;
		val.d = pData[i];
		if( (val.n & mask) == nan )
			pData[i] = d;
	}*/
}
	
// sum
double Matrix::getSum()
{
	double result = 0.0;
	
	for(int i=0; i<nData; i++)
		result += pData[i];

	return result;
}

// max
double Matrix::getMaxValue()
{
	double result = pData[0];
	
	for(int i=0; i<nData; i++)
		if (pData[i] > result)
			result = pData[i];

	return result;
}

// min
double Matrix::getMinValue()
{
	double result = pData[0];
	
	for(int i=0; i<nData; i++)
		if (pData[i] < result)
			result = pData[i];

	return result;
}

Matrix &Matrix::multiply( double d )
{
	for(int i=0; i<nData; i++)
		pData[i] *= d;
	return *this;
}


Matrix &Matrix::add( double d )
{
	for(int i=0; i<nData; i++)
		pData[i] += d;
	return *this;
}


Matrix &Matrix::exp()
{
	for(int i=0; i<nData; i++)
		pData[i] = std::exp(pData[i]);
	return *this;
}


Matrix &Matrix::sqrt()
{
	for(int i=0; i<nData; i++)
		pData[i] = std::sqrt(pData[i]);
	return *this;
}


Matrix &Matrix::transpose()
{
	int *indexes = new int[nMatrixDimension]; // array with indexes
	double *newData = new double[nData]; // array for re-arranged values
	
	// array with new shape
	int *newSize = new int[nMatrixDimension]; 
	for( int i=0; i<nMatrixDimension; i++ )
		newSize[i] = pSize[nMatrixDimension- i-1];
	
	// array with new multipliers
	int *newMult = new int[nMatrixDimension];
	int m = 1;
	for(int i=nMatrixDimension-1; i+1; i--) {
		newMult[i] = m;
		m *= newSize[i];
	}
	
	for( int i=0; i<nData; i++ ) {
		// find index of data i
		int remainder = i;
		for( int j=0; j<nMatrixDimension; j++) {
			indexes[j] = remainder / pMult[j];
			remainder -= indexes[j] * pMult[j]; // modulo :-)
		}
		// find new offset by re-interpreting index
		int newOffset = 0;
		for( int j=0; j<nMatrixDimension; j++)
			newOffset += indexes[nMatrixDimension-j-1] * newMult[j];
		// write into new position
		newData[ newOffset ] = pData[i];
	}	
	
	// save new data
	delete[] pMult;
	pMult = newMult;
	delete[] pSize;
	pSize = newSize;
	pData = newData;
	matrixParent = shared_array<double>(newData, MatrixDataDeleter(this));
	
	// bye.
	return *this;
}


Matrix &Matrix::differentiate()
{
	int n = nMatrixDimension-1;
	bool done = false;
	int *indices = new int[n];
	for (int i=0; i<n; ++i)
		indices[i] = 0;
	
	while (!done) {
		
		// increase indices
		if (!n)
			done = true;
		else for (int dim=n-1; dim+1; --dim) {
			indices[dim]++;
			if (indices[dim] == pSize[dim]) {
				indices[dim] = 0;
				if (!dim) 
					done = true;
			} else
				break;
		}
		
		// create offset from indices
		int offset = 0;
		for (int i=0; i<n; ++i)
			offset += indices[i]*pMult[i];
		
		// differentiate last dimension of this offset
		double last = pData[offset];
		for (int i=1; i<pSize[n]; ++i) {
			double next = pData[offset+i] - last;
			last = pData[offset+i];
			pData[offset+i] = next;
		}
	}
	
	delete[] indices;
	return *this;
}

Matrix &Matrix::integrate()
{
	int n = nMatrixDimension-1;
	bool done = false;
	int *indices = new int[n];
	for (int i=0; i<n; ++i)
		indices[i] = 0;
	
	while (!done) {
		
		// increase indices
		if (!n)
			done = true;
		else for (int dim=n-1; dim+1; --dim) {
			indices[dim]++;
			if (indices[dim] == pSize[dim]) {
				indices[dim] = 0;
				if (!dim) 
					done = true;
			} else
				break;
		}
		
		// create offset from indices
		int offset = 0;
		for (int i=0; i<n; ++i)
			offset += indices[i]*pMult[i];
		
		// integrate last dimension of this offset
		for (int i=1; i<pSize[n]; ++i)
			pData[offset+i] += pData[offset + i-1];
	}
	
	delete[] indices;
	return *this;
}

bool Matrix::bShapeEqual( const Matrix &m )
{
	bool ok = true;
	// check shape
	if( nMatrixDimension != m.nMatrixDimension ) {
		cout << "error: dimensionality mismatch in matrix operation " << endl;
		ok = false;
	} else {
		for(int i=0; i<nMatrixDimension; i++)
			if( pSize[i] != m.pSize[i] ) {
				cout << "error: size mismatch at dimension " << i << " in matrix operation " << endl;
				ok = false;
			}
	}
	return ok;
}

Matrix &Matrix::operator=( const Matrix &m )
{
	// self-assignment
	if( &m == this ) return *this;
	
	// if we're a reference, dimensionality must be ok
	if( bReference ) {
		// copy data
		if( bShapeEqual(m) )
			for(int i=0; i<nData; i++)
				pData[i] = m.pData[i];
	} else 
		// use copy if we're no reference
		copy( m );
	return *this;
}

Matrix &Matrix::operator+=(const Matrix&m)
{
	if( bShapeEqual(m) )
		for( int i=0; i<nData; i++ )
			pData[i] += m.pData[i];
	return *this;
}

Matrix operator-(const Matrix &m0, const Matrix &m1)
{
	Matrix *m = new Matrix(m0);
	m->sMatrixName = m0.sMatrixName + string(" - ") + m1.sMatrixName;
	if( m->bShapeEqual(m1) )
		for( int i=0; i<m->nData; i++ )
			m->pData[i] -= m1.pData[i];
	return *m;
}

Matrix operator+(const Matrix &m0, const Matrix &m1)
{
	Matrix *m = new Matrix(m0);
	m->sMatrixName = m0.sMatrixName + string(" + ") + m1.sMatrixName;
	return m->operator+=( m1 );
}


ostream& operator<<(ostream& s, const Matrix& m)
{	
	switch( m.nMatrixDimension ) {
	case 0:
		s  << m.pData[0] << endl;
		break;
	case 1:
		for( int i=0; i<m.pSize[0]; i++ )
			s << m.pData[i] << ' ';
		s << endl;
		break;
	default:
		// start recursion
		for(int i=0; i<m.pSize[0]; i++) 
			s << m.read(i);
		s << endl;			
		break;
	}
	return s;
}
	

