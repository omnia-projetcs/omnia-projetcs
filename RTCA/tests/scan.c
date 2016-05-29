//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : https://github.com/omnia-projetcs/omnia-projetcs
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"

//------------------------------------------------------------------------------
DWORD WINAPI StopGUIScan(LPVOID lParam)
{
  start_scan          = FALSE;
  stop_scan           = TRUE;
  EnableWindow(GetDlgItem(h_conf,BT_START),FALSE);
  return 0;
}
//------------------------------------------------------------------------------
DWORD WINAPI GUIScan(LPVOID lParam)
{
  //init session
  current_session_id  = session[nb_session-1];
  nb_current_test     = 0;
  unsigned int j;
  for (j=0;j<NB_TESTS;j++)h_thread_test[j] = 0;

  sqlite3_exec(db_scan,"BEGIN TRANSACTION;", NULL, NULL, NULL);//optimizations
  TEST_REG_PASSWORD_ENABLE = Ischeck_treeview(htrv_test, H_tests[INDEX_REG_PASSWORD]);

  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_files,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = 0; nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_log,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_disk,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_clipboard,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_env,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_task,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_process,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_prefetch,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_pipe,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_network,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_route,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_dns,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_arp,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_share,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_registry_setting,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_registry_service,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_registry_usb,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_registry_software,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_registry_update,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_registry_start,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_registry_user,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_registry_userassist,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_registry_mru,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_registry_ShellBags,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_registry_password,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_registry_path,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_guide,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_registry_deletedKey,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_antivirus,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_firewall,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_firefox_history,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_chrome_history,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_ie_history,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_android_history,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_ldap,(void*)nb_current_test,0,0); nb_current_test++;

  //wait !
  for (j=0;j<nb_current_test;j++)WaitForSingleObject(h_thread_test[j],INFINITE);

  sqlite3_exec(db_scan,"END TRANSACTION;", NULL, NULL, NULL);//optimizations

  //the end
  start_scan          = FALSE;
  stop_scan           = FALSE;
  EndSession(0, db_scan);

  //at end of scan : view main form + quit
  ShowWindow (h_main, SW_SHOW);
  UpdateWindow(h_main);
  EndDialog(h_conf, 0);

  return 0;
}
//------------------------------------------------------------------------------
DWORD WINAPI CMDScan(LPVOID lParam)
{
  BOOL safe_mode            = (BOOL)lParam;
  start_scan                = TRUE;
  LOCAL_SCAN                = TRUE;
  enable_LNK                = TRUE;
  _SYSKEY[0]                = 0;

  WINE_OS                   = isWine();
  SetDebugPrivilege(TRUE);

  AddNewSession(LOCAL_SCAN, session_name_ch, db_scan);

  CMDScanNum((LPVOID)INDEX_DISK);
  CMDScanNum((LPVOID)INDEX_CLIPBOARD);
  CMDScanNum((LPVOID)INDEX_ENV);
  CMDScanNum((LPVOID)INDEX_TASK);
  CMDScanNum((LPVOID)INDEX_PIPE);
  CMDScanNum((LPVOID)INDEX_LAN);
  CMDScanNum((LPVOID)INDEX_ROUTE);
  CMDScanNum((LPVOID)INDEX_DNS);
  CMDScanNum((LPVOID)INDEX_ARP);
  CMDScanNum((LPVOID)INDEX_SHARE);
  CMDScanNum((LPVOID)INDEX_REG_CONF);
  CMDScanNum((LPVOID)INDEX_REG_SERVICES);
  CMDScanNum((LPVOID)INDEX_REG_USB);
  CMDScanNum((LPVOID)INDEX_REG_SOFTWARE);
  CMDScanNum((LPVOID)INDEX_REG_UPDATE);
  CMDScanNum((LPVOID)INDEX_REG_START);
  CMDScanNum((LPVOID)INDEX_REG_USERS);
  CMDScanNum((LPVOID)INDEX_REG_USERASSIST);
  CMDScanNum((LPVOID)INDEX_REG_MRU);
  CMDScanNum((LPVOID)INDEX_REG_SHELLBAGS);
  CMDScanNum((LPVOID)INDEX_REG_PASSWORD);
  CMDScanNum((LPVOID)INDEX_REG_PATH);
  CMDScanNum((LPVOID)INDEX_REG_GUIDE);
  CMDScanNum((LPVOID)INDEX_ANTIVIRUS);
  CMDScanNum((LPVOID)INDEX_REG_FIREWALL);
  CMDScanNum((LPVOID)INDEX_NAV_FIREFOX);
  CMDScanNum((LPVOID)INDEX_NAV_CHROME);
  CMDScanNum((LPVOID)INDEX_NAV_IE);
  CMDScanNum((LPVOID)INDEX_PREFETCH);
  CMDScanNum((LPVOID)INDEX_PROCESS);

  if(!safe_mode)
  {
    CMDScanNum((LPVOID)INDEX_LOG);
    CMDScanNum((LPVOID)INDEX_FILE);
  }

  start_scan = FALSE;
  EndSession(0, db_scan);
  return 0;
}
//------------------------------------------------------------------------------
DWORD WINAPI CMDScanNum(LPVOID lParam)
{
  unsigned int test = (unsigned int)lParam;
  sqlite3_exec(db_scan,"BEGIN TRANSACTION;", NULL, NULL, NULL);//optimizations
  TEST_REG_PASSWORD_ENABLE = TRUE;

  nb_current_test = 0;
  switch(test)
  {
    case INDEX_FILE_NK        :enable_LNK= TRUE;break;
    case INDEX_FILE           :Scan_files(0);break;
    case INDEX_LOG            :Scan_log(0);break;
    case INDEX_DISK           :Scan_disk(0);break;
    case INDEX_CLIPBOARD      :Scan_clipboard(0);break;
    case INDEX_ENV            :Scan_env(0);break;
    case INDEX_TASK           :Scan_task(0);break;
    case INDEX_PROCESS        :Scan_process(0);break;
    case INDEX_PIPE           :Scan_pipe(0);break;
    case INDEX_LAN            :Scan_network(0);break;
    case INDEX_ROUTE          :Scan_route(0);break;
    case INDEX_DNS            :Scan_dns(0);break;
    case INDEX_ARP            :Scan_arp(0);break;
    case INDEX_SHARE          :Scan_share(0);break;
    case INDEX_REG_CONF       :Scan_registry_setting(0);break;
    case INDEX_REG_SERVICES   :Scan_registry_service(0);break;
    case INDEX_REG_USB        :Scan_registry_usb(0);break;
    case INDEX_REG_SOFTWARE   :Scan_registry_software(0);break;
    case INDEX_REG_UPDATE     :Scan_registry_update(0);break;
    case INDEX_REG_START      :Scan_registry_start(0);break;
    case INDEX_REG_USERS      :Scan_registry_user(0);break;
    case INDEX_REG_USERASSIST :Scan_registry_userassist(0);break;
    case INDEX_REG_MRU        :Scan_registry_mru(0);break;
    case INDEX_REG_SHELLBAGS  :Scan_registry_ShellBags(0);break;
    case INDEX_REG_PASSWORD   :Scan_registry_password(0);break;
    case INDEX_REG_PATH       :Scan_registry_path(0);break;
    case INDEX_REG_GUIDE      :Scan_guide(0);break;
    case INDEX_REG_DELETED_KEY:Scan_registry_deletedKey(0);break;
    case INDEX_ANTIVIRUS      :Scan_antivirus(0);break;
    case INDEX_REG_FIREWALL   :Scan_firewall(0);break;
    case INDEX_NAV_FIREFOX    :Scan_firefox_history(0);break;
    case INDEX_NAV_CHROME     :Scan_chrome_history(0);break;
    case INDEX_NAV_IE         :Scan_ie_history(0);break;
    case INDEX_ANDROID        :Scan_android_history(0);break;
    case INDEX_PREFETCH       :Scan_prefetch(0);break;
    case INDEX_LDAP           :Scan_ldap(0);break;
  }

  sqlite3_exec(db_scan,"END TRANSACTION;", NULL, NULL, NULL);//optimizations
  return 0;
}
