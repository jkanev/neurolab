set title 
set multiplot
set grid
set xtics font "Helvetica,12pt"
set ytics font "Helvetica,12pt"
set ztics font "Helvetica,12pt"
set xlabel "s, " font "Helvetica,14pt"
set ylabel "[]" font "Helvetica,14pt"
set zlabel "" font "Helvetica,14pt"


plot "data/functionInt2.0.data" title " 1 - integral {exp(s - t)} d s" with lines , "data/functionInt2.1.data" title " exp(-t)" with lines , "data/functionInt2.2.data" title " 1 - exp(-t) * integral {exp(t)} d t" with lines 
unset multiplot
