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


#ifndef __DISPLAY_HXX
#define __DISPLAY_HXX

#include "matrix.hxx"
#include "function.hxx"
#include <cstdlib> // for system()
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// modes from the different blocks can be combined by |
// color modes (bit 0)
const int DSP_COLOR_MASK = 0x1; ///< for internal use only
const int DSP_COLOR = 0x0; ///< colour
const int DSP_BW = 0x1; ///< black and white

// dimension modes (bit 1)
const int DSP_DIMENSION_MASK = 0x2; ///< for internal use only
const int DSP_2D = 0x0; ///< 2D
const int DSP_3D = 0x2; ///< 3D

// line modes (bit 2-4)
const int DSP_LINE_MASK = 0x1c; ///< for intermal use only
const int DSP_LINES = 0x0; ///< line
const int DSP_POINTS = 0x4; ///< points
const int DSP_DOTS = 0x8; ///< dots
const int DSP_IMPULSES = 0xc; ///< lines from zero to each data point
const int DSP_BARS = 0x10; ///< display bars, useful for probabilities
const int DSP_STEPS =0x14; ///< step curve
const int DSP_LINESPOINTS = 0x18; ///< use normal drawing again
const int DSP_NOLINES = 0x1c; ///< use normal drawing again

// position (bit 5-9)
const int DSP_POSITION_MASK = 0x3e0; ///< for internal use only
const int DSP_MAIN = 0x0; ///< normal plotting
const int DSP_TOP_RIGHT = 0x20; ///< include in top right corner
const int DSP_TOP_LEFT = 0x40; ///< include in top left corner
const int DSP_BOTTOM_RIGHT = 0x60; ///< include in bottom right corner
const int DSP_BOTTOM_LEFT = 0x80; ///< include in bottom right corner
const int DSP_LEFT = 0xa0; ///< left half
const int DSP_RIGHT = 0xc0; ///< right half
const int DSP_TOP = 0xe0; ///< top half
const int DSP_BOTTOM = 0x100; ///< bottom half

// axis modes (bit 10-12)
const int DSP_AXIS_MASK = 0x1c00; /// for internal use only
const int DSP_X_AXIS_LOG = 0x400; ///< x axis logarithmic
const int DSP_Y_AXIS_LOG = 0x800; /// < y axis logarithmic
const int DSP_Z_AXIS_LOG = 0x1000; /// < z axis logarithmic
const int DSP_AXIS_LOG = 0x1c00; ///< all axis logarithmic

// save modes (bit 13-15)
const int DSP_SAVE_MASK = (0x7 << 13); ///< for internal use only
const int DSP_SAVE_GNUPLOT = (0x0 << 13); ///< save for further gnuplot calls
const int DSP_SAVE_POSTSCRIPT = (0x1 << 13); ///< save as postscript
const int DSP_SAVE_PNG = (0x2 << 13); ///< save as png
const int DSP_SAVE_JPEG = (0x3 << 13); ///< save as jpeg
const int DSP_SAVE_PDF = (0x4 << 13); ///< save as pdf

/// A plot descriptor used by the Display class.
/** This is meant to be used internally by the Display class. An object like this describes a plot command in gnuplot. */
class Plot
{
public:
	string sFunction; ///< The function (analytic expression or filename).
	string sName; ///< The name (label in gnuplot).
	int nMode; ///< The mode (2D, 3D etc.)
	string sXLabel; ///< The x-axis label
	string sYLabel; ///< The y-axis label
	string sZLabel; ///< The z-axis label
	string sUserSettings; ///< optional user settings
	string sUsedDimensions; ///< optional setting for "using" string (see gnuplot)
	int nLineStyle; ///< the line type (0 - normal count, 1 - ..: special colour/style)
	
	/// Construct.
	Plot(string function, string name, string xlabel, string ylabel, string zlabel, int mode, int lineStyle=0) {
		sFunction = function;
		sName = name;
		sXLabel = xlabel;
		sYLabel = ylabel;
		sZLabel = zlabel;
		nMode = mode;
		nLineStyle = lineStyle;
	};
	/// Destruct.
	~Plot(){};
};

/// Class for displaying matrix data and analytical functions.
/** Display data given as Matrixes or as strings describing functions in various forms. Currently there is only an interface to gnuplot implemented, others shall follow (qwt, grace, qplotter). */
class Display
{
private:
	string sFileBase; // filebase
	ofstream *pFile; // gnuplot instruction file
	vector<Plot> vPlots; // vector with plot objects
	int nMode; // current display mode
	string sTitle; // the title
	string sUserSettings; // various settings appended by the user
	void appendPlot( string s, string t ); // append a plot-instruction to gnuplot instruction file; s - plot data (filename or function), t - title
	int plot(); // write file and exec gnuplot. this is the main function.
	
	string sXFont;
	string sYFont;
	string sZFont;
	string sXTicFont;
	string sYTicFont;
	string sZTicFont;

public:
	
	/// constructor
	/** @param filebase A string which will be used to construct the filenames, which will be called [filebase]000.dat, [filebase]001.dat, ... for the data files, and [filebase].gnuplot for the gnuplot instruction file.
	Constructs a display. The input argument file is used to create files in which to store data for gnuplot / grace. The will be named filebase.xxx.data, where xxx is a decimal integer running from 000 to 999 (depending on the number of files). After destruction these will not be deleted. */
	Display(const char *filebase);
	
	/// destructor
	~Display();
	
	/// Show data as plot.
	/** This displays the data. It opens a terminal with a gnuplot-session, with the data already plotted. The user may close the terminal and/or gnuplot window, or use the terminal to add more plots to the gnuplot window. */
	void show();
	
	/// Save data to postscript file.
	/** This saves the data. The raw data is saved anyway, this function saves the postscript file which you want to include in your paper. */
	void save();
	
	/// Set mode.
	/** Sets the display mode. A mode is an |-list of values. This must be done *before* the first data is inserted using the << operator. Current values are DSP_3D, DSP_2D, DSP_INCLUDE_START, DSP_INCLUDE_END */
	void setMode( const int &mode );
	
	/// Set line style.
	/** Sets the line style of the last plot. Style is an integer >= 0. In gnuplot, if many plots are executed, each plot gets a different colour (if in colour mode), or line style like dottet, dashed, etc. (if in monochrome mode). Using the setLineStyle function you can override this for the plot added last, and can specify a certain value (if you want multiple lines in the same color, for instance). If style==0, gnuplot defaults are used. */
	void setLineStyle( const int &style );
	
	/// Set line addition.
	/** Sets an addition to the style of the last plot (intended for "using" commands). */
	void setUsedDimensions( string s );

	/// Add mode.
	/* Adds a display mode. This adds a single mode, whithout changing the other modes of the |-list. This must be done *before* the first data is inserted using the << operator. Current values are DSP_3D, DSP_2D, DSP_INCLUDE_START, DSP_INCLUDE_END */
	void addMode( const int &mode );
	
	/// Remove mode.
	/* Remove a display mode. This removes a single mode, whithout changing the other modes of the |-list. This must be done *before* the first data is inserted using the << operator. Current values are DSP_3D, DSP_2D, DSP_INCLUDE_START, DSP_INCLUDE_END */
	void rmMode( const int &mode );
	
	/// Set title.
	/** \param title The title.
	This is the title of the plot, written abouve the actual plot in gnuplot. */
	void setTitle( string title );
	
	/// Add user setting.
	/** This adds a user setting. The given string will be handed over to gnuplot before the first plot data is made. Example: dsp.addUserSetting("set yrange [-20:20]"); will set the y range to the given interval. It is also possible to add a comment here, which will then appear in the gnuplot file (handy if you want to understand what's in that file you've produced three months ago); Example: dsp.addUserSetting("# this contains the simulated output of an integrate-and-fire neuron with firing rate of 10Hz");. */
	void addUserSetting(string s);
	
	/// Clears all user settings
	void clearUserSettings();
	
	/// Insert matrix data
	/** This appends some data to the current display. Use show() to display all data. */
	Display& operator<<(Matrix m);

	/// Insert function
	/** This appends a function to plot to the current display. The stream must describe a string string which evaluates in gnuplot. Use show() to display all data and functions which have been inserted. */
	Display& operator<<(Function &f);
	
	/// Execute modification or action.
	/** This executes a modification (like dsp << set3d;) or an action (like dsp << show;). See the documentation for DisplayAction objects. */
	Display& operator<<(const class DisplayAction &a);
};

/// An action object.
/** This object can be appended to modify a Display object, or to execute an action. */
class DisplayAction
{
private:
	int nInt;
	string sString;
	void (Display::* action)();
	void (Display::* setInt)(const int &);
	void (Display::* setString)(string);
public:
	DisplayAction(
		int n, 
		string s, 
		void (Display::* sa)(), 
		void (Display::* sn)(const int &), 
		void (Display::* ss)(string)
	) {
		nInt = n;
		sString = s;
		action = sa;
		setInt = sn;
		setString = ss;
	}
	
	/// Execute one of my members.
	/* If I'm a Display class and call this with my this-pointer, one of my member functions gets called. Which, depends on the settings of this DisplayAction object. */
	void execute(Display *d) const {
		if (setInt) (d->*setInt)(nInt);
		if (setString) (d->*setString)(sString);
		if (action) (d->*action)();
	}
};

const DisplayAction showdsp(0, "", &Display::show, 0, 0);
const DisplayAction savedsp(0, "", &Display::save, 0, 0);
const DisplayAction saveps(DSP_SAVE_POSTSCRIPT, "", &Display::save, &Display::addMode, 0);
const DisplayAction savejpg(DSP_SAVE_JPEG, "", &Display::save, &Display::addMode, 0);
const DisplayAction savepng(DSP_SAVE_PNG, "", &Display::save, &Display::addMode, 0);
const DisplayAction savepdf(DSP_SAVE_PDF, "", &Display::save, &Display::addMode, 0);
DisplayAction addsetting( string variable, string value );
DisplayAction setmode( const int mode );
DisplayAction setLineStyle( const int n );
DisplayAction setTitle( const string s );
DisplayAction setUsedDimensions( const vector<uint> &dimension);

#endif
