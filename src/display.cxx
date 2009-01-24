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

#include "../h/display.hxx"

Display::Display(const char *filebase) : sFileBase( filebase )
{
	pFile = 0;
	pFile = new ofstream( (sFileBase + string(".gnuplot")).c_str(), ios_base::trunc );
	nMode = 0;
}

Display::~Display()
{
	pFile->close();
}

void Display::setTitle( string title )
{
	sTitle = title;
}

void Display::addUserSetting( string setting )
{
	if (vPlots.size())
		vPlots.back().sUserSettings += setting + "\n";
	else
		sUserSettings += setting + "\n";
}

void Display::clearUserSettings()
{
	sUserSettings = "";
}

void Display::setLineStyle( const int &n )
{
	if (vPlots.size())
		vPlots.back().nLineStyle = n;
}

void Display::setMode( const int &n )
{
	nMode = n;
}

void Display::addMode( const int &n )
{
	nMode |= n;
}

void Display::rmMode( const int &n )
{
	nMode &= !n;
}

void Display::save()
{
	pFile->close();
	pFile = new ofstream( (sFileBase + string(".gnuplot")).c_str(), ios_base::trunc );
	if (pFile) {
		switch (nMode & DSP_SAVE_MASK) {
			case DSP_SAVE_GNUPLOT:
				sXFont = "Helvetica,14pt";
				sYFont = "Helvetica,14pt";
				sZFont = "Helvetica,14pt";
				sXTicFont = "Helvetica,12pt";
				sYTicFont = "Helvetica,12pt";
				sZTicFont = "Helvetica,12pt";
				break;
			case DSP_SAVE_JPEG:
				(*pFile) << "set terminal jpeg" << endl
						<< "set output " << "\""<< sFileBase << ".jpg\"" << endl;
				sXFont = ",12pt";
				sYFont = ",12pt";
				sZFont = ",12pt";
				sXTicFont = ",10pt";
				sYTicFont = ",10pt";
				sZTicFont = ",10pt";
				break;
			case DSP_SAVE_PNG:
				(*pFile) << "set terminal png" << endl
						<< "set output " << "\""<< sFileBase << ".png\"" << endl;
				sXFont = ",12pt";
				sYFont = ",12pt";
				sZFont = ",12pt";
				sXTicFont = ",10pt";
				sYTicFont = ",10pt";
				sZTicFont = ",10pt";
				break;
			case DSP_SAVE_POSTSCRIPT:
			default:
//				(*pFile) << "set terminal postscript color solid landscape 22" << endl
				(*pFile) << "set terminal postscript landscape enhanced mono 22" << endl
					<< "set output " << "\""<< sFileBase << ".ps\"" << endl;
				sXFont = "Helvetica,38pt";
				sYFont = "Helvetica,38pt";
				sZFont = "Helvetica,38pt";
				sXTicFont = "Helvetica,20pt";
				sYTicFont = "Helvetica,20pt";
				sZTicFont = "Helvetica,20pt";
				break;
		}
		plot();
	} 
	else cout << "instruction file error" << endl;
}

void Display::show()
{
	pFile->close();
	pFile = new ofstream( (sFileBase + string(".gnuplot")).c_str(), ios_base::trunc );
	if(pFile) {
		sXFont = "Helvetica,14pt";
		sYFont = "Helvetica,14pt";
		sZFont = "Helvetica,14pt";
		sXTicFont = "Helvetica,12pt";
		sYTicFont = "Helvetica,12pt";
		sZTicFont = "Helvetica,12pt";
		plot();
	} 
	else cout << "instruction file error" << endl;
}

int Display::plot()
{
	// sort plot vector by plot-mode
	// not implemented yet
	if(pFile) {
		int mode = 0; // start with default mode
		int oldMode = 0; // to detect mode changes -> set different coordinates for subplots
		bool firstPlot = true; // the first plot get a different command
		
		(*pFile) << "set title " << sTitle << endl;
		(*pFile) << "set multiplot" << endl << "set grid" << endl;
		for(uint i=0; i<vPlots.size(); i++) {
			mode = vPlots[i].nMode;
			
			// set new position when changed
			if( (mode & DSP_POSITION_MASK) != (oldMode & DSP_POSITION_MASK) ) {
				(*pFile) << endl;
				switch( mode & DSP_POSITION_MASK ) {
					case DSP_TOP_LEFT:
						(*pFile) << "unset grid" << endl
							<< "set origin 0.0,0.5" << endl
							<< "set size 0.5,0.5" << endl << "clear" << endl;
						break;
					case DSP_TOP_RIGHT:
						(*pFile) << "unset grid" << endl
							<< "set origin 0.5,0.5" << endl
							<< "set size 0.5,0.5" << endl << "clear" << endl;
						break;
					case DSP_BOTTOM_LEFT:
						(*pFile) << "unset grid" << endl
							<< "set origin 0.0,0.0" << endl
							<< "set size 0.5,0.5" << endl << "clear" << endl;
						break;
					case DSP_BOTTOM_RIGHT:
						(*pFile) << "unset grid" << endl
							<< "set origin 0.5,0.0" << endl
							<< "set size 0.5,0.5" << endl << "clear" << endl;
						break;
					case DSP_TOP:
						(*pFile) << "set grid" << endl
							<< "set origin 0.0,0.5" << endl
							<< "set size 1.0,0.5" << endl << "clear" << endl;
						break;
					case DSP_BOTTOM:
						(*pFile) << "set grid" << endl
							<< "set origin 0.0,0.0" << endl
							<< "set size 1.0,0.5" << endl << "clear" << endl;
						break;
					case DSP_LEFT:
						(*pFile) << "set grid" << endl
							<< "set origin 0.0,0.5" << endl
							<< "set size 0.5,1.0" << endl << "clear" << endl;
						break;
					case DSP_RIGHT:
						(*pFile) << "set grid" << endl
							<< "set origin 0.5,0.0" << endl
							<< "set size 0.5,1.0" << endl << "clear" << endl;
						break;
				}
				firstPlot = true;
			}
			
			// append next plot
			if( firstPlot ) {
				(*pFile) << "set xtics font \"" << sXTicFont << "\"" << endl;
				(*pFile) << "set ytics font \"" << sYTicFont << "\"" << endl;
				(*pFile) << "set ztics font \"" << sZTicFont << "\"" << endl;
				(*pFile) << "set xlabel \"" << vPlots[i].sXLabel << "\" font \"" << sXFont << "\"" << endl;
				(*pFile) << "set ylabel \"" << vPlots[i].sYLabel << "\" font \"" << sYFont << "\"" << endl;
				(*pFile) << "set zlabel \"" << vPlots[i].sZLabel << "\" font \"" << sZFont << "\"" << endl;
				(*pFile) << sUserSettings << endl;
				(*pFile) << vPlots[i].sUserSettings << endl;
				switch(mode & (DSP_AXIS_MASK) ) {
					case DSP_X_AXIS_LOG:
						(*pFile) << "set logscale x" << endl;
						break;
					case DSP_Y_AXIS_LOG:
						(*pFile) << "set logscale y " << endl;
						break;
					case DSP_Z_AXIS_LOG:
						(*pFile) << "set logscale z" << endl;
				}
				switch(mode & (DSP_DIMENSION_MASK|DSP_COLOR_MASK)) {
					case DSP_3D:
						(*pFile) << "set pm3d map" << endl;
					case DSP_3D|DSP_BW:
						(*pFile) << "splot ";
						break;
					case DSP_2D:
						(*pFile) << "plot ";
				}
			} else 
				(*pFile) << ", ";
			(*pFile) << vPlots[i].sFunction << " title \"" << vPlots[i].sName << "\" " ;
			
			switch(mode & DSP_LINE_MASK) {
				case DSP_LINES:
					(*pFile) << "with lines "; 
					break;
				case DSP_POINTS:
					(*pFile) << "with points "; 
					break;
				case DSP_DOTS:
					(*pFile) << "with dots "; 
					break;
				case DSP_IMPULSES:
					(*pFile) << "with impulses "; 
					break;
				case DSP_BARS:
					(*pFile) << "with boxes "; 
					break;
				case DSP_STEPS:
					(*pFile) << "with steps "; 
					break;
				case DSP_LINESPOINTS:
					(*pFile) << "with linespoints ";
					break;
				case DSP_NOLINES:
					(*pFile) << " ";
			}
			if (vPlots[i].nLineStyle)
				(*pFile) << vPlots[i].nLineStyle;
			firstPlot = false;
			oldMode = mode;
		}
		(*pFile) << endl << "unset multiplot" << endl;
		(*pFile) << flush;
		
		// run terminal + gnuplot
		ostringstream command("");
		command << _GNUPLOT_CALL
			<< "-xrm \"gnuplot*background:#e0e0a0\" "
			<< "-xrm \"gnuplot*line1Color:blue\" "
			<< "-xrm \"gnuplot*line2Color:red\" "
			<< "-xrm \"gnuplot*line3Color:darkgrey\" "
			<< "-persist -title \"NeuroLab :: Plot\" ";
//		command << "/usr/bin/gnuplot -title \"NeuroLab :: Plot\" ";
		command << sFileBase << ".gnuplot" << flush;
		system( command.str().c_str() );
		return 0;
	}
	else cout << "instruction file error" << endl;
	return 1;
}


Display& Display::operator<<(Matrix m)
{
	// write data to file
	if(pFile) {
		stringstream filename("");
		filename << sFileBase << '.' << vPlots.size() << ".data" << flush;
		ofstream datafile(filename.str().c_str());
		if(!filename) {
			cout << "unable to open data file " << filename << endl;
			return *this;
		}
		datafile.precision(8);
		datafile << m << endl;
		datafile.close();
		
		// create plot object
		Plot plot("\"" + filename.str() + "\"", m.sName(), m.pPhysical(0).getDescription(), m.pPhysical(1).getDescription(), m.pPhysical(2).getDescription(), nMode);
		vPlots.push_back( plot );
	} else {
		cout << "data file error" << endl;
	}
	return (*this);
}

Display& Display::operator<<( Function &f )
{
	// write data to file
	if(pFile) {
		stringstream filename("");
		filename << sFileBase << '.' << vPlots.size() << ".data" << flush;
		ofstream datafile(filename.str().c_str());
		if(!filename) {
			cout << "unable to open data file " << filename << endl;
			return *this;
		}
		datafile.precision(8);
		datafile << f << endl;
		datafile.close();
		
		// create plot object
		if (f.getYInputName() != "")  {
			// 3d plot
			Plot plot("\"" + filename.str() + "\"", f.getName(), f.getXInputName(), f.getYInputName(), f.getPhysicalDescription() + ", " + f.getUnitName(), nMode);
			vPlots.push_back( plot );
		} else {
			// 2d plot
			Plot plot("\"" + filename.str() + "\"", f.getName(), f.getXInputName(), f.getPhysicalDescription() + "[" + f.getUnitSymbol() + "]", "", nMode);
			vPlots.push_back( plot );
		}
		
	} else {
		cout << "data file error" << endl;
	}
	return (*this);
}

Display& Display::operator<<( const DisplayAction &da )
{
	da.execute(this);
	return *this;
}

DisplayAction addsetting( string variable, string value )
{
	return DisplayAction(0, "set " + variable + " " + value, 0, 0, &Display::addUserSetting); 
};

DisplayAction setmode( const int mode )
{
	return DisplayAction(mode, "", 0, &Display::setMode, 0);
};

DisplayAction setLineStyle( const int style )
{
	return DisplayAction(style, "", 0, &Display::setLineStyle, 0);
};

DisplayAction setTitle( const string title )
{
	return DisplayAction(0, "\""+title+"\"", 0, 0, &Display::setTitle);
};


