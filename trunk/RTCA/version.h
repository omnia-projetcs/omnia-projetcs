#ifndef VERSION_H
#define VERSION_H

	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 4;
	static const long BUILD = 789;
	static const long REVISION = 350;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 5997;
	#define RC_FILEVERSION 0,4,789,350
	#define RC_FILEVERSION_STRING "0, 4, 789, 350\0"
	static const char FULLVERSION_STRING[] = "0.4.789.350";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 1187;
	

#endif //VERSION_H
