Please refer the slide deck for basic overview of the project

Software Configuration
	Code running on Blimp - blimp_control.ino
	Code running on Arduino Nano ble 33 as mesh proxy - Mesh_Bridge.ino
	App running on android phone for blimp control - Meshengerr.apk
	Webserver to calculate Mesh message : open cmd in Mesh_Message_Compute folder and run ws -https 
	Python script to patch the web server : patch_app.py (keep the .json file from nrf mesh app in the same directory)
	Refer nRF_Mesh_Nodes.zip for codes of mesh nodes
	
Hardware Configuration 
	Refer slide deck for schematics of Mesh Nodes
	Refer Schematic_Nano_Drone_Controller.pdf for schematic of the drone
	