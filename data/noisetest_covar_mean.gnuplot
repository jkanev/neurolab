set title 
set multiplot
set grid
set xtics font "Helvetica,12pt"
set ytics font "Helvetica,12pt"
set ztics font "Helvetica,12pt"
set xlabel "time, second" font "Helvetica,14pt"
set ylabel "stochastic process" font "Helvetica,14pt"
set zlabel "" font "Helvetica,14pt"


plot "data/noisetest_covar_mean.0.data" title "mean of stochastic process" with lines , "data/noisetest_covar_mean.1.data" title "t * -0.0266896" with lines , "data/noisetest_covar_mean.2.data" title "mean of stochastic process" with lines , "data/noisetest_covar_mean.3.data" title "t * 0.0757396" with lines , "data/noisetest_covar_mean.4.data" title "mean of stochastic process" with lines , "data/noisetest_covar_mean.5.data" title "t * 0.512" with lines 
unset multiplot
