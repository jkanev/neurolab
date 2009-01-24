set title 
set multiplot
set grid
set xtics font "Helvetica,12pt"
set ytics font "Helvetica,12pt"
set ztics font "Helvetica,12pt"
set xlabel "time, second" font "Helvetica,14pt"
set ylabel "voltage, millivolt" font "Helvetica,14pt"
set zlabel "" font "Helvetica,14pt"


plot "data/ItoVsStratVoltage.0.data" title "voltage sample of Ito neuron" with lines , "data/ItoVsStratVoltage.1.data" title "voltage sample of Stratonovich neuron" with lines 
unset multiplot
