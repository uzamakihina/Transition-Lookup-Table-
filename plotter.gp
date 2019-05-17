set datafile separator ","
set terminal png size 1920,1080
set logscale x 2
#set logscale y 1.05
set title "Hit Rate vs. Page Size"
set ylabel "Hit Rate"
set xlabel "Page Size"
set xdata time
set timefmt "%s"
set format x "%s"
set key right bottom
set grid
plot filename using 1:2  with lines lw 2 lt 2 title 'TLB16', \
     filename using 1:3  with lines lw 2 lt 3 title 'TLB32', \
     filename using 1:4  with lines lw 2 lt 4 title 'TLB64', \
     filename using 1:5  with lines lw 2 lt 6 title 'TLB256', \
#plot filename using 1:2  with lines lw 2 lt 1  title 'TLB1', \
#     filename using 1:3  with lines lw 2 lt 2  title 'TLB2', \
#     filename using 1:4  with lines lw 2 lt 3  title 'TLB4', \
#     filename using 1:5  with lines lw 2 lt 4  title 'TLB8', \
#     filename using 1:6  with lines lw 2 lt 5  title 'TLB16', \
#     filename using 1:7  with lines lw 2 lt 6  title 'TLB32', \
#     filename using 1:8  with lines lw 2 lt 7  title 'TLB64', \
#     filename using 1:9  with lines lw 2 lt 8  title 'TLB128', \
#     filename using 1:10 with lines lw 2 lt 9  title 'TLB256', \
#     filename using 1:11 with lines lw 2 lt 10 title 'TLB512', \
#     filename using 1:12 with lines lw 2 lt 11 title 'TLB1024', \
#     filename using 1:13 with lines lw 2 lt 12 title 'TLB2048', \
