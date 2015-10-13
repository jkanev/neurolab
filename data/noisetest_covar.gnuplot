set title 
set multiplot
set grid
set xtics font "Helvetica,12pt"
set ytics font "Helvetica,12pt"
set ztics font "Helvetica,12pt"
set xlabel "time, second" font "Helvetica,14pt"
set ylabel "" font "Helvetica,14pt"
set zlabel "" font "Helvetica,14pt"


plot "data/noisetest_covar.0.data" title " sample of stochastic process" with lines , "data/noisetest_covar.1.data" title " sample of stochastic process" with lines , "data/noisetest_covar.2.data" title " sample of stochastic process" with lines 
unset multiplot
