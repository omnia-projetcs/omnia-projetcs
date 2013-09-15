//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "../resources.h"
//----------------------------------------------------------------
//http://msdn.microsoft.com/library/default.asp?url=/library/en-us/iphlp/iphlp/sendarp.asp
BOOL __cdecl ARP(char * ip,char *mac_dst)
{
    //variables
    IPAddr  ipAddr;
    ULONG   pulMac[2];
    ULONG   ulLen;

    //transformation pour traitement de l'adresse ip
    ipAddr = inet_addr(ip);

    //init
    memset (pulMac, 0xff, sizeof (pulMac));
    ulLen = 6;

    //on récupère l'adresse mac ici si possible (si après un routage type internet = impossible)
    if (SendARP (ipAddr, 0, pulMac, &ulLen)==NO_ERROR)
    {
      PBYTE pbHexMac = (PBYTE) pulMac;
      snprintf (mac_dst,18,"%02X:%02X:%02X:%02X:%02X:%02X",pbHexMac[0],pbHexMac[1],pbHexMac[2],pbHexMac[3],pbHexMac[4],pbHexMac[5],pbHexMac[6]);
      return 1;
    }
    return 0;
}
//----------------------------------------------------------------
