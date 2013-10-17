#ifndef VERSION_H
#define VERSION_H

	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 6;
	static const long BUILD = 4;
	static const long REVISION = 0;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 0;
	#define RC_FILEVERSION 0,6,4,0
	#define RC_FILEVERSION_STRING "0, 6, 4, 0\0"
	static const char FULLVERSION_STRING[] = "0.6.4.0";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 394;
	

#endif //VERSION_H