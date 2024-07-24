all:
	cd generic; make -f unix.make 
	cd Linux;   make -f unix.make 
	

clean:
	cd FreeBSD; make -f unix.make clean
	cd generic; make -f unix.make clean
	cd Linux;   make -f unix.make clean
	cd MacOSX;  make -f unix.make clean
	cd Win32;   make -f unix.make clean
	