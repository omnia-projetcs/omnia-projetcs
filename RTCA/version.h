#ifndef VERSION_H
#define VERSION_H

	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "b";
	
	//Standard Version Type
	static const long MAJOR  = 0;
	static const long MINOR  = 7;
	static const long BUILD  = 190;
	static const long REVISION  = 411;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1785;
	#define RC_FILEVERSION 0,7,190,411
	#define RC_FILEVERSION_STRING "0, 7, 190, 411\0"
	static const char FULLVERSION_STRING [] = "0.7.190.411";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 1581;
	

#endif //VERSION_H
