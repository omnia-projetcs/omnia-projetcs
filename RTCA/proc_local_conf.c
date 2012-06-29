//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
void GetLocalDisk(char *path) //csv
{
  HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
  if (MyhFile != INVALID_HANDLE_VALUE)
  {
    char tmp[MAX_PATH], tmp_path[5]="_:\\\0", tempory[MAX_PATH];
    int i,nblecteurs = GetLogicalDriveStrings(MAX_PATH,tmp);
    DWORD FileMaxLen, FileFlags, cluster, secteur, Cluster_libre, Total_Cluster;
    DWORD copiee;

    if (nblecteurs>0)
    {
      WriteFile(MyhFile,"Drive;Type;Read_only;Compressed;Free space;\r\n",strlen("Drive;Type;Read_only;Compressed;Free space;\r\n"),&copiee,0);

      for (i=0;i<nblecteurs;i+=4)
      {
        tmp_path[0] = tmp[i];
        switch(GetDriveType(&tmp[i]))
        {
          case DRIVE_REMOVABLE:snprintf(tempory,MAX_PATH,"%s;DRIVE_REMOVABLE;",tmp_path);WriteFile(MyhFile,tempory,strlen(tempory),&copiee,0);break;
          case DRIVE_FIXED:snprintf(tempory,MAX_PATH,"%s;DRIVE_FIXED;",tmp_path);WriteFile(MyhFile,tempory,strlen(tempory),&copiee,0);break;
          case DRIVE_REMOTE:snprintf(tempory,MAX_PATH,"%s;DRIVE_REMOTE;",tmp_path);WriteFile(MyhFile,tempory,strlen(tempory),&copiee,0);break;
          case DRIVE_CDROM:snprintf(tempory,MAX_PATH,"%s;DRIVE_CDROM;",tmp_path);WriteFile(MyhFile,tempory,strlen(tempory),&copiee,0);break;
          case DRIVE_RAMDISK:snprintf(tempory,MAX_PATH,"%s;DRIVE_RAMDISK;",tmp_path);WriteFile(MyhFile,tempory,strlen(tempory),&copiee,0);break;
        }

        //type de système de fichier
        FileMaxLen = 10;
        FileFlags = 0;
        tempory[0]=0;
        if (GetVolumeInformation(tmp_path,0,10,0,&FileMaxLen, &FileFlags,tempory,10)!=0)
        {
          if (FileFlags & FILE_READ_ONLY_VOLUME == FILE_READ_ONLY_VOLUME)strncat(tempory,";READ_ONLY;\0",MAX_PATH);
          else strncat(tempory,";;\0",MAX_PATH);
          if (FileFlags & FILE_VOLUME_IS_COMPRESSED == FILE_VOLUME_IS_COMPRESSED)strncat(tempory,";COMPRESSED;\0",MAX_PATH);
          else strncat(tempory,";;\0",MAX_PATH);

          WriteFile(MyhFile,tempory,strlen(tempory),&copiee,0);
        }else WriteFile(MyhFile,";;;",strlen(";;;"),&copiee,0);

        //taille
        cluster =0;
        secteur =0;
        Cluster_libre =0;
        Total_Cluster =0;
        if (GetDiskFreeSpace(&tmp[i],&cluster,&secteur,&Cluster_libre,&Total_Cluster)!=0)
        {
          snprintf(tempory,MAX_PATH,"%1.2f Go/%1.2f Go;\r\n",
               ((Cluster_libre * cluster)/1024.0* secteur)/(1024.0*1024.0),(((Total_Cluster * cluster)/1024.0)*secteur)/(1024.0*1024.0));
          WriteFile(MyhFile,tempory,strlen(tempory),&copiee,0);
        }else WriteFile(MyhFile,";\r\n",strlen(";\r\n"),&copiee,0);
      }
    }
  }
  CloseHandle(MyhFile);
}
//------------------------------------------------------------------------------
void GetRoutingTable(char *path)//csv
{
  HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
  if (MyhFile != INVALID_HANDLE_VALUE)
  {
    PMIB_IPFORWARDTABLE pIpForwardTable;
    DWORD dwSize = 0;
    char tmp[MAX_PATH];

    struct in_addr IpAddr_dst;
    struct in_addr IpAddr_msk;
    struct in_addr IpAddr_hop;

    int i;
    HMODULE hDLL,hDLL2;
    BOOL err =0;

    typedef char* (WINAPI *INET_NTOA)(struct in_addr in);
    INET_NTOA inet_ntoa;

    typedef DWORD (WINAPI *GETIPFORWARDTABLE)(PMIB_IPFORWARDTABLE pIpForwardTable, PULONG pdwSize, BOOL bOrder);
    GETIPFORWARDTABLE GetIpForwardTable;

    hDLL = LoadLibrary( "WS2_32.DLL" );
    hDLL2 = LoadLibrary( "IPHLPAPI.DLL" );
    DWORD copiee;

    if (hDLL && hDLL2)
    {
       inet_ntoa = (INET_NTOA) GetProcAddress(hDLL,"inet_ntoa");
       GetIpForwardTable = (GETIPFORWARDTABLE) GetProcAddress(hDLL2,"GetIpForwardTable");

       if (inet_ntoa && GetIpForwardTable)
       {
         pIpForwardTable = (MIB_IPFORWARDTABLE*) HeapAlloc(GetProcessHeap(), 0, (sizeof(MIB_IPFORWARDTABLE)));
         if ( pIpForwardTable != NULL)
         {
           if (GetIpForwardTable(pIpForwardTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER)
           {
             HeapFree(GetProcessHeap(), 0,pIpForwardTable);
             pIpForwardTable = (MIB_IPFORWARDTABLE*) HeapAlloc(GetProcessHeap(), 0,dwSize);
           }

           if ( pIpForwardTable != NULL)
           {
            if (GetIpForwardTable(pIpForwardTable, &dwSize, 0) == NO_ERROR)
            {
                WriteFile(MyhFile,"Destination;Mask;Gateway;Metrique;\r\n",strlen("Destination;Mask;Gateway;Metrique;\r\n"),&copiee,0);
                for (i = 0; i < (int) pIpForwardTable->dwNumEntries; i++)
                {
                   IpAddr_dst.S_un.S_addr = (u_long) pIpForwardTable->table[i].dwForwardDest;
                   IpAddr_msk.S_un.S_addr = (u_long) pIpForwardTable->table[i].dwForwardMask;
                   IpAddr_hop.S_un.S_addr = (u_long) pIpForwardTable->table[i].dwForwardNextHop;
                   snprintf(tmp,MAX_PATH,"%s;%s;%s;%d;\r\n",inet_ntoa(IpAddr_dst)
                                                           ,inet_ntoa(IpAddr_msk)
                                                           ,inet_ntoa(IpAddr_hop)
                                                           ,pIpForwardTable->table[i].dwForwardMetric1);

                   WriteFile(MyhFile,tmp,strlen(tmp),&copiee,0);
                }
            }
            HeapFree(GetProcessHeap(), 0,pIpForwardTable);
           }
         }
       }
       FreeLibrary(hDLL);
       FreeLibrary(hDLL2);
    }
  }
  CloseHandle(MyhFile);
}
//------------------------------------------------------------------------------
void GetShare(char *path)//csv
{
  HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
  if (MyhFile != INVALID_HANDLE_VALUE)
  {
    DWORD copiee;
    PSHARE_INFO_502 Buffer,p;
    NET_API_STATUS res;
    DWORD nb=0,tr=0,i;
    char tmp[MAX_PATH];
    HMODULE hDLL;
    typedef NET_API_STATUS (WINAPI *NETAPIBUFFERFREE)(LPVOID Buffer);
    NETAPIBUFFERFREE NetApiBufferFree;
    typedef NET_API_STATUS (WINAPI *NETSHAREENUM)(LPWSTR servername, DWORD level, LPBYTE* bufptr, DWORD prefmaxlen, LPDWORD entriesread, LPDWORD totalentries, LPDWORD resume_handle);
    NETSHAREENUM NetShareEnum;

    if ((hDLL = LoadLibrary( "NETAPI32.dll" ))!=NULL)
    {
      NetApiBufferFree = (NETAPIBUFFERFREE) GetProcAddress(hDLL,"NetApiBufferFree");
      NetShareEnum = (NETSHAREENUM) GetProcAddress(hDLL,"NetShareEnum");
      if (NetApiBufferFree && NetShareEnum)
      {
        WriteFile(MyhFile,"Share;Type;Description;Used connexions;\r\n",strlen("Share;Type;Description;Used connexions;\r\n"),&copiee,0);
        do
        {
          //lecture du nombre de partage et leurs infos
          res = NetShareEnum (0, 502, (LPBYTE *) &Buffer,MAX_PREFERRED_LENGTH, &nb, &tr,0);

          if(res == ERROR_SUCCESS || res == ERROR_MORE_DATA)
          {
            tr=1; //ok

            p=Buffer;

            for(i=1;i<=nb;i++)
            {
              //nom du partage + emplacement
              if (strlen(p->shi502_path)>0)
                snprintf(tmp,MAX_PATH,"%S (%S);",p->shi502_netname,p->shi502_path);
              else
                snprintf(tmp,MAX_PATH,"%S;",p->shi502_netname);

              WriteFile(MyhFile,tmp,strlen(tmp),&copiee,0);

              //type de partage
              switch(p->shi502_type)
              {
                case STYPE_DISKTREE: strcpy(tmp,"DISKTREE;");break;
                case STYPE_PRINTQ: strcpy(tmp,"PRINT;");break;
                case STYPE_DEVICE: strcpy(tmp,"DEVICE;");break;
                case STYPE_IPC: strcpy(tmp,"IPC;");break;
                case STYPE_SPECIAL: strcpy(tmp,"SPECIAL ");break;
                case 0x40000000/*STYPE_TEMPORARY*/: strcpy(tmp,"TEMPORARY;");break;
                case -2147483645: strcpy(tmp,"RPC;");break;
                default :snprintf(tmp,MAX_PATH,"Unknow (%l);",p->shi502_type);
              }
              WriteFile(MyhFile,tmp,strlen(tmp),&copiee,0);

              //description
              if (strlen(p->shi502_remark)>0)
              {
                snprintf(tmp,MAX_PATH,"%S;",p->shi502_remark);
                WriteFile(MyhFile,tmp,strlen(tmp),&copiee,0);
              }else WriteFile(MyhFile,";",strlen(";"),&copiee,0);

              //connections actives
              if (p->shi502_max_uses==-1)
                snprintf(tmp,MAX_PATH,"%d / Illimity;\r\n",p->shi502_current_uses);
              else
                snprintf(tmp,MAX_PATH,"%d / %d;\r\n",p->shi502_current_uses,p->shi502_max_uses);

              WriteFile(MyhFile,tmp,strlen(tmp),&copiee,0);
              p++;
            }
          }else
            tr=0;//erreur

        //boucle tant que tout le comptenu n'est pas affiché
        }while(res==ERROR_MORE_DATA);

        if (tr)NetApiBufferFree(Buffer);
      }
      FreeLibrary( hDLL );
    }
  }
  CloseHandle(MyhFile);
}
//------------------------------------------------------------------------------
void GetEnv(char *path)//txt
{
  HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
  if (MyhFile != INVALID_HANDLE_VALUE)
  {
    DWORD copiee;
    char tmp[MAX_PATH];
    char *ListeTraitement;

    if ((ListeTraitement = GetEnvironmentStrings())!=NULL)
    {
     while (*ListeTraitement)
     {
       snprintf(tmp,MAX_PATH,"%s\r\n",ListeTraitement);

       if ((tmp[0]!='=')&&(tmp[1]!=':'))//permet d'éviter le bug de gestion de début de variables
         WriteFile(MyhFile,tmp,strlen(tmp),&copiee,0);

       ListeTraitement+=lstrlen(ListeTraitement)+1;
     }
     FreeEnvironmentStrings((LPTCH)ListeTraitement);
    }
  }
  CloseHandle(MyhFile);
}
//------------------------------------------------------------------------------
//http://msdn.microsoft.com/en-us/library/windows/desktop/ms649016%28v=vs.85%29.aspx#_win32_Example_of_a_Clipboard_Viewer
void GetClipBord(char *path)//txt
{
  HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
  if (MyhFile != INVALID_HANDLE_VALUE)
  {
    //lecture du contenu du presse papier et extraction
    if (OpenClipboard(0))
    {
      UINT uFormat = EnumClipboardFormats(0);
      char format[MAX_PATH], tmp[MAX_PATH], tmp2[MAX_PATH];
      DWORD i,j;
      DWORD copiee;

      while (uFormat)
      {
         format[0]=0;
         GetClipboardFormatName(uFormat, format, MAX_PATH);

         //traitement des données
         HGLOBAL hMem = GetClipboardData(uFormat);
         char *c = GlobalLock(hMem);

         if(strlen(format)==0 && uFormat>0 && uFormat<17)
         {
           switch(uFormat)
           {
             case 1:strcpy(format,"CF_TEXT");break;
             case 2:strcpy(format,"CF_BITMAP");break;
             case 3:strcpy(format,"CF_METAFILEPICT");break;
             case 4:strcpy(format,"CF_SYLK");break;
             case 5:strcpy(format,"CF_DIF");break;
             case 6:strcpy(format,"CF_TIFF");break;
             case 7:strcpy(format,"CF_OEMTEXT");break;
             case 8:strcpy(format,"CF_DIB");break;
             case 9:strcpy(format,"CF_PALETTE");break;
             case 10:strcpy(format,"CF_PENDATA");break;
             case 11:strcpy(format,"CF_RIFF");break;
             case 12:strcpy(format,"CF_WAVE");break;
             case 13:strcpy(format,"CF_UNICODETEXT");break;
             case 14:strcpy(format,"CF_ENHMETAFILE");break;
             case 15:strcpy(format,"CF_HDROP");break;
             case 16:strcpy(format,"CF_LOCALE");break;
           }
         }

         snprintf(tmp,MAX_PATH,"\r\n######################\r\n[FORMAT:%s (%d)]\r\n[TEXT:]",format,uFormat);
         WriteFile(MyhFile,tmp,strlen(tmp),&copiee,0);
         WriteFile(MyhFile,c,strlen(c),&copiee,0);
         WriteFile(MyhFile,"\r\n[UNICODE:]",12,&copiee,0);
         snprintf(tmp,MAX_PATH,"%S\r\n[HEXA:]\r\n",c);
         WriteFile(MyhFile,tmp,strlen(tmp),&copiee,0);

         //traitement des données en HEXA
         DWORD dwSize = GlobalSize(hMem);
         for (i=0;i<dwSize;)
         {
           tmp[0]=0;tmp2[0]=0;
           char *p = tmp;
           for (j=0;j<8 && i<dwSize;j++,i++, p+=2)
           {
             snprintf(p,3,"%02X",c[i]&0xFF);
             if (c[i]>31 && c[i]<127)tmp2[j]=c[i];else tmp2[j]='.';
           }
           tmp2[j]=0;

           j = strlen(tmp);
           if (j<16)
           {
             for(;j<16;j++)tmp[j]=' ';
             tmp[16]=0;
           }
           snprintf(format,MAX_PATH,"%s  %s\r\n",tmp,tmp2);
           WriteFile(MyhFile,format,strlen(format),&copiee,0);
         }
         GlobalUnlock(hMem);
         uFormat = EnumClipboardFormats(uFormat);//next
      }
      CloseClipboard(); //fermeture du presse papier
    }
    CloseHandle(MyhFile);
  }
}
//------------------------------------------------------------------------------
void GetPipe(char *path)
{
  WIN32_FIND_DATA data;
  HANDLE hfic = FindFirstFile("\\\\.\\pipe\\*", &data);
  char tmp[MAX_PATH], tmp2[MAX_PATH],owner[MAX_PATH];
  DWORD copiee;

  if (hfic != INVALID_HANDLE_VALUE)
  {
    HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
    if (MyhFile != INVALID_HANDLE_VALUE)
    {
      do
      {
        snprintf(tmp,MAX_PATH,"\\\\.\\pipe\\%s ",data.cFileName);
        WriteFile(MyhFile,tmp,strlen(tmp),&copiee,0);

        //acls
        GetACLS(tmp, tmp2, MAX_PATH, owner, MAX_PATH);
        WriteFile(MyhFile,tmp2,strlen(tmp2),&copiee,0);
        WriteFile(MyhFile," ",strlen(" "),&copiee,0);
        WriteFile(MyhFile,owner,strlen(owner),&copiee,0);
        WriteFile(MyhFile," ",strlen(" "),&copiee,0);

        //size
        if ((data.nFileSizeLow+data.nFileSizeHigh) > 1099511627776)snprintf(tmp,MAX_PATH,"(%uTo)\r\n",(unsigned int)((data.nFileSizeLow+data.nFileSizeHigh)/1099511627776));
        else if (data.nFileSizeLow+data.nFileSizeHigh > 1073741824)snprintf(tmp,MAX_PATH,"(%uGo)\r\n",(unsigned int)((data.nFileSizeLow+data.nFileSizeHigh)/1073741824));
        else if (data.nFileSizeLow+data.nFileSizeHigh > 1048576)snprintf(tmp,MAX_PATH,"(%uMo)\r\n",(unsigned int)((data.nFileSizeLow+data.nFileSizeHigh)/1048576));
        else if (data.nFileSizeLow+data.nFileSizeHigh  > 1024)snprintf(tmp,MAX_PATH,"(%uKo)\r\n",(unsigned int)((data.nFileSizeLow+data.nFileSizeHigh)/1024));
        else snprintf(tmp,MAX_PATH,"(%uo)\r\n",(unsigned int)(data.nFileSizeLow+data.nFileSizeHigh));

        WriteFile(MyhFile,tmp,strlen(tmp),&copiee,0);

      }while(FindNextFile (hfic,&data));
    }
    CloseHandle(MyhFile);
  }
  FindClose(hfic);
}
//------------------------------------------------------------------------------
void SaveGet(DWORD id)
{
  char path[MAX_PATH];
  OPENFILENAME ofn;

  switch(id)
  {
    case POPUP_EXT_CLIPBOARD:     strcpy(path,"CLIPBOARD.txt");break;
    case POPUP_EXT_DISK:          strcpy(path,"DISK.csv");break;
    case POPUP_EXT_ROUTING_TABLE: strcpy(path,"ROUTING_TABLE.csv");break;
    case POPUP_EXT_SHARE:         strcpy(path,"SHARE.csv");break;
    case POPUP_EXT_LOCAL_VAR:     strcpy(path,"ENV.txt");break;
    case POPUP_EXT_PIPE:          strcpy(path,"PIPE.txt");break;
  }

  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = Tabl[TABL_MAIN];
  ofn.lpstrFile = path;
  ofn.nMaxFile = MAX_PATH;
  ofn.nFilterIndex = 1;
  ofn.Flags =OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

  if (id == POPUP_EXT_LOCAL_VAR || id == POPUP_EXT_PIPE || id == POPUP_EXT_CLIPBOARD){ofn.lpstrFilter ="File TXT\0*.txt\0";ofn.lpstrDefExt =".txt\0";}
  else{ofn.lpstrFilter ="File CSV\0*.csv\0";ofn.lpstrDefExt =".csv\0";}

  if (GetSaveFileName(&ofn)==TRUE)
  {
    switch(id)
    {
      case POPUP_EXT_CLIPBOARD:     GetClipBord(path);break;
      case POPUP_EXT_DISK:          GetLocalDisk(path);break;
      case POPUP_EXT_ROUTING_TABLE: GetRoutingTable(path);break;
      case POPUP_EXT_SHARE:         GetShare(path);break;
      case POPUP_EXT_LOCAL_VAR:     GetEnv(path);break;
      case POPUP_EXT_PIPE:          GetPipe(path);break;
    }
  }
}
