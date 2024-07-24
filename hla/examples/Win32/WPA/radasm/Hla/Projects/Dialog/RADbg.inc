//;Use with RADbg.dll version 1.0.0.4 or higher

#macro RADbg (rabpid, rahwnd, ravar);

	pushd (rahwnd);
	pushd (rabpid);
	pushd (ravar);
	RADebug ();

#endmacro;

procedure RADebug;
@NOFRAME;
begin RADebug;

	pushfd;
	pushad;
	w.GetLastError ();
	pushd (eax);
	mov ([esp+52], ebx);
	pushd (esp);
	pushd (52);
	pushd (0);
	w.SendMessage (ebx, w.WM_COPYDATA, -1, esp);
	add (12, esp);
	if (eax = 0) then
		w.ExitProcess (0);
	endif;
	pop (eax);
	popad;
	popfd;
	ret (12);

end RADebug;
