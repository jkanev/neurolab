set title 
set multiplot
set grid
set xtics font "Helvetica,12pt"
set ytics font "Helvetica,12pt"
set ztics font "Helvetica,12pt"
set xlabel "cond. variance" font "Helvetica,14pt"
set ylabel "isi variance" font "Helvetica,14pt"
set zlabel "" font "Helvetica,14pt"


plot "data/ItoVsStratVar2.0.data" title " Itô" with lines , "data/ItoVsStratVar2.1.data" title " Stratonovich" with lines 
unset multiplot
