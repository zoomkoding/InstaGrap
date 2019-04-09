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
	rm -f 21500670*

.PHONY: all clean test
