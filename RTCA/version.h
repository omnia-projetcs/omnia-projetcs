#ifndef VERSION_H
#define VERSION_H

	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "b";
	
	//Standard Version Type
	static const long MAJOR  = 0;
	static const long MINOR  = 7;
	static const long BUILD  = 248;
	static const long REVISION  = 733;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1872;
	#define RC_FILEVERSION 0,7,248,733
	#define RC_FILEVERSION_STRING "0, 7, 248, 733\0"
	static const char FULLVERSION_STRING [] = "0.7.248.733";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 1639;
	

#endif //VERSION_H
