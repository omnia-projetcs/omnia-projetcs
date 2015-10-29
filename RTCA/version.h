#ifndef VERSION_H
#define VERSION_H

	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "b";
	
	//Standard Version Type
	static const long MAJOR  = 0;
	static const long MINOR  = 6;
	static const long BUILD  = 779;
	static const long REVISION  = 268;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1160;
	#define RC_FILEVERSION 0,6,779,268
	#define RC_FILEVERSION_STRING "0, 6, 779, 268\0"
	static const char FULLVERSION_STRING [] = "0.6.779.268";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 1169;
	

#endif //VERSION_H
