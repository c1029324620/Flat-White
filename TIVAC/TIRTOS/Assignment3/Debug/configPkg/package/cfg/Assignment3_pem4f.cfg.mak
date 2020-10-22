# invoke SourceDir generated makefile for Assignment3.pem4f
Assignment3.pem4f: .libraries,Assignment3.pem4f
.libraries,Assignment3.pem4f: package/cfg/Assignment3_pem4f.xdl
	$(MAKE) -f C:\Users\c1029\workspace_v10\Assignment3/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\c1029\workspace_v10\Assignment3/src/makefile.libs clean

