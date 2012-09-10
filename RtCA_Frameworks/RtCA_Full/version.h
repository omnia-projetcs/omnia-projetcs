#ifndef VERSION_H
#define VERSION_H

	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 0;
	static const long BUILD = 425;
	static const long REVISION = 772;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 3239;
	#define RC_FILEVERSION 0,0,425,772
	#define RC_FILEVERSION_STRING "0, 0, 425, 772\0"
	static const char FULLVERSION_STRING[] = "0.0.425.772";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 1364;
	

#endif //VERSION_H
