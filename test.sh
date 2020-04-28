#!/bin/bash
PORT=9004

echo "LATENCY"
echo "directory"
./bin/thor.py -t 10 -h 10 "http://student01.cse.nd.edu:$PORT"
echo "static files"
./bin/thor.py -t 10 -h 10 "http://student01.cse.nd.edu:$PORT/song.txt"
echo "CGI Scripts"
./bin/thor.py -t 10 -h 10 "http://student01.cse.nd.edu:$PORT/cowsay.sh?message=stuff&template=flaming-sheep"

echo "Throughput"
echo "small"
./bin/thor.py -t 10 -h 1 "http://student01.cse.nd.edu:$PORT/html/index.html"
echo "medium"
./bin/thor.py -t 10 -h 1 "http://student01.cse.nd.edu:$PORT/images/a.png"
echo large
./bin/thor.py -t 10 -h 1 "http://student01.cse.nd.edu:$PORT/test.img"

