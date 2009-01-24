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

#include "../h/function.hxx"

Function::Function( const Function &f )
{
	copy(f);
}

Function::Function( FunctionType type )
{
	init();
	setType(type);
}

Function::Function(double d)
{
	init();
	xSrc = 0;
	dValue = d;
	setType(CONSTANT);
}	

Function::Function( Function& left, Function& right, FunctionType type )
{
	init();
	xLeft = &left;
	xRight = &right;
	dXMin = right.dXMin;
	dXMax = right.dXMax;
	dXStep = right.dXStep;
	dYMin = right.dYMin;
	dYMax = right.dYMax;
	dYStep = right.dYStep;
	setType( type );
}

Function::Function( Function& left, Function& right, Function& plus, FunctionType type )
{
	init();
	xLeft = &left;
	xRight = &right;
	xPlus = &plus;
	dXMin = right.dXMin;
	dXMax = right.dXMax;
	dXStep = right.dXStep;
	dYMin = right.dYMin;
	dYMax = right.dYMax;
	dYStep = right.dYStep;
	setType(type);
}

Function::Function( Function& right, FunctionType type )
{
	init();
	xRight = &right;
	dXMin = right.dXMin;
	dXMax = right.dXMax;
	dXStep = right.dXStep;
	dYMin = right.dYMin;
	dYMax = right.dYMax;
	dYStep = right.dYStep;
	setType(type);
}

Function::Function( double &d, string name, FunctionType type )
{
	init();
	switch (type) {
		case VARIABLE:
			xSrc = &d;
			dValue = 0.0;
			break;
		case CONSTANT:
		default:
			xSrc = 0;
			dValue = d;
			break;
		}
	// bValueValid = false;
	setType(type);
	sName = name;
}

Function::Function( double &d, FunctionType type )
{
	init();
	switch (type) {
		case VARIABLE:
			xSrc = &d;
			dValue = 0.0;
			break;
		case CONSTANT:
		default:
			xSrc = 0;
			dValue = d;
			break;
		}
	// bValueValid = false;
	setType(type);
}

Function::Function( Function &right, Matrix *m, FunctionType type )
{
	init();
	xRight = &right;
	dXMin = right.dXMin;
	dXMax = right.dXMax;
	dXStep = right.dXStep;
	dYMin = right.dYMin;
	dYMax = right.dYMax;
	dYStep = right.dYStep;
	switch (type) {
		case X_TABLE:
			xTable = m;
			break;
		default:
			xTable = 0;
	}
	setType(type);
}

void Function::init()
{
	oLog = &cout;
	eType = EMPTY;
	xSrc = 0;
	xLeft = 0;
	xRight = 0;
	xPlus = 0;
	xTable = 0;
	xFunction = 0;
	dValue = 0.0;
	dLastValue = 0.0;
	dXMin = -10.0;
	dXMax = 10.0;
	dXStep = 0.1;
	dYMin = -10.0;
	dYMax = 10.0;
	dYStep = 0.1;
	dXInput = 0.0;
	dNextXInput = 0.0;
	dYInput = 0.0;
	dNextYInput = 0.0;
	bValueValid = false;
	bVerbose = false;
	functionUseCount = 0;
}

void Function::copy(const Function &f)
{
	eType = f.eType;
	xLeft = f.xLeft;
	if (xLeft)
		xLeft->functionUseCount++;
	xRight = f.xRight;
	if (xRight)
		xRight->functionUseCount++;
	xPlus = f.xPlus;
	if (xPlus)
		xPlus->functionUseCount++;
	xTable = f.xTable;
	xSrc = f.xSrc;
	xFunction = f.xFunction;
	dValue = f.dValue;
	dLastValue = f.dLastValue;
	dXMin = f.dXMin;
	dXMax = f.dXMax;
	dXStep = f.dXStep;
	dYMin = f.dYMin;
	dYMax = f.dYMax;
	dYStep = f.dYStep;
	dXInput = f.dXInput;
	dNextXInput = f.dNextXInput;
	dYInput = f.dYInput;
	dNextYInput = f.dNextYInput;
	bValueValid = f.bValueValid;
	bVerbose = f.bVerbose;
	sName = f.sName;
	functionUseCount = f.functionUseCount;
}

Function::~Function()
{
	if (xLeft) {
		xLeft->functionUseCount--;
		if (xLeft->functionUseCount)
			delete xLeft;
	}
	if (xRight) {
		xRight->functionUseCount--;
		if (xRight->functionUseCount)
			delete xRight;
	}
	if (xPlus) {
		xPlus->functionUseCount--;
		if (xPlus->functionUseCount)
			delete xPlus;
	}
}

Function &Function::setPhysical(string s, Unit u)
{
	((Physical *)this)->setDescription(s);
	setUnit(u);
	return *this;
}

Function &Function::setXRange(double min, double max, double step)
{
	dXMin = min;
	dXMax = max;
	dXStep = step;
        if (xLeft) xLeft->setXRange(min, max, step);
        if (xRight) xRight->setXRange(min, max, step);
        if (xPlus) xPlus->setXRange(min, max, step);
	return *this;
}

Function &Function::setYRange(double min, double max, double step)
{
	dYMin = min;
	dYMax = max;
	dYStep = step;
        if (xLeft) xLeft->setYRange(min, max, step);
        if (xRight) xRight->setYRange(min, max, step);
        if (xPlus) xPlus->setYRange(min, max, step);
	return *this;
}

void Function::setType(FunctionType type)
{
	eType = type;
	string left, right, plus;
	stringstream name;
	switch (type) {
	case VARIABLE: 
		xFunction = &Function::id;
		sName = "var";
		break;
	case CONSTANT:
		xFunction = &Function::constant;
		name << dValue;
		sName = name.str();
		break;
	case PLUS: 
		xFunction = &Function::plus; 
		sName = xLeft->getName() + " + " + xRight->getName();
		physicalDescription = xRight->getPhysicalDescription();
		physicalUnit = xRight->getUnit();
		physicalUnit += xLeft->getUnit();
		break;
		
	case MINUS: 
		xFunction = &Function::minus; 
		sName = xLeft->getName() + " - " + xRight->getName();
		physicalDescription = xRight->getPhysicalDescription();
		physicalUnit = xRight->getUnit();
		physicalUnit -= xLeft->getUnit();
		break;
		
	case NEG:
		xFunction = &Function::neg;
		if (xRight->eType == PLUS || xRight->eType==MINUS)
			sName = "- (" + xRight->getName() + ")";
		else sName =  "-" + xRight->getName();
		physicalDescription = xRight->getPhysicalDescription();
		physicalUnit = xRight->getUnit();
		break;
		
	case TIMES:
		xFunction = &Function::times;
		if (xRight->eType == PLUS || xRight->eType==MINUS)
			right = "(" + xRight->getName() + ")";
		else right = xRight->getName();
		if (xLeft->eType == PLUS || xLeft->eType==MINUS)
			left = "(" + xLeft->getName() + ")";
		else left = xLeft->getName();
		sName = left + " * " + right;
		physicalDescription = xRight->getPhysicalDescription();
		physicalUnit = xRight->getUnit();
		physicalUnit *= xLeft->getUnit();
		break;
		
	case OVER:
		xFunction = &Function::over;
		if (xRight->eType == PLUS || xRight->eType==MINUS)
			right = "(" + xRight->getName() + ")";
		else right = xRight->getName();
		if (xLeft->eType == PLUS || xLeft->eType==MINUS)
			left = "(" + xLeft->getName() + ")";
		else left = xLeft->getName();
		sName = left + " / " + right;
		physicalDescription = xRight->getPhysicalDescription();
		physicalUnit = xLeft->getUnit();
		physicalUnit /= xRight->getUnit();
		break;
		
	case EXP:
		xFunction = &Function::exponential;
		sName = "exp(" + xRight->getName() + ")";
		physicalUnit = xRight->getUnit();
		physicalUnit.dimensionless();
		break;
		
	case LN:
		xFunction = &Function::lognat;
		sName = "ln(" + xRight->getName() + ")";		
		physicalUnit = xRight->getUnit();
		physicalUnit.dimensionless();
		break;
		
	case SIN:
		xFunction = &Function::sinus;
		sName = "sin(" + xRight->getName() + ")";
		physicalDescription = xRight->getPhysicalDescription();
		physicalUnit = xRight->getUnit();
		physicalUnit.dimensionless();
		break;
	
	case COS:
		xFunction = &Function::cosinus;
		sName = "cos(" + xRight->getName() + ")";
		physicalDescription = xRight->getPhysicalDescription();
		physicalUnit = xRight->getUnit();
		physicalUnit.dimensionless();
		break;
	
	case SQUARE:
		xFunction = &Function::square;
		sName = "(" + xRight->getName() + ")^2";
		physicalDescription = xRight->getPhysicalDescription();
		physicalUnit = xRight->getUnit();
		physicalUnit *= physicalUnit;
		break;
	
	case SQRT:
		xFunction = &Function::squareroot;
		sName = "sqrt(" + xRight->getName() + ")";
		physicalDescription = xRight->getPhysicalDescription();
		physicalUnit = xRight->getUnit();
		physicalUnit.sqrt();
		break;
	
	case POWER:
		xFunction = &Function::power;
		sName = "(" + xLeft->getName() + ") ^ (" + xRight->getName() + ")";
		break;
	
	case ROOT:
		xFunction = &Function::root;
		sName = "(" + xLeft->getName() + ") ^ 1/(" + xRight->getName() + ")";
		break;
		
	case INCREMENTS:
		xFunction = &Function::increments;
		sName = "d {" + xRight->getName() + "}";
		physicalUnit = xRight->getUnit();
		break;
		
	case INTEGRAL_X: 
		xFunction = &Function::integral_x; 
		sName = "integral {" + xLeft->getName() + "} d " + xRight->getName();
		physicalUnit = xLeft->getUnit();
		physicalUnit *= xRight->getUnit();
		break;
	
	case INTEGRAL_Y:
		xFunction = &Function::integral_y;
		sName = "integral {" + xRight->getName() + "} d" + xRight->getYInputName();
		physicalUnit = xLeft->getUnit();
		physicalUnit *= xRight->getUnit();
		break;
	
	case INPUT_X: 
		xFunction = &Function::inputx;
		break;
	
	case INPUT_Y:
		xFunction = &Function::inputy;
		break;
	
	case GREATER:
		xFunction = &Function::greater;
		sName = xLeft->getName() + " > " + xRight->getName();
		break;
	
	case GREATER_EQUAL:
		xFunction = &Function::greater_equal;
		sName = xLeft->getName() + " >= " + xRight->getName();
		break;
		
	case SMALLER:
		xFunction = &Function::smaller;
		sName = xLeft->getName() + " < " + xRight->getName();
		break;
		
	case SMALLER_EQUAL:
		xFunction = &Function::smaller_equal;
		sName = xLeft->getName() + " <= " + xRight->getName();
		break;
		
	case SIGN:
		xFunction = &Function::signum;
		sName = "sign(" + xRight->getName() + ")";
		break;
		
	case X_TABLE:
		xFunction = &Function::x_table;
		sName = xTable->sName();
		physicalDescription = xRight->getPhysicalDescription();
		physicalUnit = xTable->pPhysical(1).getUnit();
		break;
	
	case EMPTY:
		xFunction = &Function::inputx;
		break;
	}	
// 	if (sName.length()>100) sName = "...";
}

void Function::setXInput(double d)
{
// cout << "setting x input of " << sName << " to " << d << endl;
//	if (dNextXInput != d) {
//		dNextXInput = d;
		dXInput = d;
		if (eType==INPUT_X) {
			dValue = d;
		}
		if (xLeft) xLeft->setXInput(d);
		if (xRight) xRight->setXInput(d);
		if (xPlus) xPlus->setXInput(d);
		bValueValid = false;
//	}
}

void Function::setYInput(double d)
{
	if (dNextYInput != d) {
		dNextYInput = d;
		if (eType==INPUT_Y) {
			dValue = d;
		}
		if (xLeft) xLeft->setYInput(d);
		if (xRight) xRight->setYInput(d);
		if (xPlus) xPlus->setYInput(d);
		bValueValid = false;
	}
}

double Function::getXInput()
{
	if (eType==INPUT_X) {
		return getValue();
	}
	if (xLeft) return xLeft->getXInput();
	if (xRight) return xRight->getXInput();
	if (xPlus) return xPlus->getXInput();
	
	return 0.0;
}

double Function::getYInput()
{
	if (eType==INPUT_Y) {
		return getValue();
	}
	if (xLeft) return xLeft->getYInput();
	if (xRight) return xRight->getYInput();
	if (xPlus) return xPlus->getYInput();
	
	return 0.0;
}

string Function::getXInputName()
{
	string name;
	if (eType == INPUT_X)
		name = sName + ", " + physicalUnit.getName();
	if (!name.length() && xLeft)
		name = xLeft->getXInputName();
	if (!name.length() && xRight)
		name = xRight->getXInputName();
	if (!name.length() && xPlus)
		name = xPlus->getXInputName();
	return name;
}

string Function::getYInputName()
{
	string name;
	if (eType == INPUT_Y)
		name = sName + ", " + physicalUnit.getName();
	if (!name.length() && xLeft)
		name = xLeft->getYInputName();
	if (!name.length() && xRight)
		name = xRight->getYInputName();
	if (!name.length() && xPlus)
		name = xPlus->getYInputName();
	return name;
}

string Function::getName() const
{
	return sName;
}

Function &Function::setName(string s)
{
	sName = s;
	return *this;
}

Function &Function::setVerbose(bool verbose, ostream *log)
{
		bVerbose = verbose;
		oLog = log;
		if (xRight) xRight->setVerbose(verbose, log);
		if (xLeft) xLeft->setVerbose(verbose, log);
		if (xPlus) xPlus->setVerbose(verbose, log);
		return *this;
}
	
double Function::getValue() const
{
	if (!bValueValid) {
		dLastValue = dValue;
		dValue = (this->*xFunction)();
		bValueValid = true;
	}
	if (bVerbose)
		(*oLog) << dValue << " <- " << getName() << endl;
	return dValue;
}

double Function::getLastValue() const
{
// cout << sName << ": getLastValue() returns " << dValue << endl;
	return dLastValue;
}

void Function::unvalidate()
{
	// cout << " unvalidating {" << sName << endl;
	bValueValid = false;
	if (xLeft) xLeft->unvalidate();
	if (xRight) xRight->unvalidate();
	if (xPlus) xPlus->unvalidate();
	// cout << "} end unvaliding" << endl;
}

ostream& operator<<(ostream& s, Function& f)
{
	// only x input
	int n = int(floor((f.dXMax - f.dXMin)/f.dXStep + 0.5));
	int c = int(floor(f.dXMin/f.dXStep + 0.5));
	Matrix m( n, 2 );
	double d = f.dXMin;
        f.unvalidate();
 	for (int i=0; i<n; ++i, ++c) {
		d = c * f.dXStep;
		f.setXInput(d);
		m[i][0] = d;
		m[i][1] = f.getValue();
		if (!(i&0x7)) cout << "\rdisplaying function: " << double(i) / double(n) * 100.0 << "%         \t" << flush;
	}
	cout << "\rdisplaying function: done.         \t" << endl;
	
	// x and y input
	
	return s << m;
}

void Function::operator+=( Function &add )
{
	if (eType==EMPTY)
		copy(add);
	else {
		Function *clone = new Function(*this);
		init();
		xLeft = clone;
		xRight = &add;
		setType(PLUS);
	}
}

void Function::operator-=( Function &add )
{
	if (eType==EMPTY)
		copy(add);
	else {
		Function *clone = new Function(*this);
		init();
		xLeft = clone;
		xRight = &add;
		setType(MINUS);
	}
}

void Function::operator/=( Function &add )
{
	if (eType==EMPTY)
		copy(add);
	else {
		Function *clone = new Function(*this);
		init();
		xLeft = clone;
		xRight = &add;
		setType(OVER);
	}
}

void Function::operator*=( Function &add )
{
	if (eType==EMPTY)
		copy(add);
	else {
		Function *clone = new Function(*this);
		init();
		xLeft = clone;
		xRight = &add;
		setType(TIMES);
	}
}

Function &operator+( Function &left, Function &right )
{
	return *(new Function(left, right, PLUS));
}

Function &operator+( double left, Function &right )
{
	return *(new Function(cnst(left), right, PLUS));
}

Function &operator+( Function &left, double right )
{
	return *(new Function(left, cnst(right), PLUS));
}

Function &operator-( Function &left, Function &right )
{	
	return *(new Function(left, right, MINUS));
}

Function &operator-( double left, Function &right )
{	
	return *(new Function(cnst(left), right, MINUS));
}

Function &operator-( Function &left, double right )
{	
	return *(new Function(left, cnst(right), MINUS));
}

Function &operator-( Function &right )
{	
	return *(new Function(right, NEG));
}

Function &greater( Function &left, Function &right )
{	
	return *(new Function(left, right, GREATER));
}

Function &smaller( Function &left, Function &right )
{	
	return *(new Function(left, right, SMALLER));
}

Function &sign( Function &right )
{	
	return *(new Function(right, SIGN));
}

Function &operator>( Function &left, Function &right )
{	
	return *(new Function(left, right, GREATER));
}

Function &operator<( Function &left, Function &right )
{	
	return *(new Function(left, right, SMALLER));
}

Function &operator<=( Function &left, Function &right )
{	
	return *(new Function(left, right, SMALLER_EQUAL));
}

Function &operator>=( Function &left, Function &right )
{	
	return *(new Function(left, right, GREATER_EQUAL));
}

Function &operator*( Function &left, Function &right )
{	
	return *(new Function(left, right, TIMES));
}

Function &operator*( double left, Function &right )
{	
	return *(new Function(cnst(left), right, TIMES));
}

Function &operator*( Function &left, double right )
{	
	return *(new Function(left, cnst(right), TIMES));
}

Function &operator/( Function &left, Function &right )
{	
	return *(new Function(left, right, OVER));
}

Function &operator/( double left, Function &right )
{	
	return *(new Function(cnst(left), right, OVER));
}

Function &operator/( Function &left, double right )
{	
	return *(new Function(left, cnst(right), OVER));
}

Function &inputX()
{
	return *(new Function(INPUT_X));
}

Function &inputY()
{
	return *(new Function(INPUT_Y));
}

Function &input(string s)
{
	return inputX().setName(s);
}

Function &cnst(double d)
{
	return *(new Function(d, CONSTANT));
}

Function &cnst(double d, string name)
{
	return *(new Function(d, name, CONSTANT));
}

Function &increments(Function &right)
{
	if (right.eType==EMPTY)
		return empty();
	return *(new Function(right, INCREMENTS));
}

Function &integral(Function &left, Function &right)
{
	if (left.eType==EMPTY && right.eType==EMPTY)
		return empty();
	if (left.eType==EMPTY && right.eType!=EMPTY)
		return right;
	if (left.eType!=EMPTY && right.eType==EMPTY)
		return left;
	return *(new Function(left, right, INTEGRAL_X));
}

Function &xtable(Matrix &matrix, Function &right)
{
	return *(new Function(right, &matrix, X_TABLE));
}

Function &exp( Function &right )
{	
	if (right.eType==EMPTY)
		return empty();
	return *(new Function(right, EXP));
}

Function &ln( Function &right )
{	
	if (right.eType==EMPTY)
		return empty();
	return *(new Function(right, LN));
}

Function &sin( Function &right )
{	
	if (right.eType==EMPTY)
		return empty();
	return *(new Function(right, SIN));
}

Function &cos( Function &right )
{	
	if (right.eType==EMPTY)
		return empty();
	return *(new Function(right, COS));
}

Function &sqrt( Function &right )
{	
	if (right.eType==EMPTY)
		return empty();
	return *(new Function(right,  SQRT));
}

Function &square( Function &right )
{	
	if (right.eType==EMPTY)
		return empty();
	return *(new Function(right, SQUARE));
}

Function &root( Function &left, Function &right )
{	
	if (left.eType==EMPTY && right.eType==EMPTY)
		return empty();
	if (left.eType==EMPTY && right.eType!=EMPTY)
		return right;
	if (left.eType!=EMPTY && right.eType==EMPTY)
		return left;
	return *(new Function(left, right, ROOT));
}

Function &power( Function &left, Function &right )
{	
	if (left.eType==EMPTY && right.eType==EMPTY)
		return empty();
	if (left.eType==EMPTY && right.eType!=EMPTY)
		return right;
	if (left.eType!=EMPTY && right.eType==EMPTY)
		return left;
	return *(new Function(left, right, POWER));
}

Function &empty()
{
	return *(new Function(EMPTY));
}

double Function::increments() const 
{
	// get current value
	double d0 = xRight->getValue();
	
	// get value at next step
	xRight->setXInput(dXInput + dXStep);
	xRight->unvalidate();
	double d1 = xRight->getValue();
	
	// restore input
	xRight->setXInput(dXInput);
	xRight->unvalidate();
	
	// return difference
	return d1 - d0;
};

double Function::integral_x() const
{
	xRight->setXInput(0.0);
	xLeft->unvalidate();
	double x0 = xLeft->getValue(), x1 = xLeft->getValue(); // value of integrand at t=0,1
	double y0 = xRight->getValue(), y1 = xRight->getValue(); // values of integrator at t=0,1
	double xMinus = 0.0; // value of integral
	double xPlus = 0.0;
	double dy = y1 - y0; // increment
	
	int dir = dXInput>=0.0 ? 1 : -1; // direction
	double oldX = xRight->getXInput();
	int n = int(floor(dXInput*dir/dXStep + 0.5)) + 1; // round(.) 
	for (int c=0; c<n; ++c) {
		xMinus += x0 * dy;
		xPlus += x1 * dy;
		// next step
		xRight->setXInput( (c+1)*dXStep*dir );
		xLeft->unvalidate();
		x0 = x1;
		y0 = y1;
		x1 = xLeft->getValue();
		y1 = xRight->getValue();
		dy = y1 - y0;
	}
	xRight->setXInput(oldX);
	return 0.5 * (xMinus + xPlus);
}

double Function::x_table() const
{
	
	// find place in table
	int steps = xTable->nSize(0);
	int i=steps-1, j=0; // indices into matrix
	int dir = 1; // search direction, 1-up, -1-down
	double low = (*xTable)[0][0].to_d(); // x-value left of dXInput
	double high = (*xTable)[steps-1][0].to_d(); // x-value right of dXInput
	double *current = &low;
	double lowValue = (*xTable)[0][1].to_d();
	double highValue = (*xTable)[steps-1][1].to_d();
	double x = xRight->getValue();
	if (x <= low)
		return lowValue;
	else if (x>=high)
		return highValue;
	else {
		while ( steps>1 
			|| low > x
			|| high < x
		) {	
			// divide by two, keeping parity
			steps = (steps&1) | (steps>>1);
			if (dir>0) 
				i = j;
			else j = i;
			
			// compare
			if (*current < x)
				dir = 1;
			else dir = -1;
			
			// step
			if (dir>0) {
				j += dir*steps;
				current = &high;
			} else {
				i += dir*steps;
				current = &low;
			}
			low = (*xTable)[i][0].to_d();
			high = (*xTable)[j][0].to_d();
		}
		lowValue = (*xTable)[i][1].to_d();
		highValue = (*xTable)[j][1].to_d();
	}
	// interpolate
	return ((x - low) * highValue + (high - x) * lowValue) / (high - low);
}

