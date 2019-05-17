#!/bin/bash
pagesizes='128 256 512 1024'
tlbsizes='16 32 64 256'
inputs='100 1000 10000'
policies='FIFO LRU'
programs='mergesort heapsort quicksort'
performance='performance.csv'

rm PLOT*U.*
make clean
rm PLOT*O.*
make

for input in $inputs
do
  echo "$input"
  for program in $programs
  do
    echo "$program"
    for policy in $policies
    do
      echo "$policy"
      > PLOT$program$input$policy.csv
      for pagesize in $pagesizes
      do
        echo "page $pagesize"
        echo -n $pagesize >> PLOT$program$input$policy.csv
        for tlbsize in $tlbsizes
        do
          echo "tlb $tlbsize"
          valgrind --tool=lackey --trace-mem=yes ./programs/$program $input 2>&1 | ./valtlb379    -p $policy $pagesize $tlbsize > $performance
          x=$(head -n 1 $performance)
          y=$(tail -n 1 $performance)
          rate=$(echo $y/$x | bc -l)
          echo -n ,0$rate >> PLOT$program$input$policy.csv
        done
        echo "" >> PLOT$program$input$policy.csv
      done
      gnuplot -e "filename='PLOT$program$input$policy.csv'" plotter.gp > PLOT$program$input$policy.png
    done
  done
done
