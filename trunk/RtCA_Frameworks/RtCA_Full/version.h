#ifndef VERSION_H
#define VERSION_H

	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 0;
	static const long BUILD = 766;
	static const long REVISION = 800;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 3690;
	#define RC_FILEVERSION 0,0,766,800
	#define RC_FILEVERSION_STRING "0, 0, 766, 800\0"
	static const char FULLVERSION_STRING[] = "0.0.766.800";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 1705;
	

#endif //VERSION_H
