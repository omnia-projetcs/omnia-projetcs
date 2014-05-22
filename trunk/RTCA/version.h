#ifndef VERSION_H
#define VERSION_H

	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "b";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 6;
	static const long BUILD = 194;
	static const long REVISION = 14;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 248;
	#define RC_FILEVERSION 0,6,194,14
	#define RC_FILEVERSION_STRING "0, 6, 194, 14\0"
	static const char FULLVERSION_STRING[] = "0.6.194.14";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 584;
	

#endif //VERSION_H
