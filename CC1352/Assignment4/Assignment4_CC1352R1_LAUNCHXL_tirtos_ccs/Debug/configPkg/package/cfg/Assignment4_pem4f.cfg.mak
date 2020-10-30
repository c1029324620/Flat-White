# invoke SourceDir generated makefile for Assignment4.pem4f
Assignment4.pem4f: .libraries,Assignment4.pem4f
.libraries,Assignment4.pem4f: package/cfg/Assignment4_pem4f.xdl
	$(MAKE) -f C:\Users\c1029\workspace_v10\Assignment4_CC1352R1_LAUNCHXL_tirtos_ccs/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\c1029\workspace_v10\Assignment4_CC1352R1_LAUNCHXL_tirtos_ccs/src/makefile.libs clean

