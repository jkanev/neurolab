#include "../h/neurolab"

int main(int argc, char **argv)
{
	Display dsp("data/functions");
	dsp.setMode(0);
	
	// plot lots of one-dimensional functions
	Function t = input("t");
		
	double a, b, c;
	for( a=b=c=0; a<2; a+=0.4, b-=0.4, c+=1.2 )
		dsp << c * sin(a*t + b);

	// another plot of a one-dimensional function
	dsp.setMode( DSP_TOP_LEFT);
	dsp << sin(t);
	
	dsp.show();
	dsp.save();
	
	// some integral
	Display dspInt("data/functionInt");
	double scale = 1.0 / sqrt(2.0 * 3.14159265737);
	dspInt << scale * integral( exp(-square(t)/cnst(2.0)), t );
	dspInt << scale * exp(-square(t)/cnst(2.0) );
	dspInt.show();
	
	// some other integral
	Display dspInt2("data/functionInt2");
	Function s = input("s");
	Function fOne = 1.0 - integral( exp(s-t), s ); 
	fOne.setXRange(-1.0, 10.0, 0.01);
	Function fTwo = exp(-t); 
	fTwo.setXRange(-1.0, 10.0, 0.01);
	Function fThree = 1.0 - exp(-t) * integral( exp(t), t );
	fThree.setXRange(-1.0, 10.0, 0.01);
	dspInt2 << fOne << fTwo << fThree;
	dspInt2.show();
	
	// integrating over functions
	Display dspInt3("data/functionInt3");
	Function i1 = integral( exp(s-t), sin(s) ); 
	Function i2 = integral( exp(s-t)*cos(s), s );
	dspInt3 << i1 << i2;
	dspInt3.show();
	 
	return 0;
}
