set datafile separator ","
set key autotitle columnhead
splot \
"State.csv" u 1:2:3 with lines, \
"State.csv" u 4:5:6 with lines, \
"State.csv" u 7:8:9 with lines, \
"State.csv" u 10:11:12 with lines, \
"State.csv" u 13:14:15 with lines, \
"State.csv" u 16:17:18 with lines
