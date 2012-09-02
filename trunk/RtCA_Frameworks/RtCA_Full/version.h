#ifndef VERSION_H
#define VERSION_H

	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 0;
	static const long BUILD = 251;
	static const long REVISION = 881;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 2945;
	#define RC_FILEVERSION 0,0,251,881
	#define RC_FILEVERSION_STRING "0, 0, 251, 881\0"
	static const char FULLVERSION_STRING[] = "0.0.251.881";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 1190;
	

#endif //VERSION_H
