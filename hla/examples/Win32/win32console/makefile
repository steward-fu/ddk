inc=..\..\include
incs=win32cons.hhf

..\wincons.lib: 																	\
	a_getRect.obj       clrtoeoln.obj       clrtoeoscrn.obj     cls.obj				\
	deleteChars.obj     deletelines.obj     deleteLn.obj							\
	fillrect.obj        fillRectAttr.obj    getc.obj            getRect.obj			\
	gets.obj            getxy.obj           gotoxy.obj          info.obj			\
	input.obj           insertChars.obj     insertLines.obj     insertLn.obj		\
	mode.obj            NumButtons.obj      putRect.obj         puts.obj			\
	putsx.obj           ScrollDn.obj        scrolldnN.obj       scrolldnrect.obj	\
	scrollDnRectV.obj   scrolldnx.obj       scrollup.obj        scrollupn.obj		\
	scrolluprect.obj    scrolluprectv.obj   scrollupx.obj       setOutputAttr.obj	\
	title.obj
	lib @console.txt
	
	
	

a_getRect.obj:	   a_getRect.hla     $(incs)
	hla -p:temp -c a_getRect.hla

clrtoeoln.obj:	   clrtoeoln.hla	 $(incs)
	hla -p:temp -c clrtoeoln.hla

clrtoeoscrn.obj:   clrtoeoscrn.hla	 $(incs) 
	hla -p:temp -c clrtoeoscrn.hla

cls.obj:		   cls.hla	  		 $(incs)
	hla -p:temp -c cls.hla

deleteChars.obj:   deleteChars.hla	 $(incs) 
	hla -p:temp -c deleteChars.hla

deletelines.obj:   deletelines.hla	 $(incs) 
	hla -p:temp -c deletelines.hla

deleteLn.obj:	   deleteLn.hla		 $(incs)
	hla -p:temp -c deleteLn.hla

fillrect.obj:	   fillrect.hla		 $(incs)
	hla -p:temp -c fillrect.hla

fillRectAttr.obj:  fillRectAttr.hla	 $(incs)
	hla -p:temp -c fillRectAttr.hla

getc.obj:		   getc.hla			 $(incs)
	hla -p:temp -c getc.hla

getRect.obj:	   getRect.hla	  	 $(incs)
	hla -p:temp -c getRect.hla

gets.obj:		   gets.hla			 $(incs)
	hla -p:temp -c gets.hla

getxy.obj:		   getxy.hla		 $(incs)
	hla -p:temp -c getxy.hla

gotoxy.obj:		   gotoxy.hla		 $(incs)
	hla -p:temp -c gotoxy.hla

info.obj:		   info.hla			 $(incs)
	hla -p:temp -c info.hla

input.obj:		   input.hla		 $(incs)
	hla -p:temp -c input.hla

insertChars.obj:   insertChars.hla	 $(incs) 
	hla -p:temp -c insertChars.hla

insertLines.obj:   insertLines.hla	 $(incs) 
	hla -p:temp -c insertLines.hla

insertLn.obj:	   insertLn.hla		 $(incs)
	hla -p:temp -c insertLn.hla

mode.obj:		   mode.hla			 $(incs)
	hla -p:temp -c mode.hla

NumButtons.obj:	   NumButtons.hla	 $(incs)
	hla -p:temp -c NumButtons.hla

putRect.obj:	   putRect.hla	  	 $(incs)
	hla -p:temp -c putRect.hla

puts.obj:		   puts.hla			 $(incs)
	hla -p:temp -c puts.hla

putsx.obj:		   putsx.hla		 $(incs)
	hla -p:temp -c putsx.hla

ScrollDn.obj:	   ScrollDn.hla		 $(incs)
	hla -p:temp -c ScrollDn.hla

scrolldnN.obj:	   scrolldnN.hla	 $(incs)
	hla -p:temp -c scrolldnN.hla

scrolldnrect.obj:  scrolldnrect.hla	 $(incs)
	hla -p:temp -c scrolldnrect.hla

scrollDnRectV.obj: scrollDnRectV.hla $(incs)
	hla -p:temp -c scrollDnRectV.hla

scrolldnx.obj:	   scrolldnx.hla	 $(incs)
	hla -p:temp -c scrolldnx.hla

scrollup.obj:	   scrollup.hla		 $(incs)
	hla -p:temp -c scrollup.hla

scrollupn.obj:	   scrollupn.hla	 $(incs)
	hla -p:temp -c scrollupn.hla

scrolluprect.obj:  scrolluprect.hla	 $(incs)
	hla -p:temp -c scrolluprect.hla

scrolluprectv.obj: scrolluprectv.hla $(incs)
	hla -p:temp -c scrolluprectv.hla

scrollupx.obj:	   scrollupx.hla	 $(incs)
	hla -p:temp -c scrollupx.hla

setOutputAttr.obj: setOutputAttr.hla $(incs)
	hla -p:temp -c setOutputAttr.hla

title.obj:		   title.hla		 $(incx)
	hla -p:temp -c title.hla


clean:
	delete *.obj
	delete *.lib
	delete *.bak
	delete finder.dat
	delete temp\*.*

