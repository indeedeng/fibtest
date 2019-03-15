# Throttletest
This package is meant to be a collections of tests that are used for
reproducing the Cgroup CPU accounting throttling issues as described here

## fibtest
Fibtest is a small C application that runs the fibonacci sequence and reports
how many iterations it completed.

It spawns fast threads and slow threads.  Fast threads run the sequence as fast
as possible.  Slow threads run 100 iterations and then sleep for 10ms.  Each
thread is pinned to it's corresponding cpu (thread 0 is on CPU 0, thread 2 on
CPU 2 etc...).

### Running fibtest
`
$ make clean; make
$ runfibtest
`

## License

[Apache License Version 2.0](https://github.com/indeedeng/throttletest/blob/master/LICENSE)
