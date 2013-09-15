//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "../resources.h"
//----------------------------------------------------------------
int Ping(char *ip)
{
  int Ttl = -1;
  if (IcmpOk)
  {
    HANDLE hndlFile = pIcmpCreateFile();
    if (hndlFile != INVALID_HANDLE_VALUE)
    {
      LPHOSTENT pHost     = gethostbyname(ip);
      DWORD *dwAddress    = (DWORD *)(*pHost->h_addr_list);
      ICMPECHO icmpEcho;

      if (pIcmpSendEcho(hndlFile,*dwAddress,0,0,0,&icmpEcho,sizeof(icmpEcho),ICMP_TIMEOUT)!=0)
      {
        //existe
        if ((icmpEcho.Status==0)&&(icmpEcho.Options.Ttl>0))
        {
          Ttl = icmpEcho.Options.Ttl;
        }
      }

      pIcmpCloseHandle(hndlFile);
      return  Ttl;
    }
  }
  return Ttl;
}
