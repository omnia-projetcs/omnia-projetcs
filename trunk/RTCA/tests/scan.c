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
  TEST_REG_PASSWORD_ENABLE = Ischeck_treeview(htrv_test, H_tests[TEST_REG_PASSWORD]);

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
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_registry_password,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_registry_path,(void*)nb_current_test,0,0); nb_current_test++;
  if (start_scan && Ischeck_treeview(htrv_test, H_tests[nb_current_test]))h_thread_test[nb_current_test] = CreateThread(NULL,0,Scan_guide,(void*)nb_current_test,0,0); nb_current_test++;
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
  start_scan = TRUE;
  nb_current_test = 0;
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
  h_thread_test[j++] = CreateThread(NULL,0,Scan_registry_password,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_registry_path,(void*)nb_current_test,0,0);nb_current_test++;
  h_thread_test[j++] = CreateThread(NULL,0,Scan_guide,(void*)nb_current_test,0,0);nb_current_test++;
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
    case 0:Scan_files(0);break;
    case 1:Scan_log(0);break;
    case 2:Scan_disk(0);break;
    case 3:Scan_clipboard(0);break;
    case 4:Scan_env(0);break;
    case 5:Scan_task(0);break;
    case 6:Scan_process(0);break;
    case 7:Scan_pipe(0);break;
    case 8:Scan_network(0);break;
    case 9:Scan_route(0);break;
    case 10:Scan_dns(0);break;
    case 11:Scan_arp(0);break;
    case 12:Scan_share(0);break;
    case 13:Scan_registry_setting(0);break;
    case 14:Scan_registry_service(0);break;
    case 15:Scan_registry_usb(0);break;
    case 16:Scan_registry_software(0);break;
    case 17:Scan_registry_update(0);break;
    case 18:Scan_registry_start(0);break;
    case 19:Scan_registry_user(0);break;
    case 20:Scan_registry_userassist(0);break;
    case 21:Scan_registry_mru(0);break;
    case 22:TEST_REG_PASSWORD_ENABLE = TRUE;Scan_registry_password(0);break;
    case 23:Scan_registry_path(0);break;
    case 24:Scan_guide(0);break;
    case 25:Scan_antivirus(0);break;
    case 26:Scan_firewall(0);break;
    case 27:Scan_firefox_history(0);break;
    case 28:Scan_chrome_history(0);break;
    case 29:Scan_ie_history(0);break;
    case 30:Scan_android_history(0);break;
    case 31:Scan_prefetch(0);break;
  }

  sqlite3_exec(db_scan,"END TRANSACTION;", NULL, NULL, NULL);//optimizations

  //the end
  start_scan          = FALSE;
  return 0;
}
