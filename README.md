# Instantly Grading Programming

submitter command

./submitter -n 127.0.0.1:8090 -u 21500670 -k 12345678 solution_compiletime.c
./submitter -n 127.0.0.1:8090 -u 21500670 -k 12345678 solution_runtime.c
./submitter -n 127.0.0.1:8090 -u 21500670 -k 12345678 solution_timeout.c
./submitter -n 127.0.0.1:8090 -u 21500670 -k 12345678 solution.c
./submitter -n 127.0.0.1:8090 -u 21700105 -k 12345678 solution.c



instagrapd command

./instagrapd -p 8090 -w 127.0.0.1:8091 ./testcases

worker command

./worker -p 8091
