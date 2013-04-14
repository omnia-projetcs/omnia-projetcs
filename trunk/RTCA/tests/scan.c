//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
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

  if (Ischeck_treeview(htrv_test, H_tests[INDEX_FILE_NK]))enable_LNK= TRUE;
  else enable_LNK= FALSE;

  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_files,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_log,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_disk,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_clipboard,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_env,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_task,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_process,(void*)nb_current_test,0,0); nb_current_test++;
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
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_prefetch,(void*)nb_current_test,0,0); nb_current_test++;

  //wait !
  for (j=0;j<nb_current_test;j++)WaitForSingleObject(h_thread_test[j],INFINITE);

  sqlite3_exec(db_scan,"END TRANSACTION;", NULL, NULL, NULL);//optimizations

  //the end
  start_scan          = FALSE;
  stop_scan           = FALSE;

  //at end of scan : view main form + quit
  ShowWindow (h_main, SW_SHOW);
  EndDialog(h_conf, 0);

  return 0;
}
//------------------------------------------------------------------------------
DWORD WINAPI CMDScan(LPVOID lParam)
{
  BOOL safe_mode = (BOOL)lParam;
  unsigned int j = 0;

  //generate new session !!!
  start_scan          = TRUE;
  nb_current_test     = 0;
  enable_LNK          = TRUE;
  AddNewSession(LOCAL_SCAN,db_scan);

  //test all test on by on
  for (j=0;j<NB_TESTS;j++)h_thread_test[j] = 0;
  j=0;

  sqlite3_exec(db_scan,"BEGIN TRANSACTION;", NULL, NULL, NULL);//optimizations
  TEST_REG_PASSWORD_ENABLE = TRUE;

  if (safe_mode == FALSE)
  {
    h_thread_test[j++] = CreateThread(NULL,0,Scan_files,(void*)nb_current_test,0,0);  nb_current_test++;
    h_thread_test[j++] = CreateThread(NULL,0,Scan_log,(void*)nb_current_test,0,0);    nb_current_test++;
  }

  h_thread_test[j++] = CreateThread(NULL,0,Scan_disk,(void*)nb_current_test,0,0);     nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_clipboard,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_env,(void*)nb_current_test,0,0);      nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_task,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_process,(void*)nb_current_test,0,0);  nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_pipe,(void*)nb_current_test,0,0);     nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_network,(void*)nb_current_test,0,0);  nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_route,(void*)nb_current_test,0,0);    nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_dns,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_arp,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_share,(void*)nb_current_test,0,0);    nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_registry_setting,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_registry_service,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_registry_usb,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_registry_software,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_registry_update,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_registry_start,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_registry_user,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_registry_userassist,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_registry_mru,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_registry_ShellBags,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_registry_password,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_registry_path,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_guide,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_registry_deletedKey,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_antivirus,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_firewall,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_firefox_history,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_chrome_history,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_ie_history,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_android_history,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_prefetch,(void*)nb_current_test,0,0);nb_current_test++;


  //wait !
  for (j=0;j<nb_current_test;j++)WaitForSingleObject(h_thread_test[j],INFINITE);

  sqlite3_exec(db_scan,"END TRANSACTION;", NULL, NULL, NULL);//optimizations

  //the end
  start_scan          = FALSE;
  return 0;
}
//------------------------------------------------------------------------------
DWORD WINAPI CMDScanNum(LPVOID lParam)
{
  unsigned int test = (unsigned int)lParam;
  unsigned int j;
  for (j=0;j<NB_TESTS;j++)h_thread_test[j] = 0;

  sqlite3_exec(db_scan,"BEGIN TRANSACTION;", NULL, NULL, NULL);//optimizations

  nb_current_test = 0;
  start_scan = TRUE;

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
    case INDEX_REG_PASSWORD   :TEST_REG_PASSWORD_ENABLE = TRUE;Scan_registry_password(0);break;
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

  }

  sqlite3_exec(db_scan,"END TRANSACTION;", NULL, NULL, NULL);//optimizations

  //the end
  start_scan          = FALSE;
  return 0;
}
