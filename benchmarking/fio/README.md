I was able to exceed the [IOPS spec for this NVMe drive][1] using these fio
configurations through a file interface (I used xfs, default params, on top of
the nvme block device) and watched the block device using iostat to verify
that the drive was reporting the same tps as fio.  A couple of considerations:

1. There's a test for sparse random writes (fio-rand-write), random reads
   (fio-rand-read), and random overwrites (fio-rand-rewrite). Sparse random
   writes are much slower than randomly writing to a preallocated (non-sparse)
   file because there's a bunch of file system overhead involved in maintaining
   sparse files.
2. IOPS should decrease as drives fill up, so I chose file sizes that filled
   the NVMe to around 50%.  I tested smaller files and it didn't make a
   difference though, so you are probably OK using much smaller files. I didn't
   test to see what would happen if the drives were >90% full though. I expect
   performance would drop fast.
3. I used 1 fio job per core (`numjobs=8`) and a moderately deep queue depth
   (`iodepth=64`) with Linux aio (`ioengine=libaio`) so, at max, the OS might be
   able to use up 512 (`numjobs` * `iodepth`) of the [1024 max outstanding I/Os][2]
   in the physical drive.

The best `iodepth` and `numjobs` will require experimentation and deciding what
is most realistic for user workloads. I think most HPC apps would do the
equivalent of `numjobs=176` (the total core count on a node) and `iodepth=1`,
but I don't think that would come close to maxing out the drive's IOPS
capability. If your goal is to show how fast an expert user can hope to go,
the config I shared above is probably a good place to start. Then try to
increase numjobs and iodepth to see how high things will go.

[1]: https://learn.microsoft.com/en-us/azure/virtual-machines/lsv3-series
[2]: https://learn.microsoft.com/en-us/azure/virtual-machines/linux/storage-performance
