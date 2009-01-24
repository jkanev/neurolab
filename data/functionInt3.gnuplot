set title 
set multiplot
set grid
set xtics font "Helvetica,12pt"
set ytics font "Helvetica,12pt"
set ztics font "Helvetica,12pt"
set xlabel "s, " font "Helvetica,14pt"
set ylabel "[]" font "Helvetica,14pt"
set zlabel "" font "Helvetica,14pt"


plot "data/functionInt3.0.data" title "integral {exp(s - t)} d sin(s)" with lines , "data/functionInt3.1.data" title "integral {exp(s - t) * cos(s)} d s" with lines 
unset multiplot
