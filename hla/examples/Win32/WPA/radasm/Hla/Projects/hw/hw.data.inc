MainPgmCoroutine__hla_ label	byte
		dword	offset32 MainPgmVMT__hla_
		dword	00h		;/* CurrentSP */
		dword	00h		;/* Pointer to stack */
		dword	00h		;/* ExceptionContext */
		dword	00h		;/* Pointer to last caller */
MainPgmVMT__hla_ label	dword
		dword	offset32 QuitMain__hla_
