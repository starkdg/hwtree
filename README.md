# HWTree

An index structure for indexing objects in 64-dim hamming space.
Objects are indexed by 64-bit integers with the ability to insert
and search for all nearest neighbor objects within a given radius
of a target value.  

## Performance

Performance chart for HWTree for varying index sizes. Build time
is normalized to the size of the index, so elapsed time is average
insert time.  All stats are averaged  over 5 runs.  Queries are the
results of a range search with radius = 10. Query operations are the
average number of distance operations computed per query (norm'd to
the size of the index).  


| Size |  MEM   |  Avg. Insert Time | Query Opers | Avg. Query Time |
|------|--------|-------------------|-------------|-----------------|
| 100K |   9MB  |     867ns    |    31.6%    |    5.61ms   |
| 200K |  18MB  |   1.01&mu;s  |    34.5%    |   13.18ms   |
| 400K |  33MB  |   1.15&mu;s  |    29.3%    |   25.45ms   |
| 800K |  62MB  |   1.27&mu;s  |    26.8%    |   44.65ms   |
|  1M  |  76MB  |   1.31&mu;s  |    27.2%    |   55.78ms   |
|  2M  | 149MB  |   1.46&mu;s  |    21.3%    |   98.67ms   |
|  4M  | 312MB  |   1.67&mu;s  |    16.7%    |  215.98ms   |
|  8M  | 670MB  |   1.96&mu;s  |    11.3%    |  439.51ms   |
| 16M  | 1.4GB  |   2.26&mu;s  |     7.3%    |  871.34ms   |



Queries for range searches with radius from 0 to 12. (Index size is 4M)

| Radius |  Query Operations | Avg. Query Time |
|--------|-------------------|-----------------|
|   0    |    0.0003%  | 62.4&mu;s |
|   2    |    0.0060%  | 1.66ms  |
|   4    |    0.15%    | 14.30ms |
|   6    |    1.25%    | 46.30ms  |
|   8    |    5.10%    | 101.30ms  |
|  10    |   16.7%     | 213.75ms |
|  12    |   32.6%     | 311.56ms  |





## Install

```
cmake .
make
make test
```
