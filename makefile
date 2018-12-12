

check:
	gcc -g -fno-stack-protector -fPIC -o libckpt.o -c libckpt.c	
	gcc -shared -g -fno-stack-protector -o libckpt.so libckpt.o
	gcc -fno-stack-protector -o myProg myProg.c
	(sleep 5 && kill -2 `pgrep -n myProg` && sleep 2 && make restart) & LD_PRELOAD=/home/prashant/myCkpt/libckpt.so ./myProg
	
restart:
	gcc -static -g -fno-stack-protector -Wl,-Ttext-segment=5000000 -Wl,-Tdata=5100000 -Wl,-Tbss=5200000 -o testForRestart testForRestart.c 
	./testForRestart myckpt.bin
	
