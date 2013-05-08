#ifndef VERSION_H
#define VERSION_H

	//Software Status
	static const char STATUS[] = "Alpha";
	static const char STATUS_SHORT[] = "a";
	
	//Standard Version Type
	static const long MAJOR = 0;
	static const long MINOR = 5;
	static const long BUILD = 423;
	static const long REVISION = 280;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT = 7034;
	#define RC_FILEVERSION 0,5,423,280
	#define RC_FILEVERSION_STRING "0, 5, 423, 280\0"
	static const char FULLVERSION_STRING[] = "0.5.423.280";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY = 1889;
	

#endif //VERSION_H
