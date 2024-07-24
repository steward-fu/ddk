
		.data
		externdef _VMT_table___hla_:dword
		externdef _VMT_coroutine___hla_:dword
		externdef _VMT_file___hla_:dword
		externdef _VMT_mmap___hla_:dword
		externdef __imp__MessageBoxA@16:dword
		externdef _VMT_timer___hla_:dword
		externdef __imp__ExitProcess@4:dword
		externdef _VMT_node___hla_:dword
		externdef _VMT_list___hla_:dword

		.code
		externdef STDOUT_NEWLN:near32
		externdef DefaultExceptionHandler__hla_:near32
		externdef STDIN_READLN:near32
		externdef abstract__hla_:near32
		externdef HardwareException__hla_:near32
		externdef BuildExcepts__hla_:near32
		externdef STDOUT_PUTS:near32
		externdef Raise__hla_:near32
		externdef shortDfltExcept__hla_:near32

		public	MainPgmCoroutine__hla_
		public	DfltExHndlr__hla_
		public	_HLAMain
		public	HWexcept__hla_
