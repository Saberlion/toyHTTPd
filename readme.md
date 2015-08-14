#Benchmark
##ab -n 1000000 -c1000 http://127.0.0.1:10086/
This is ApacheBench, Version 2.3 <$Revision: 1528965 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 127.0.0.1 (be patient)
Completed 100000 requests
Completed 200000 requests
Completed 300000 requests
Completed 400000 requests
Completed 500000 requests
Completed 600000 requests
Completed 700000 requests
Completed 800000 requests
Completed 900000 requests
Completed 1000000 requests
Finished 1000000 requests


Server Software:        
Server Hostname:        127.0.0.1
Server Port:            10086

Document Path:          /
Document Length:        5 bytes

Concurrency Level:      1000
Time taken for tests:   40.877 seconds
Complete requests:      1000000
Failed requests:        0
Total transferred:      93000000 bytes
HTML transferred:       5000000 bytes
Requests per second:    24463.47 [#/sec] (mean)
Time per request:       40.877 [ms] (mean)
Time per request:       0.041 [ms] (mean, across all concurrent requests)
Transfer rate:          2221.78 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0   25 119.8     12    3029
Processing:     0   14  35.1     13    6540
Waiting:        0   10  34.9     10    6539
Total:          0   39 131.2     26    7542

Percentage of the requests served within a certain time (ms)
  50%     26
  66%     31
  75%     33
  80%     34
  90%     37
  95%     41
  98%     48
  99%   1015
 100%   7542 (longest request)
 
 
