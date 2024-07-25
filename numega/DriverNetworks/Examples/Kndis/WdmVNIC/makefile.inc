$(O)\wdmvnic.mof: wdmvnic.mof

$(O)\wdmvnic_wmi.h: $(O)\wdmvnic.bmf 
    wmimofck -h$(O)\wdmvnic_wmi.h $(O)\wdmvnic.bmf
