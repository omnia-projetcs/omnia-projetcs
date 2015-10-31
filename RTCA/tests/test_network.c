//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : https://github.com/omnia-projetcs/omnia-projetcs
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void addNetworktoDB(char *source, char *card, char *description, char *guid,
               char *hostname, char *ip, char *netmask, char *gateway,
               char *dns, char *domain,
               char *dhcp_server, char *dhcp_mode,
               char *wifi, char *last_update, DWORD session_id, sqlite3 *db)
{
  #ifndef CMD_LINE_ONLY_NO_DB
  char request[REQUEST_MAX_SIZE];
  snprintf(request,REQUEST_MAX_SIZE,
           "INSERT INTO extract_network (source,card,description,guid,ip,netmask,gateway,dns,domain,dhcp_mode,dhcp_server,wifi,last_update,session_id) "
           "VALUES(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%lu);",
           source,card,description,guid, ip,netmask,gateway,dns,domain,dhcp_mode,dhcp_server,wifi,last_update,session_id);
  sqlite3_exec(db,request, NULL, NULL, NULL);
  #else
  printf("\"Network\";\"%s\";\"%s\";\"%s\";\"%s\";\"%s\";\"%s\";\"%s\";\"%s\";\"%s\";\"%s\";\"%s\";\"%s\";\"%s\";\"%lu\";\r\n",
         source,card,description,guid, ip,netmask,gateway,dns,domain,dhcp_mode,dhcp_server,wifi,last_update,session_id);
  #endif
}
//------------------------------------------------------------------------------
//local function part !!!
//------------------------------------------------------------------------------
BOOL ReadRegistryWifiInfos(char *guid, char *wifi_cache, DWORD size_wifi_cache)
{
  BOOL ret = FALSE;
  char key_path[MAX_PATH];
  char value[MAX_PATH], data[MAX_PATH];
  wifi_cache[0] = 0;
  unsigned int tmp_size;

  snprintf(key_path,MAX_PATH,"SOFTWARE\\MICROSOFT\\WZCSVC\\Parameters\\interfaces\\%s",guid);

  if (ReadValue(HKEY_LOCAL_MACHINE,key_path,"ControlFlags",value, MAX_PATH))
  {
    HKEY CleTmp   = 0;
    DWORD i, nbSubValue = 0, value_size, data_size,type;

    if (RegOpenKey(HKEY_LOCAL_MACHINE,key_path,&CleTmp)==ERROR_SUCCESS)
    {
      if (RegQueryInfoKey (CleTmp,0,0,0,0,0,0,&nbSubValue,0,0,0,0)==ERROR_SUCCESS)
      {
        for (i=0;i<nbSubValue;i++)
        {
          value_size = MAX_PATH;
          data_size  = MAX_PATH;
          value[0]   = 0;
          data[0]    = 0;
          type       = 0;
          if (RegEnumValue (CleTmp,i,value,&value_size,0,&type,(LPBYTE)data,&data_size)==ERROR_SUCCESS)
          {
            //set datas
            if (data_size > 0x34 && (type == REG_EXPAND_SZ || type ==REG_SZ || type ==REG_BINARY))
            {
              tmp_size = strlen(wifi_cache);
              if (size_wifi_cache-tmp_size <= 0)break;

              ret = TRUE;
              switch (data[0x34])
              {
                case 0:snprintf(wifi_cache+tmp_size,size_wifi_cache-tmp_size,"(%s, eSSID:\"%s\", WEP)\r\n",value,data+0x14);break;
                case 1:snprintf(wifi_cache+tmp_size,size_wifi_cache-tmp_size,"(%s, eSSID:\"%s\", Open)\r\n",value,data+0x14);break;
                case 4:snprintf(wifi_cache+tmp_size,size_wifi_cache-tmp_size,"(%s, eSSID:\"%s\", %s)\r\n",value,data+0x14,data[4]==3?"WPA-PSK/TKIP":"WPA/TKIP");break;
                case 6:snprintf(wifi_cache+tmp_size,size_wifi_cache-tmp_size,"(%s, eSSID:\"%s\", %s)\r\n",value,data+0x14,data[4]==3?"WPA-PSK/AES":"WPA/AES");break;
                default:snprintf(wifi_cache+tmp_size,size_wifi_cache-tmp_size,"(%s, eSSID:\"%s\", serial:%d, %s)\r\n",value,data+0x14,data[0x34],data[4]==3?"WPA2-PSK":"WPA2");break;
              }
            }
          }
        }
      }
     RegCloseKey(CleTmp);
    }
  }
  return ret;
}
//------------------------------------------------------------------------------
void SearchNetworkGUID(HKEY hk,char *path,char *guid, char *card, DWORD size_card, char*description, DWORD size_description)
{
  HKEY CleTmp   = 0;
  card[0]       = 0;
  description[0]= 0;
  DWORD i, nbSubKey = 0, key_size;
  char tmp_key[MAX_PATH],key_path[MAX_PATH];

  if (RegOpenKey(hk,path,&CleTmp)==ERROR_SUCCESS)
  {
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      for (i=0;i<nbSubKey;i++)
      {
        key_size   = MAX_PATH;
        tmp_key[0] = 0;
        if (RegEnumKeyEx (CleTmp,i,tmp_key,&key_size,0,0,0,0)==ERROR_SUCCESS)
        {
          //generate the key
          snprintf(key_path,MAX_PATH,"%s%s",path,tmp_key);

          //read guid data
          tmp_key[0]=0;
          if (ReadValue(hk,key_path,"NetCfgInstanceId",tmp_key, size_description))
          {
            if (strcmp(tmp_key,guid) == 0)
            {
              //get datas
              if (ReadValue(hk,key_path,"DriverDesc",description, size_description) == 0)description[0]= 0;
              if (ReadValue(hk,key_path,"ComponentId",card, size_card) == 0)
                if (ReadValue(hk,key_path,"MatchingDeviceI",card, size_card) == 0)description[0]= 0;

              break;
            }
          }
        }
      }
    }
    RegCloseKey(CleTmp);
  }
}
//------------------------------------------------------------------------------
void Scan_network_local(sqlite3 *db, unsigned int session_id)
{
  char ip[DEFAULT_TMP_SIZE]       ="",
    netmask[DEFAULT_TMP_SIZE]     ="",
    gw[DEFAULT_TMP_SIZE]          ="",
    dns[DEFAULT_TMP_SIZE]         ="",
    domain[DEFAULT_TMP_SIZE]      ="",
    dhcp_server[DEFAULT_TMP_SIZE] ="",
    card[DEFAULT_TMP_SIZE]        ="",
    description[DEFAULT_TMP_SIZE] ="",
    hostname[DEFAULT_TMP_SIZE]    ="",
    lastupdate[DATE_SIZE_MAX]     ="",
    wifi_cache[MAX_PATH]          ="",
    guid[MAX_PATH]                ="";

  //if wine
  if (WINE_OS)
  {
    DWORD i,ulOutBufLen = sizeof (IP_ADAPTER_INFO);
    PIP_ADAPTER_INFO pAdapter, pAdapterInfo = malloc(ulOutBufLen);
    if (pAdapterInfo == NULL)return;

    DWORD ret = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
    if (ret == ERROR_BUFFER_OVERFLOW)
    {
      pAdapterInfo = realloc(pAdapterInfo,ulOutBufLen);
      if (pAdapterInfo == NULL)return;
      else ret = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
    }

    //DNS
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa)== 0)
    {
      if (gethostname(hostname, DEFAULT_TMP_SIZE) == SOCKET_ERROR)hostname[0] = 0;
      WSACleanup();
    }

    if (ret == NO_ERROR)
    {
      pAdapter = pAdapterInfo;
      while (pAdapter)
      {
        card[0]         = 0;
        description[0]  = 0;
        strncpy(card,pAdapter->AdapterName,DEFAULT_TMP_SIZE);
        strncpy(description,pAdapter->Description,DEFAULT_TMP_SIZE);

        ip[0]           = 0;
        netmask[0]      = 0;
        gw[0]           = 0;
        strncpy(ip,pAdapter->IpAddressList.IpAddress.String,DEFAULT_TMP_SIZE);
        strncpy(netmask,pAdapter->IpAddressList.IpMask.String,DEFAULT_TMP_SIZE);
        strncpy(gw,pAdapter->GatewayList.IpAddress.String,DEFAULT_TMP_SIZE);

        if (pAdapter->DhcpEnabled)strncpy(dhcp_server,pAdapter->DhcpServer.IpAddress.String,DEFAULT_TMP_SIZE);
        else dhcp_server[0] = 0;

        guid[0]         = 0;
        for (i=0;i<pAdapter->AddressLength;i++)
        {
          if (i != (pAdapter->AddressLength - 1))
            snprintf(guid+strlen(guid)+1,DEFAULT_TMP_SIZE-strlen(guid),"%.2X-",(int) pAdapter->Address[i]);
          else snprintf(guid+strlen(guid)+1,DEFAULT_TMP_SIZE-strlen(guid),"%.2X",(int) pAdapter->Address[i]);
        }

        //dns
        if (pAdapter->HaveWins) snprintf(dns,DEFAULT_TMP_SIZE,"%s %s",pAdapter->PrimaryWinsServer.IpAddress.String, pAdapter->SecondaryWinsServer.IpAddress.String);
        else dns[0]=0;

        //add only if a card
        if (description[0]!=0 || card[0]!=0 || dns[0]!=0)
        {
          convertStringToSQL(description, DEFAULT_TMP_SIZE);
          addNetworktoDB("API ADAPTER_INFO",card, description, guid, hostname, ip, netmask, gw, dns, domain, dhcp_server,pAdapter->DhcpEnabled?"X":"",wifi_cache,lastupdate, session_id, db);
        }
        pAdapter = pAdapter->Next;
      }
    }
    free(pAdapterInfo);
    return;
  }

  //local registry
  //read list of key
  HKEY CleTmp = 0;
  char key_path[MAX_PATH], tmp_key[MAX_PATH];
  DWORD i, nbSubKey = 0, key_size;
  BOOL dhcp_mode;

  FILETIME LastWriteTime;

  if (RegOpenKey(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\",&CleTmp)==ERROR_SUCCESS)
  {
    if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
    {
      for (i=0;i<nbSubKey;i++)
      {
        key_size   = MAX_PATH;
        tmp_key[0] = 0;
        if (RegEnumKeyEx (CleTmp,i,tmp_key,&key_size,0,0,0,&LastWriteTime)==ERROR_SUCCESS)
        {
          //generate the key
          snprintf(key_path,MAX_PATH,"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\%s",tmp_key);

          //hostname
          if (ReadValue(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\",
                        "Hostname",hostname,DEFAULT_TMP_SIZE) == 0)hostname[0]=0;

          //card info
          SearchNetworkGUID(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}\\",
                            tmp_key, card, DEFAULT_TMP_SIZE, description, DEFAULT_TMP_SIZE);

          //last update
          filetimeToString_GMT(LastWriteTime, lastupdate, DATE_SIZE_MAX);

          //read datas
          if (ReadDwordValue(HKEY_LOCAL_MACHINE,key_path,"EnableDHCP") == 1) //DHCP
          {
            dhcp_mode = TRUE;
            //ip
            if (ReadValue(HKEY_LOCAL_MACHINE,key_path,"DhcpIPAddress",ip,DEFAULT_TMP_SIZE) == 0)ip[0]=0;
            //netmask
            if (ReadValue(HKEY_LOCAL_MACHINE,key_path,"DhcpSubnetMask",netmask,DEFAULT_TMP_SIZE) == 0)netmask[0]=0;
            //gateway
            if (ReadValue(HKEY_LOCAL_MACHINE,key_path,"DhcpDefaultGateway",gw,DEFAULT_TMP_SIZE) == 0)gw[0]=0;
            //dns
            if (ReadValue(HKEY_LOCAL_MACHINE,key_path,"DhcpNameServer",dns,DEFAULT_TMP_SIZE) == 0)
              if (ReadValue(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\","DhcpNameServer",dns,DEFAULT_TMP_SIZE) == 0)dns[0]=0;
            //domain
            if (ReadValue(HKEY_LOCAL_MACHINE,key_path,"DhcpDomain",domain,DEFAULT_TMP_SIZE) == 0)
              if (ReadValue(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\","DhcpDomain",domain,DEFAULT_TMP_SIZE) == 0)domain[0]=0;
            //dhcp server
            if (ReadValue(HKEY_LOCAL_MACHINE,key_path,"DhcpServer",dhcp_server,DEFAULT_TMP_SIZE) == 0)dhcp_server[0]=0;
          }else
          {
            dhcp_mode = FALSE;
            //ip
            if (ReadValue(HKEY_LOCAL_MACHINE,key_path,"IPAddress",ip,DEFAULT_TMP_SIZE) == 0)ip[0]=0;
            //netmask
            if (ReadValue(HKEY_LOCAL_MACHINE,key_path,"SubnetMask",netmask,DEFAULT_TMP_SIZE) == 0)netmask[0]=0;
            //gateway
            if (ReadValue(HKEY_LOCAL_MACHINE,key_path,"DefaultGateway",gw,DEFAULT_TMP_SIZE) == 0)gw[0]=0;
            //dns
            if (ReadValue(HKEY_LOCAL_MACHINE,key_path,"NameServer",dns,DEFAULT_TMP_SIZE) == 0)
              if (ReadValue(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\","NameServer",dns,DEFAULT_TMP_SIZE) == 0)dns[0]=0;
            //domain
            if (ReadValue(HKEY_LOCAL_MACHINE,key_path,"Domain",domain,DEFAULT_TMP_SIZE) == 0)
              if (ReadValue(HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\","Domain",domain,DEFAULT_TMP_SIZE == 0))domain[0]=0;
            dhcp_server[0]=0;
          }

          //WiFi
          if (!ReadRegistryWifiInfos(tmp_key, wifi_cache, MAX_PATH))
          {
            //read to local XML
          }

          //add only if a card
          if (description[0]!=0 || card[0]!=0 || dns[0]!=0)
          {
            convertStringToSQL(description, DEFAULT_TMP_SIZE);
            addNetworktoDB("HKEY_LOCAL_MACHINE\\SYSTEM",card, description, tmp_key,hostname, ip, netmask, gw, dns, domain, dhcp_server,dhcp_mode?"X":"",wifi_cache,lastupdate, session_id, db);
          }
        }
      }
    }
    RegCloseKey(CleTmp);
  }
}
//------------------------------------------------------------------------------
//file registry part
//------------------------------------------------------------------------------
BOOL SearchNetworkGUID_registry_file(char *buffer, DWORD taille_fic, DWORD position, DWORD pos_fhbin,
                                     char *guid, char *card, DWORD size_card, char*description, DWORD size_description, char*ckey)
{
  HBIN_CELL_NK_HEADER *nk_h = GetRegistryNK(buffer, taille_fic, position, pos_fhbin,ckey);

  if (card != NULL)       card[0]         = 0;
  if (description != NULL)description[0]  = 0;
  if (nk_h != NULL)
  {
    DWORD i, nbSubKey = GetSubNK(buffer, taille_fic, nk_h, position, 0, NULL, 0);
    HBIN_CELL_NK_HEADER *nk_h_tmp;
    char tmp[MAX_PATH];
    for (i=0;i<nbSubKey;i++)
    {
      //for each subkey get nk of key :)
      nk_h_tmp = GetSubNKtonk(buffer, taille_fic, nk_h, position, i);
      if (nk_h_tmp == NULL)continue;

      Readnk_Value(buffer, taille_fic, (pos_fhbin)+HBIN_HEADER_SIZE, position, NULL, nk_h_tmp,"NetCfgInstanceId", tmp, MAX_PATH);
      if (strcmp(tmp,guid) == 0)
      {
        Readnk_Value(buffer, taille_fic, (pos_fhbin)+HBIN_HEADER_SIZE, position, NULL, nk_h_tmp,"DriverDesc", description, size_description);
        if(Readnk_Value(buffer, taille_fic, (pos_fhbin)+HBIN_HEADER_SIZE, position, NULL, nk_h_tmp,"ComponentId", card, size_card)==FALSE)
          Readnk_Value(buffer, taille_fic, (pos_fhbin)+HBIN_HEADER_SIZE, position, NULL, nk_h_tmp,"MatchingDeviceI", card, size_card);

        return TRUE;
      }
    }
  }
  return FALSE;
}
//------------------------------------------------------------------------------
BOOL ReadRegistryWifiInfos_registry_file(char *buffer, DWORD taille_fic, DWORD position, DWORD pos_fhbin,
                                         char *guid, char *wifi_cache, DWORD size_wifi_cache)
{
  BOOL ret = FALSE;
  char key_path[MAX_PATH];
  snprintf(key_path,MAX_PATH,"MICROSOFT\\WZCSVC\\Parameters\\interfaces\\%s",guid);

  if (wifi_cache != NULL)       wifi_cache[0]         = 0;

  HBIN_CELL_NK_HEADER *nk_h = GetRegistryNK(buffer, taille_fic, position, pos_fhbin,key_path);
  if (nk_h != NULL)
  {
    char value[MAX_PATH], data[MAX_PATH];
    DWORD type, tmp_size,data_size = MAX_PATH;
    if(Readnk_Value(buffer, taille_fic, (pos_fhbin)+HBIN_HEADER_SIZE, position, NULL, nk_h,"ControlFlags", data, MAX_PATH))
    {
      DWORD i, nbSubKey = GetValueData(buffer, taille_fic, nk_h, position, 0, NULL, 0, NULL, 0);
      for (i=0;i<nbSubKey;i++)
      {
        data_size = MAX_PATH;
        type = GetBinaryValueData(buffer, taille_fic, nk_h, position, i, value, MAX_PATH,data, &data_size);

        //set datas
        if (data_size > 0x34 && (type == REG_EXPAND_SZ || type == REG_SZ || type == REG_BINARY))
        {
          tmp_size = strlen(wifi_cache);
          if (size_wifi_cache-tmp_size <= 0)break;

          ret = TRUE;
          switch (data[0x34])
          {
            case 0:snprintf(wifi_cache+tmp_size,size_wifi_cache-tmp_size,"(%s, eSSID:\"%s\", WEP)\r\n",value,data+0x14);break;
            case 1:snprintf(wifi_cache+tmp_size,size_wifi_cache-tmp_size,"(%s, eSSID:\"%s\", Open)\r\n",value,data+0x14);break;
            case 4:snprintf(wifi_cache+tmp_size,size_wifi_cache-tmp_size,"(%s, eSSID:\"%s\", %s)\r\n",value,data+0x14,data[4]==3?"WPA-PSK/TKIP":"WPA/TKIP");break;
            case 6:snprintf(wifi_cache+tmp_size,size_wifi_cache-tmp_size,"(%s, eSSID:\"%s\", %s)\r\n",value,data+0x14,data[4]==3?"WPA-PSK/AES":"WPA/AES");break;
            default:snprintf(wifi_cache+tmp_size,size_wifi_cache-tmp_size,"(%s, eSSID:\"%s\", serial:%d, %s)\r\n",value,data+0x14,data[0x34],data[4]==3?"WPA2-PSK":"WPA2");break;
          }
        }
      }
    }
  }
  return ret;
}
//------------------------------------------------------------------------------
void Scan_network_registry_file(char *file, char *ckey, char *_ckey_2, sqlite3 *db, unsigned int session_id)
{
  HK_F_OPEN hks;
  if(OpenRegFiletoMem(&hks, file))
  {
    char _ckey[MAX_PATH];
    snprintf(_ckey,MAX_PATH,"%s\\Interfaces",ckey);

    //exist or not in the file ?
    HBIN_CELL_NK_HEADER *nk_h = GetRegistryNK(hks.buffer, hks.taille_fic, (hks.pos_fhbin)+HBIN_HEADER_SIZE, hks.position, _ckey);
    if (nk_h == NULL)
    {
      CloseRegFiletoMem(&hks);
      return;
    }

    char tmp_key[MAX_PATH];
    BOOL dhcp_mode;
    char ip[DEFAULT_TMP_SIZE],
        netmask[DEFAULT_TMP_SIZE],
        gw[DEFAULT_TMP_SIZE],
        dns[DEFAULT_TMP_SIZE],
        domain[DEFAULT_TMP_SIZE],
        dhcp_server[DEFAULT_TMP_SIZE],
        card[DEFAULT_TMP_SIZE],
        description[DEFAULT_TMP_SIZE],
        hostname[DEFAULT_TMP_SIZE],
        lastupdate[DATE_SIZE_MAX],
        wifi_cache[MAX_PATH];

    //working
    HBIN_CELL_NK_HEADER *nk_h_tmp;
    char tmp[MAX_PATH];
    DWORD i, nbSubKey = GetSubNK(hks.buffer, hks.taille_fic, nk_h, hks.position, 0, NULL, 0);
    for (i=0;i<nbSubKey;i++)
    {
      //for each subkey
      if(GetSubNK(hks.buffer, hks.taille_fic, nk_h, hks.position, i, tmp_key, MAX_PATH))
      {
        //get nk of key :)
        nk_h_tmp = GetSubNKtonk(hks.buffer, hks.taille_fic, nk_h, hks.position, i);
        if (nk_h_tmp == NULL)continue;

        //hostname
        Readnk_Value(hks.buffer,hks.taille_fic, (hks.pos_fhbin)+HBIN_HEADER_SIZE, hks.position, ckey , NULL, "Hostname", hostname, DEFAULT_TMP_SIZE);

        //card info
        SearchNetworkGUID_registry_file(hks.buffer,hks.taille_fic, (hks.pos_fhbin)+HBIN_HEADER_SIZE, hks.position ,tmp_key, card, DEFAULT_TMP_SIZE, description, DEFAULT_TMP_SIZE, _ckey_2);

        //last update
        Readnk_Infos(hks.buffer, hks.taille_fic, (hks.pos_fhbin), hks.position, NULL, nk_h_tmp,
                     lastupdate, DATE_SIZE_MAX, NULL, 0, NULL, 0);

        //read datas
        Readnk_Value(hks.buffer, hks.taille_fic, (hks.pos_fhbin)+HBIN_HEADER_SIZE, hks.position, NULL, nk_h_tmp,"EnableDHCP", tmp, MAX_PATH);
        if (strcmp(tmp,"0x00000001") == 0)//DHCP
        {
          dhcp_mode = TRUE;
          Readnk_Value(hks.buffer, hks.taille_fic, (hks.pos_fhbin)+HBIN_HEADER_SIZE, hks.position, NULL, nk_h_tmp,"DhcpIPAddress", ip, DEFAULT_TMP_SIZE);
          Readnk_Value(hks.buffer, hks.taille_fic, (hks.pos_fhbin)+HBIN_HEADER_SIZE, hks.position, NULL, nk_h_tmp,"DhcpSubnetMask", netmask, DEFAULT_TMP_SIZE);
          Readnk_Value(hks.buffer, hks.taille_fic, (hks.pos_fhbin)+HBIN_HEADER_SIZE, hks.position, NULL, nk_h_tmp,"DhcpDefaultGateway", gw, DEFAULT_TMP_SIZE);
          if (Readnk_Value(hks.buffer, hks.taille_fic, (hks.pos_fhbin)+HBIN_HEADER_SIZE, hks.position, NULL, nk_h_tmp,"DhcpNameServer", dns, DEFAULT_TMP_SIZE) == FALSE)
              Readnk_Value(hks.buffer, hks.taille_fic, (hks.pos_fhbin)+HBIN_HEADER_SIZE, hks.position, ckey , NULL, "DhcpNameServer", dns, DEFAULT_TMP_SIZE);

          if (Readnk_Value(hks.buffer, hks.taille_fic, (hks.pos_fhbin)+HBIN_HEADER_SIZE, hks.position, NULL, nk_h_tmp,"DhcpDomain", domain, DEFAULT_TMP_SIZE) == FALSE)
            Readnk_Value(hks.buffer,hks.taille_fic, (hks.pos_fhbin)+HBIN_HEADER_SIZE, hks.position, ckey , NULL, "DhcpDomain", domain, DEFAULT_TMP_SIZE);
          Readnk_Value(hks.buffer, hks.taille_fic, (hks.pos_fhbin)+HBIN_HEADER_SIZE, hks.position, NULL, nk_h_tmp,"DhcpServer", dhcp_server, DEFAULT_TMP_SIZE);
        }else
        {
          dhcp_mode = FALSE;
          Readnk_Value(hks.buffer, hks.taille_fic, (hks.pos_fhbin)+HBIN_HEADER_SIZE, hks.position, NULL, nk_h_tmp,"IPAddress", ip, DEFAULT_TMP_SIZE);
          Readnk_Value(hks.buffer, hks.taille_fic, (hks.pos_fhbin)+HBIN_HEADER_SIZE, hks.position, NULL, nk_h_tmp,"SubnetMask", netmask, DEFAULT_TMP_SIZE);
          Readnk_Value(hks.buffer, hks.taille_fic, (hks.pos_fhbin)+HBIN_HEADER_SIZE, hks.position, NULL, nk_h_tmp,"DefaultGateway", gw, DEFAULT_TMP_SIZE);
          if (Readnk_Value(hks.buffer, hks.taille_fic, (hks.pos_fhbin)+HBIN_HEADER_SIZE, hks.position, NULL, nk_h_tmp,"NameServer", dns, DEFAULT_TMP_SIZE) == FALSE)
            Readnk_Value(hks.buffer,hks.taille_fic, (hks.pos_fhbin)+HBIN_HEADER_SIZE, hks.position, ckey , NULL, "NameServer", dns, DEFAULT_TMP_SIZE);

          if (Readnk_Value(hks.buffer, hks.taille_fic, (hks.pos_fhbin)+HBIN_HEADER_SIZE, hks.position, NULL, nk_h_tmp,"Domain", domain, DEFAULT_TMP_SIZE) == FALSE)
            Readnk_Value(hks.buffer,hks.taille_fic, (hks.pos_fhbin)+HBIN_HEADER_SIZE, hks.position, ckey , NULL, "Domain", domain, DEFAULT_TMP_SIZE);
          dhcp_server[0]=0;
        }

        //check test
        if (strcmp(ip,"0.0.0.0")==0 || strcmp(ip,"0.0.0.0 ")==0)                  ip[0]           = 0;
        if (strcmp(netmask,"0.0.0.0")==0 || strcmp(netmask,"0.0.0.0 ")==0)        netmask[0]      = 0;
        if (strcmp(gw,"0.0.0.0")==0 || strcmp(gw,"0.0.0.0 ")==0)                  gw[0]           = 0;
        if (strcmp(dhcp_server,"0.0.0.0")==0 ||
             strcmp(dhcp_server,"255.255.255.255")==0)                            dhcp_server[0]  = 0;

        //Wifi in SOFTWARE RUCHE

        //add only if a real card
        if (description[0]!=0 || card[0]!=0 || dns[0]!=0 || ip[0]!=0)
        {
          convertStringToSQL(description, DEFAULT_TMP_SIZE);
          addNetworktoDB(file,card, description, tmp_key, hostname, ip, netmask, gw, dns, domain, dhcp_server,dhcp_mode?"X":"",wifi_cache,lastupdate, session_id, db);
        }
      }
    }
    CloseRegFiletoMem(&hks);
  }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DWORD WINAPI Scan_network(LPVOID lParam)
{
  //init
  sqlite3 *db = (sqlite3 *)db_scan;
  unsigned int session_id = current_session_id;
  char file[MAX_PATH];
  #ifdef CMD_LINE_ONLY_NO_DB
  printf("\"Network\";\"source\";\"card\";\"description\";\"guid\";\"ip\";\"netmask\";\"gateway\";\"dns\";\"domain\";\"dhcp_mode\";\"dhcp_server\";\"wifi\";\"last_update\";\"session_id\";\r\n");
  #endif
  //files or local
  if(!SQLITE_FULL_SPEED)sqlite3_exec(db_scan,"BEGIN TRANSACTION;", NULL, NULL, NULL);
  HTREEITEM hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]);
  if (hitem!=NULL && !LOCAL_SCAN) //files
  {
    while(hitem!=NULL && start_scan)
    {
      file[0] = 0;
      GetTextFromTrv(hitem, file, MAX_PATH);
      if (file[0] != 0)
      {
        //verify
        Scan_network_registry_file(file, "ControlSet001\\Services\\Tcpip\\Parameters","ControlSet001\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}", db, session_id);
        Scan_network_registry_file(file, "ControlSet002\\Services\\Tcpip\\Parameters","ControlSet002\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}", db, session_id);
        Scan_network_registry_file(file, "ControlSet003\\Services\\Tcpip\\Parameters","ControlSet003\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}", db, session_id);
        Scan_network_registry_file(file, "ControlSet004\\Services\\Tcpip\\Parameters","ControlSet004\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}", db, session_id);
      }
      hitem = (HTREEITEM)SendMessage(htrv_files, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem);
    }
  }else Scan_network_local(db, session_id); //local

  if(!SQLITE_FULL_SPEED)sqlite3_exec(db_scan,"END TRANSACTION;", NULL, NULL, NULL);
  check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  h_thread_test[(unsigned int)lParam] = 0;
  return 0;
}
