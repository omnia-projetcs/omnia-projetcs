//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "resources.h"
//----------------------------------------------------------------
DWORD WINAPI ScanIp(LPVOID lParam)
{
  DWORD index = (DWORD)lParam;
  char ip[MAX_PATH]="", ip_mac[MAX_PATH]="", dns[MAX_PATH]="", ttl_os[MAX_PATH]="", cfg[MAX_PATH]="";
  DWORD iitem = 0;
  BOOL exist  = FALSE, dnsok = FALSE, netBIOS = FALSE;

  SendDlgItemMessage(h_main, CB_IP, LB_GETTEXT, (WPARAM)index,(LPARAM)ip);

  if (ip[0]!=0)
  {
    #ifdef DEBUG_MODE
    AddMsg(h_main,"DEBUG","SCAN:BEGIN",ip);
    #endif
    //check if exist + NetBIOS
    if (config.disco_arp||config.disco_icmp||config.disco_dns||config.disco_netbios)
    {
      WaitForSingleObject(hs_disco,0);

      //ICMP
      int ttl = -1;
      if (config.disco_icmp && scan_start)
      {
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","ICMP:BEGIN",ip);
        #endif
        ttl = Ping(ip);
        if (ttl > -1)
        {
          exist = TRUE;

          if (ttl <= MACH_LINUX)snprintf(ttl_os,MAX_PATH,"TTL:%d (Linux?)",ttl);
          else if (ttl <= MACH_WINDOWS)snprintf(ttl_os,MAX_PATH,"TTL:%d (Windows?)",ttl);
          else if (ttl <= MACH_WINDOWS)snprintf(ttl_os,MAX_PATH,"TTL:%d (Router?)",ttl);

          iitem = AddLSTVItem(ip, NULL, ttl_os, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        }
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","ICMP:END",ip);
        #endif
      }

      //ARP
      /*if (config.disco_arp)
      {
        if(exist)ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,"ARP");
        char mac[18]="";
        if (ARP(ip,mac))
        {
          snprintf(ip_mac,MAX_PATH,"%s [%s]",ip,mac);
          if (!exist)
          {
            iitem = AddLSTVItem(ip_mac, NULL, "Firewall", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
          }else
          {
            ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_IP,ip_mac);
            exist = TRUE;
          }
        }
      }*/

      //DNS
      if (config.disco_dns && scan_start)
      {
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","DNS:BEGIN",ip);
        #endif
        if(exist)ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"DNS");
        if(ResDNS(ip, dns, MAX_PATH))
        {
          if (!exist)
          {
            iitem = AddLSTVItem(ip, dns, (char*)"Firewall", NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            dnsok = TRUE;
          }else
          {
            ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_DNS,dns);
            exist = TRUE;
            dnsok = TRUE;
          }
        }
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","DNS:END",ip);
        #endif
      }

      //NetBIOS
      if ((exist || dnsok || !config.disco_dns) && config.disco_netbios && scan_start)
      {
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","NetBIOS:BEGIN",ip);
        #endif
        ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,(LPSTR)"NetBIOS");
        WaitForSingleObject(hs_netbios,0);
        char domain[MAX_PATH] = "";
        char os[MAX_PATH]     = "";

        if (scan_start)
        {
          if (dns[0] == 0) Netbios_OS(ip, os, dns, domain, MAX_PATH);
          else Netbios_OS(ip, os, NULL, domain, MAX_PATH);
        }

        if (os[0] != 0)
        {
          if (ttl > -1)snprintf(ttl_os,MAX_PATH,"TTL:%d (%s)",ttl,os);
          else snprintf(ttl_os,MAX_PATH,"Firewall (%s)",os);

          ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_TTL,ttl_os);
          netBIOS = TRUE;
        }

        char tmp[MAX_PATH] = "";
        if (domain[0] != 0)
        {
          snprintf(tmp,MAX_PATH,"Domain: %s\r\n\0",domain);
          strncat(cfg,tmp,MAX_PATH);
          netBIOS = TRUE;
        }

        //NULL session
        if (scan_start)
        {
          if(Netbios_NULLSession(ip))
          {
            strncat(cfg,"NULL Session: Enable\r\n\0",MAX_PATH);
            //ReversSID (only administrator + guest + defaults account test)
            if(TestReversSID(ip,"invité"))            strncat(cfg,"Revers SID: Enable (OK with \"invité\" account)\r\n\0",MAX_PATH);
            else if(TestReversSID(ip,"guest"))        strncat(cfg,"Revers SID: Enable (OK with \"guest\" account)\r\n\0",MAX_PATH);
            else if(TestReversSID(ip,"HelpAssistant"))strncat(cfg,"Revers SID: Enable (OK with \"HelpAssistant\" account)\r\n\0",MAX_PATH);
            else if(TestReversSID(ip,"ASPNET"))       strncat(cfg,"Revers SID: Enable (OK with \"ASPNET\" account)\r\n\0",MAX_PATH);
            else if(TestReversSID(ip,"administrateur"))strncat(cfg,"Revers SID: Enable (OK with \"administrateur\" account)\r\n\0",MAX_PATH);
            else if(TestReversSID(ip,"administrator"))strncat(cfg,"Revers SID: Enable (OK with \"administrator\" account)\r\n\0",MAX_PATH);
            netBIOS = TRUE;
          }
        }

        if (scan_start)
        {
          wchar_t server[MAX_PATH];
          char c_time[MAX_PATH]="";
          snprintf(tmp,MAX_PATH,"\\\\%s",ip);
          mbstowcs(server, tmp, MAX_PATH);
          Netbios_Time(server, c_time, MAX_PATH);
          if (c_time[0] != 0)
          {
            snprintf(tmp,MAX_PATH,"Time: %s\r\n\0",c_time);
            strncat(cfg,tmp,MAX_PATH);
            netBIOS = TRUE;
          }

          //Share
          if (scan_start)
          {
            char shares[MAX_PATH]="";
            Netbios_Share(server, shares, MAX_PATH);
            if (shares[0] != 0)
            {
              snprintf(tmp,MAX_PATH,"Share:\r\n%s\0",shares);
              strncat(cfg,tmp,MAX_PATH);
              netBIOS = TRUE;
            }
          }
        }

        if (cfg[0] != 0)
        {
          ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_CONFIG,cfg);
        }

        ReleaseSemaphore(hs_netbios,1,NULL);
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","NetBIOS:END",ip);
        #endif
      }
      ReleaseSemaphore(hs_disco,1,NULL);
    }

    if (exist&& scan_start) GetWMITests(iitem, ip, config);

/*
    if((exist || netBIOS) && scan_start)
    {
      //registry
      BOOL remote_con = FALSE;
      if (config.check_registry || config.check_services || config.check_software || config.check_USB)
      {
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","registry:BEGIN",ip);
        #endif
        ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,"Registry");
        WaitForSingleObject(hs_registry,0);
        remote_con = RemoteConnexionScan(iitem, dns, ip, config);
        ReleaseSemaphore(hs_registry,1,NULL);
        #ifdef DEBUG_MODE
        AddMsg(h_main,"DEBUG","registry:END",ip);
        #endif
      }

      //files
      if (config.check_files && scan_start)
      {

        //if ((remote_con && (config.check_registry || config.check_services || config.check_software || config.check_USB)) ||
        //    (!(config.check_registry || config.check_services || config.check_software || config.check_USB) && !remote_con))
        //{
          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","files:BEGIN",ip);
          #endif

          ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,"Files");
          WaitForSingleObject(hs_file,0);
          if (!RemoteConnexionFilesScan(iitem, dns, ip, config))ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_FILES,"CONNEXION FAIL!");
          ReleaseSemaphore(hs_file,1,NULL);

          #ifdef DEBUG_MODE
          AddMsg(h_main,"DEBUG","files:END",ip);
          #endif
        //}else ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_FILES,"CONNEXION DENY!");
      }

      ListView_SetItemText(GetDlgItem(h_main,LV_results),iitem,COL_STATE,"OK");
    }*/
    #ifdef DEBUG_MODE
    AddMsg(h_main,"DEBUG","SCAN:END",ip);
    #endif
  }
  ReleaseSemaphore(hs_threads,1,NULL);
  return 0;
}
//----------------------------------------------------------------
DWORD WINAPI scan(LPVOID lParam)
{
  //load IP
  if (IsDlgButtonChecked(h_main,CHK_LOAD_IP_FILE)!=BST_CHECKED)
  {
    //load IP interval
    SendDlgItemMessage(h_main,CB_IP,LB_RESETCONTENT,(WPARAM)NULL,(LPARAM)NULL);

    DWORD LIp1,LIp2;
    BYTE L11,L12,L13,L14,L21,L22,L23,L24;
    SendDlgItemMessage(h_main,IP1, IPM_GETADDRESS,(WPARAM) 0, (LPARAM)&LIp1);
    SendDlgItemMessage(h_main,IP2, IPM_GETADDRESS,(WPARAM) 0, (LPARAM)&LIp2);

    L11 = LIp1 >> 24;
    L12 = (LIp1 >> 16) & 0xFF;
    L13 = (LIp1 >> 8) & 0xFF;
    L14 = LIp1 & 0xFF;

    L21 = LIp2 >> 24;
    L22 = (LIp2 >> 16) & 0xFF;
    L23 = (LIp2 >> 8) & 0xFF;
    L24 = LIp2 & 0xFF;

    char sip1[IP_SIZE]="",sip2[IP_SIZE]="";
    snprintf(sip1,IP_SIZE,"%d.%d.%d.%d",L11,L12,L13,L14);
    snprintf(sip2,IP_SIZE,"%d.%d.%d.%d",L21,L22,L23,L24);

    addIPInterval(sip1, sip2);
  }

  char tmp[MAX_PATH];
  snprintf(tmp,LINE_SIZE,"Loaded %lu IP",SendDlgItemMessage(h_main,CB_IP,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL));
  AddMsg(h_main,(char*)"INFORMATION",tmp,(char*)"");

  //load config
  unsigned int ref = 0;
  config.disco_arp          = 0;//SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)0,(LPARAM)NULL);
  config.disco_icmp         = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.disco_dns          = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.disco_netbios      = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  ref++;
  /*config.config_service     = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.config_user        = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.config_software    = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.config_USB         = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.config_start       = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.config_revers_SID  = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.config_RPC         = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  ref++;*/
  config.check_files        = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.check_registry     = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.check_services     = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.check_software     = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);
  config.check_USB          = SendDlgItemMessage(h_main,CB_tests,LB_GETSEL,(WPARAM)ref++,(LPARAM)NULL);

  //load files
  if (config.check_files)     load_file_list(CB_T_FILES,     (char*)DEFAULT_LIST_FILES);
  if (config.check_registry)  load_file_list(CB_T_REGISTRY,  (char*)DEFAULT_LIST_REGISTRY);
  if (config.check_services)  load_file_list(CB_T_SERVICES,  (char*)DEFAULT_LIST_SERVICES);
  if (config.check_software)  load_file_list(CB_T_SOFTWARE,  (char*)DEFAULT_LIST_SOFTWARE);
  if (config.check_USB)       load_file_list(CB_T_USB,       (char*)DEFAULT_LIST_USB);

  if (IsDlgButtonChecked(h_main,CHK_NULL_SESSION)!=BST_CHECKED)
  {
    config.local_account   = FALSE;
    GetWindowText(GetDlgItem(h_main,ED_NET_LOGIN),config.login,MAX_PATH);
    GetWindowText(GetDlgItem(h_main,ED_NET_PASSWORD),config.mdp,MAX_PATH);
    GetWindowText(GetDlgItem(h_main,ED_NET_DOMAIN),config.domain,MAX_PATH);

    /*AddMsg(h_main,"AUTHENT","get config.login",config.login);
    AddMsg(h_main,"AUTHENT","get config.mdp",config.mdp);
    AddMsg(h_main,"AUTHENT","get config.domain",config.domain);*/
  }else
  {
    config.local_account = TRUE;
    config.login[0]      = 0;
    config.domain[0]     = 0;
    config.mdp[0]        = 0;
  }

  //---------------------------------------------
  //scan_start
  DWORD i, nb_i = SendDlgItemMessage(h_main,CB_IP,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);
  char ip_test[MAX_PATH], test_title[MAX_PATH];

  //threads
  hs_threads  = CreateSemaphore(NULL,NB_MAX_THREAD,NB_MAX_THREAD,NULL);
  hs_disco    = CreateSemaphore(NULL,NB_MAX_DISCO_THREADS,NB_MAX_DISCO_THREADS,NULL);
  hs_netbios  = CreateSemaphore(NULL,NB_MAX_NETBIOS_THREADS,NB_MAX_NETBIOS_THREADS,NULL);
  hs_file     = CreateSemaphore(NULL,NB_MAX_FILE_THREADS,NB_MAX_FILE_THREADS,NULL);
  hs_registry = CreateSemaphore(NULL,NB_MAX_REGISTRY_THREADS,NB_MAX_REGISTRY_THREADS,NULL);

  //wsa init
  WSADATA WSAData;
  WSAStartup(0x02, &WSAData );

  for (i=0;(i<nb_i) && scan_start;i++)
  {
    WaitForSingleObject(hs_threads,0);
    //ScanIp(i);
    CreateThread(NULL,0,ScanIp,(PVOID)i,0,0);

    //tracking
    snprintf(test_title,MAX_PATH,"%s %lu/%lu",TITLE,i+1,nb_i);
    SetWindowText(h_main,test_title);
  }

  //wait
  AddMsg(h_main,(char*)"INFORMATION",(char*)"Start waiting threads.",(char*)"");
  for(i=0;i<NB_MAX_THREAD;i++)WaitForSingleObject(hs_threads,INFINITE);
  /*for(i=0;i<NB_MAX_DISCO_THREADS;i++)WaitForSingleObject(hs_disco,INFINITE);
  WaitForSingleObject(hs_netbios,INFINITE);
  WaitForSingleObject(hs_file,INFINITE);
  WaitForSingleObject(hs_registry,INFINITE);*/

  WSACleanup();
  AddMsg(h_main,(char*)"INFORMATION",(char*)"End of scan!",(char*)"");

  //close
  CloseHandle(hs_threads);
  CloseHandle(hs_disco);
  CloseHandle(hs_netbios);
  CloseHandle(hs_file);
  CloseHandle(hs_registry);

  //---------------------------------------------
  //init
  if (IsDlgButtonChecked(h_main,CHK_NULL_SESSION)!=BST_CHECKED)
  {
    EnableWindow(GetDlgItem(h_main,ED_NET_LOGIN),TRUE);
    EnableWindow(GetDlgItem(h_main,ED_NET_DOMAIN),TRUE);
    EnableWindow(GetDlgItem(h_main,ED_NET_PASSWORD),TRUE);
  }

  if (IsDlgButtonChecked(h_main,CHK_LOAD_IP_FILE)!=BST_CHECKED)
  {
    EnableWindow(GetDlgItem(h_main,GRP_PERIMETER),TRUE);
    EnableWindow(GetDlgItem(h_main,IP1),TRUE);
    EnableWindow(GetDlgItem(h_main,BT_IP_CP),TRUE);
    EnableWindow(GetDlgItem(h_main,IP2),TRUE);
  }

  EnableWindow(GetDlgItem(h_main,CHK_ALL_TEST),TRUE);
  EnableWindow(GetDlgItem(h_main,CHK_NULL_SESSION),TRUE);
  EnableWindow(GetDlgItem(h_main,CHK_LOAD_IP_FILE),TRUE);
  EnableWindow(GetDlgItem(h_main,BT_START),TRUE);
  EnableWindow(GetDlgItem(h_main,CB_tests),TRUE);
  scan_start = FALSE;

  SetWindowText(GetDlgItem(h_main,BT_START),"Start");
  SetWindowText(h_main,TITLE);
}
