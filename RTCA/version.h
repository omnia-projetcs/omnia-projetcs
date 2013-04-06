#ifndef VERSION_H
#define VERSION_H

	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 5;
	static const long BUILD = 7;
	static const long REVISION = 5;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 6376;
	#define RC_FILEVERSION 0,5,7,5
	#define RC_FILEVERSION_STRING "0, 5, 7, 5\0"
	static const char FULLVERSION_STRING[] = "0.5.7.5";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 1473;
	

#endif //VERSION_H
