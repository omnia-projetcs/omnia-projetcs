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
   if (name[0] != '\\' && name[1] != '\\')return TRUE;
 }
 return FALSE;
}
