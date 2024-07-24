dll.dll: dll.obj
	polink /map dll.obj @dll.link

dll.obj: dll.hla
	fhla -@ -c dll.hla 