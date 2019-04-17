ALL:
	mkfifo fifo1 fifo2
	g++ processB.cpp  -o B
	g++ processB1.cpp -o B1
	g++ processA.cpp  -o A
	g++ processA1.cpp -o A1

clean:
	rm -rf A A1 B B1 fifo1 fifo2
