#ifndef VERSION_H
#define VERSION_H

	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 4;
	static const long BUILD = 946;
	static const long REVISION = 256;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 6205;
	#define RC_FILEVERSION 0,4,946,256
	#define RC_FILEVERSION_STRING "0, 4, 946, 256\0"
	static const char FULLVERSION_STRING[] = "0.4.946.256";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 1344;
	

#endif //VERSION_H