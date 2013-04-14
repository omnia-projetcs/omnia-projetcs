#ifndef VERSION_H
#define VERSION_H

	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 5;
	static const long BUILD = 224;
	static const long REVISION = 122;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 6710;
	#define RC_FILEVERSION 0,5,224,122
	#define RC_FILEVERSION_STRING "0, 5, 224, 122\0"
	static const char FULLVERSION_STRING[] = "0.5.224.122";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 1690;
	

#endif //VERSION_H
