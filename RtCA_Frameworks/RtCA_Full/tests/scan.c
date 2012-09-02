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

  //wait !
  for (j=0;j<nb_current_test;j++)WaitForSingleObject(h_thread_test[j],INFINITE);

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
  AddNewSession(LOCAL_SCAN,db_scan);
  for (j=0;j<NB_TESTS;j++)h_thread_test[j] = 0;
  j=0;

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

  //wait !
  for (j=0;j<nb_current_test;j++)WaitForSingleObject(h_thread_test[j],INFINITE);

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
  switch(test)
  {
    case 0:h_thread_test[0]   = CreateThread(NULL,0,Scan_files,(void*)nb_current_test,0,0);break;
    case 1:h_thread_test[0]   = CreateThread(NULL,0,Scan_log,(void*)nb_current_test,0,0);break;
    case 2:h_thread_test[0]   = CreateThread(NULL,0,Scan_disk,(void*)nb_current_test,0,0);break;
    case 3:h_thread_test[0]   = CreateThread(NULL,0,Scan_clipboard,(void*)nb_current_test,0,0);break;
    case 4:h_thread_test[0]   = CreateThread(NULL,0,Scan_env,(void*)nb_current_test,0,0);break;
    case 5:h_thread_test[0]   = CreateThread(NULL,0,Scan_task,(void*)nb_current_test,0,0);break;
    case 6:h_thread_test[0]   = CreateThread(NULL,0,Scan_process,(void*)nb_current_test,0,0);break;
    case 7:h_thread_test[0]   = CreateThread(NULL,0,Scan_pipe,(void*)nb_current_test,0,0);break;
    case 8:h_thread_test[0]   = CreateThread(NULL,0,Scan_network,(void*)nb_current_test,0,0);break;
    case 9:h_thread_test[0]   = CreateThread(NULL,0,Scan_route,(void*)nb_current_test,0,0);break;
    case 10:h_thread_test[0]  = CreateThread(NULL,0,Scan_dns,(void*)nb_current_test,0,0);break;
    case 11:h_thread_test[0]  = CreateThread(NULL,0,Scan_arp,(void*)nb_current_test,0,0);break;
    case 12:h_thread_test[0]  = CreateThread(NULL,0,Scan_share,(void*)nb_current_test,0,0);break;
    case 13:h_thread_test[0]  = CreateThread(NULL,0,Scan_registry_setting,(void*)nb_current_test,0,0);break;
    case 14:h_thread_test[0]  = CreateThread(NULL,0,Scan_registry_service,(void*)nb_current_test,0,0);break;
    case 15:h_thread_test[0]  = CreateThread(NULL,0,Scan_registry_usb,(void*)nb_current_test,0,0);break;
    case 16:h_thread_test[0]  = CreateThread(NULL,0,Scan_registry_software,(void*)nb_current_test,0,0);break;
    case 17:h_thread_test[0]  = CreateThread(NULL,0,Scan_registry_update,(void*)nb_current_test,0,0);break;
    case 18:h_thread_test[0]  = CreateThread(NULL,0,Scan_registry_start,(void*)nb_current_test,0,0);break;
    case 19:h_thread_test[0]  = CreateThread(NULL,0,Scan_registry_user,(void*)nb_current_test,0,0);break;
    case 20:h_thread_test[0]  = CreateThread(NULL,0,Scan_registry_userassist,(void*)nb_current_test,0,0);break;
    case 21:h_thread_test[0]  = CreateThread(NULL,0,Scan_registry_mru,(void*)nb_current_test,0,0);break;
    case 22:h_thread_test[0]  = CreateThread(NULL,0,Scan_registry_password,(void*)nb_current_test,0,0);break;
    case 23:h_thread_test[0]  = CreateThread(NULL,0,Scan_registry_path,(void*)nb_current_test,0,0);break;
    case 24:h_thread_test[0]  = CreateThread(NULL,0,Scan_guide,(void*)nb_current_test,0,0);break;
    case 25:h_thread_test[0]  = CreateThread(NULL,0,Scan_antivirus,(void*)nb_current_test,0,0);break;
    case 26:h_thread_test[0]  = CreateThread(NULL,0,Scan_firewall,(void*)nb_current_test,0,0);break;
    case 27:h_thread_test[0]  = CreateThread(NULL,0,Scan_firefox_history,(void*)nb_current_test,0,0);break;
    case 28:h_thread_test[0]  = CreateThread(NULL,0,Scan_chrome_history,(void*)nb_current_test,0,0);break;
    case 29:h_thread_test[0]  = CreateThread(NULL,0,Scan_ie_history,(void*)nb_current_test,0,0);break;
    case 30:h_thread_test[0]  = CreateThread(NULL,0,Scan_android_history,(void*)nb_current_test,0,0);break;
  }
  nb_current_test++;

  //wait !
  WaitForSingleObject(h_thread_test[0],INFINITE);

  //the end
  start_scan          = FALSE;
  return 0;
}
