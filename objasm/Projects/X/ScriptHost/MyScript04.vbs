' List All WMI Namespaces
Output.Clear
strComputer = "."
Call EnumNameSpaces("root")
 
Sub EnumNameSpaces(strNameSpace)
    Output.Write strNameSpace & vbCRLF
    Set objWMIService=GetObject _
        ("winmgmts:{impersonationLevel=impersonate}\\" & _ 
            strComputer & "\" & strNameSpace)

    Set colNameSpaces = objWMIService.InstancesOf("__NAMESPACE")

    For Each objNameSpace In colNameSpaces
        Call EnumNameSpaces(strNameSpace & "\" & objNameSpace.Name)
    Next
End Sub 