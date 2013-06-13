#ifndef VERSION_H
#define VERSION_H

	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 6;
	static const long BUILD = 576;
	static const long REVISION = 114;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 7210;
	#define RC_FILEVERSION 0,6,576,114
	#define RC_FILEVERSION_STRING "0, 6, 576, 114\0"
	static const char FULLVERSION_STRING[] = "0.6.576.114";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 42;
	

#endif //VERSION_H
