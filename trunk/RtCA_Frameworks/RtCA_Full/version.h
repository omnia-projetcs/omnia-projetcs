#ifndef VERSION_H
#define VERSION_H

	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 0;
	static const long BUILD = 196;
	static const long REVISION = 567;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 2874;
	#define RC_FILEVERSION 0,0,196,567
	#define RC_FILEVERSION_STRING "0, 0, 196, 567\0"
	static const char FULLVERSION_STRING[] = "0.0.196.567";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 1135;
	

#endif //VERSION_H
