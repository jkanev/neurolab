set title 
set multiplot
set grid
set xtics font "Helvetica,12pt"
set ytics font "Helvetica,12pt"
set ztics font "Helvetica,12pt"
set xlabel "time, second" font "Helvetica,14pt"
set ylabel "" font "Helvetica,14pt"
set zlabel "" font "Helvetica,14pt"


plot "data/noisetest_mean.0.data" title " mean of Poisson process[0]" with lines , "data/noisetest_mean.1.data" title " t * -0.5" with lines , "data/noisetest_mean.2.data" title " mean of Wiener process[1]" with lines , "data/noisetest_mean.3.data" title " t * -0.4" with lines , "data/noisetest_mean.4.data" title " mean of Poisson process[2]" with lines , "data/noisetest_mean.5.data" title " t * -0.3" with lines , "data/noisetest_mean.6.data" title " mean of Wiener process[3]" with lines , "data/noisetest_mean.7.data" title " t * -0.2" with lines , "data/noisetest_mean.8.data" title " mean of Poisson process[4]" with lines , "data/noisetest_mean.9.data" title " t * -0.1" with lines 
unset multiplot
