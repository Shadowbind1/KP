set title 'V(x)'
set xlabel 'x'
set ylabel 'V(x)'
set grid
plot 'graph_data.dat' using 1:2 with linespoints title 'V(x)'
pause mouse close
