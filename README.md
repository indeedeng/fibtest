# Fibtest
Fibtest is a small C application that runs the fibonacci sequence and reports
how many iterations it completed.

It spawns fast threads and slow threads.  Fast threads run the sequence as fast
as possible.  Slow threads run 100 iterations and then sleep for 10ms.  Each
thread is pinned to it's corresponding cpu (thread 0 is on CPU 0, thread 2 on
CPU 2 etc...).

By default fibtest spawns one fast thread on CPU 0, and a number of slow
threads equal to the number of CPUS minus the number of fast threads

## Running fibtest
```
$ runfibtest 1; runfibtest
```

runfibtest optionally takes an arguement which is the total number of threads to spawn. With 
no argument it will spawn one fast thread and  the number of threads equal to the number of processors.


## License

[Apache License Version 2.0](https://github.com/indeedeng/throttletest/blob/master/LICENSE)
