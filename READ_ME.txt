Das Program ist folgendermaßen aufgebaut:
	- lies ein ppm-file in ein array ein (nur P3 Format)
	- danach wird Speicher für ein weiteres array allokiert (calloc()->init all 0)
	  das größer ist (ist quasi das original Bild + Rahmen)
	- Kernel kann entweder eine 3x3 oder 5x5 Eingabe sein
