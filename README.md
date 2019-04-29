# Instantly Grading Programming

submitter command

./submitter -n 127.0.0.1:8090 -u 21500670 -k 12345678 submit.c

instagrapd command

./instagrapd -p 8090 -w 127.0.0.1:8091 ./testcase

worker command

./worker -p 8091


submitter가 instagrapd한테 code 보낼때 

send@21500670@12345678@code내용

instagrapd
