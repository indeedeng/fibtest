# Fibtest

![OSS Lifecycle](https://img.shields.io/osslifecycle/indeedeng/fibtest.svg)

Fibtest is a small C application that runs the fibonacci sequence and reports
how many iterations it completed.

It spawns fast threads and slow threads.  Fast threads run the sequence as fast
as possible.  Slow threads run 100 iterations and then sleep for 10ms.  Each
thread is pinned to it's corresponding cpu (thread 0 is on CPU 0, thread 2 on
CPU 2 etc...).

By default fibtest spawns one fast thread on CPU 0, and a number of slow
threads equal to the number of CPUS minus the number of fast threads.

This test can be used to check for the CFS scheduler bug fixed by

- [`de53fd7ae`](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=de53fd7aedb100f03e5d2231cfce0e4993282425)
  sched/fair: Fix low cpu usage with high throttling by removing expiration of cpu-local slices 
  
For a detailed explanation of the bug being tested and its fix, 
see [blog post 1](https://engineering.indeedblog.com/blog/2019/12/unthrottled-fixing-cpu-limits-in-the-cloud/),
[blog post 2](https://engineering.indeedblog.com/blog/2019/12/cpu-throttling-regression-fix/),
and video of [Dave Chiluk's presentation at KubeCon 2019](https://youtu.be/UE7QX98-kO0).

The important thing to understand is that this bug affects performance in proportion
to the number of cores on the machine. This test will not be effective at demonstrating 
the bug on a 4 core machine. At least 32 cores are recommended.

## Running fibtest

- You need to be running as root or be able to `sudo` without entering a password.
- You need `bash`, `git`, `make`, `gcc`, and `libc-devel` installed.
- You must be running on the host, not inside a Docker container or Kubernetes pod. 
It is OK if you are running in a VM, such as a cloud server. 
- You must be running a Linux family kernel. This will not work on Windows or 
macOS/Darwin/Xnu

1. Clone this git repository and `cd` into it. Check which branch you are interested in
and possibly `git checkout <branch>`. 
2. Run `getconf _NPROCESSORS_ONLN` which reports the number of CPUs available. The test 
may not produce accurate or conclusive results with fewer than 8 CPUs. The more CPUs the better.
3. Run `./runfibtest 1`. This will compile the fibtest binary if needed. If you run
into issues, you may need to install missing components using `apt-get install` or `yum isntall`
or something simlilar depending on your OS.

The results of `./runfibtest 1` should be something like this:
```
Iterations Completed(M): 1573 
Throttled for: 51 
CPU Usage (msecs) = 501
```

- "Interations Completed(M)" is relatively unimportant. It mainly has to do with the 
CPU clockspeed. 
- "Throttled for: 51" is expected. This test runs a single thread for 50 periods at full speed with a 
quota of 0.1 CPUs (100m), so it is expected to be throttled for 50 or 51 periods depending on how things
line up.
- "CPU Usage (msecs) = 501" is a baseline number. We would expect it to be 500 because we are running
100m CPU for 5 seconds which adds up to 500ms. 

The above numbers establish a baseline that the test is working correctly and no extraneous issues are
inerfering with the test.

Now we run the real test:
```
./runfibtest
```

This runs a single fast thread, plus 1 slow thread on every core but one. This demonstrates the bug, in that 
the slow threads each end up stealing 1ms per period from the fast thread, without using up 
subtantial amounts of CPU time themselves. You should see output like this on an affected machine:
```
Iterations Completed(M): 150 
Throttled for: 58 
CPU Usage (msecs) = 88
```
This test was from an affected kernel on a machine with 64 CPUs.
Note that the number of Iterations completed and the CPU Usage has dropped considerably from the 
previous test, completely contrary to expectations. 

The same test, on the same machine, using a patched kernel, produced this result:
```
Iterations Completed(M): 1209 
Throttled for: 51 
CPU Usage (msecs) = 493
```
Note there is a small decline in CPU usage from 501 to 493 ms, which can be attributed to the
extra kernel work keeping track of 64 threads. Still, the number of throttles periods remained 51
and the decline in CPU usage is less than 2%. The susbstantial decline in Interations Completed 
is a more complex topic, beyond the scope of this particular bug.

## Code of Conduct
This project is governed by the [Contributor Covenant v 1.4.1](CODE_OF_CONDUCT.md)

## License

[Apache License Version 2.0](https://github.com/indeedeng/throttletest/blob/master/LICENSE)
