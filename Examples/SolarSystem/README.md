# Solar System

## Equations of motion

TODO

## Building and Running
```console
$ ./makeRun.sh
```

## Plotting
Run the simulation with 

```console
dac/Examples/SolarSystem$ Executor/build/main.out -p ./State.csv -i 1000
```

After running the simulation, open Gnuplot and

```console
dac/Examples/SolarSystem$ gnuplot 
gnuplot> load "gnuplot.p"
```

## Optimization
You may use gcc's feedback directed optimization feature to optimize the code: 
* Choose some reasonably large number of iterations that doesn't take too long (e.g. 1000)
* compile and run the simulation with special gcc options (see below)
* GCC will create a run profile helping it to unroll loops and decide which branches are more or less likely
* Change the number of iterations to the large value you are actually interested in
* Recompile with gcc using the profile information
* It yields another ~5% speed increase.

```console
dac/Examples/SolarSystem/Executor$ make clean
dac/Examples/SolarSystem/Executor$ make BCONFIG=PROFILE_GENERATE
dac/Examples/SolarSystem/Executor$ /build/main.out
dac/Examples/SolarSystem/Executor$ make clean
dac/Examples/SolarSystem/Executor$ make BCONFIG=PROFILE_USE # This will create the faster binary, so change iteration number before this step
```

## Linux scheduler
If the binary is executed privilidged or the binary itself is granted sufficient rights

```console
dac/Examples/SolarSystem/Executor$ sudo setcap 'cap_sys_nice=eip' build/main.out
```

all threads will be run at highest priority using SCHED_FIFO.
