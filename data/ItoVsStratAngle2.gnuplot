set title 
set multiplot
set grid
set xtics font "Helvetica,12pt"
set ytics font "Helvetica,12pt"
set ztics font "Helvetica,12pt"
set xlabel "time, second" font "Helvetica,14pt"
set ylabel "voltage, millivolt" font "Helvetica,14pt"
set zlabel "" font "Helvetica,14pt"


plot "data/ItoVsStratAngle2.0.data" title " Itô neuron membrane" with lines , "data/ItoVsStratAngle2.1.data" title " Stratonovich neuron membrane" with lines 
unset multiplot
