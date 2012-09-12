#ifndef VERSION_H
#define VERSION_H

	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 0;
	static const long BUILD = 512;
	static const long REVISION = 274;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 3333;
	#define RC_FILEVERSION 0,0,512,274
	#define RC_FILEVERSION_STRING "0, 0, 512, 274\0"
	static const char FULLVERSION_STRING[] = "0.0.512.274";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 1451;
	

#endif //VERSION_H
