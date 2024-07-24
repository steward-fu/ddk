all:
	cd generic
	make -f win32.make
	cd ..
	
	cd Win32
	make
	cd ..
	

clean:
	cd FreeBSD
	make -f win32.make clean
	cd ..
	
	cd generic
	make -f win32.make clean
	cd ..
	
	cd Linux
	make -f win32.make clean
	cd ..
	
	cd MacOSX
	make -f win32.make clean
	cd ..
	
	cd Win32
	make clean
	cd ..
	