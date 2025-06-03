set terminal pngcairo size 800,600 enhanced font 'Arial,11'
set output 'echo_bench.png'

set style data boxplot
set style boxplot outlier pointtype 7   
set boxwidth 0.5 relative

set xtics ('ebpf' 1, 'kecho' 2, 'user' 3)
set xlabel 'server type'
set ylabel 'Time (us)'
set title 'Server comparison'

plot \
    'ebpf_bench.txt' using (1):2 notitle, \
    'kecho_bench.txt' using (2):2 notitle, \
    'user_bench.txt' using (3):2 notitle