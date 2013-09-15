//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "../resources.h"
//------------------------------------------------------------------------------
BOOL ResDNS(char *ip, char *name, unsigned int sz_max)
{
 name[0]=0;
 struct hostent* remoteHost;
 struct in_addr in;
 in.s_addr = inet_addr(ip);
 if ((remoteHost=gethostbyaddr((char *)&in, 4, AF_INET))!=0)
 {
   snprintf(name,sz_max,"%s",remoteHost->h_name);
   return TRUE;
 }
 return FALSE;
}
