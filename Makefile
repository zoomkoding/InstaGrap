all : submitter instagrapd worker
submitter : submitter.c
	gcc -o submitter submitter.c
instagrapd : instagrapd.c
	gcc -o instagrapd instagrapd.c
worker : worker.c
	gcc -o worker worker.c

clean:
	rm -f submitter
	rm -f instagrapd
	rm -f worker
	rm -f input.in
	rm -f output.out
	rm -f test.c
	rm -f test

