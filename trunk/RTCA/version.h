#ifndef VERSION_H
#define VERSION_H

	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 2;
	static const long BUILD = 28;
	static const long REVISION = 244;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 4048;
	#define RC_FILEVERSION 0,2,28,244
	#define RC_FILEVERSION_STRING "0, 2, 28, 244\0"
	static const char FULLVERSION_STRING[] = "0.2.28.244";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 1968;
	

#endif //VERSION_H
