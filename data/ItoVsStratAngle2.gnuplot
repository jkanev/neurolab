set title 
set multiplot
set grid
set xtics font "Helvetica,12pt"
set ytics font "Helvetica,12pt"
set ztics font "Helvetica,12pt"
set xlabel "time, millisecond" font "Helvetica,14pt"
set ylabel "voltage" font "Helvetica,14pt"
set zlabel "" font "Helvetica,14pt"


plot "data/ItoVsStratAngle2.0.data" title "sample of theta neuron" with lines , "data/ItoVsStratAngle2.1.data" title "sample of theta neuron" with lines 
unset multiplot
