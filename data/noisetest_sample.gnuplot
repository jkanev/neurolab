set title 
set multiplot
set grid
set xtics font "Helvetica,12pt"
set ytics font "Helvetica,12pt"
set ztics font "Helvetica,12pt"
set xlabel "time, second" font "Helvetica,14pt"
set ylabel "stochastic process" font "Helvetica,14pt"
set zlabel "" font "Helvetica,14pt"


plot "data/noisetest_sample.0.data" title "sample of Poisson process[0]" with lines , "data/noisetest_sample.1.data" title "sample of Wiener process[1]" with lines , "data/noisetest_sample.2.data" title "sample of Poisson process[2]" with lines , "data/noisetest_sample.3.data" title "sample of Wiener process[3]" with lines , "data/noisetest_sample.4.data" title "sample of Poisson process[4]" with lines 
unset multiplot
