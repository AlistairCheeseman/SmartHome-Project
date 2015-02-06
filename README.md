SmartHome-Project
=================

Final Year degree project to create a set of devices to create a smart home.

Hardware files are created in eagle with PDFs of finished pcbs labelled with which pcb layer they are. Hardware files are created from scratch and a custom library has been created for components not in the default eagle library.

Software has three sections.

	-	www
		copy this to the production web server. the files are based on php 5.6 HTML 5, using ajax and highcharts charting components (http://www.highcharts.com/ - free for non commercial uses).
		
	-	LFS
		This contains a set of manually written build scripts (toolchain and Filesystem) and pre-writted configuration files. After both files (install-*.sh) have been executed on a linux machine there will be a working Linux filesystem. Be sure to change configuration file (buildscript.sh) variables to suit your setup!

	-	Apps
		This contains source for all custom written applications on the server.  
