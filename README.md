# Fibtest

![NetflixOSS Lifecycle](https://img.shields.io/osslifecycle/indeedeng/fibtest.svg)

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
$ ./runfibtest 1; ./runfibtest
```

runfibtest optionally takes an arguement which is the total number of threads to spawn. With
no argument it will spawn one fast thread and  the number of threads equal to the number of processors.

It returns the number of iterations of the fibonacci sequence it was able to accomplish, as well as how
long it was throttled and the corresponding cpu usage that was used.

## Code of Conduct
This project is governed by the [Contributor Covenant v 1.4.1](CODE_OF_CONDUCT.md)

## License

[Apache License Version 2.0](https://github.com/indeedeng/throttletest/blob/master/LICENSE)
