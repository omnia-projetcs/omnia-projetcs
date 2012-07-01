//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "RtCA.h"
//------------------------------------------------------------------------------
//main dialogue function
//------------------------------------------------------------------------------
BOOL CALLBACK DialogProc_main(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uMsg)
  {
    case WM_SIZE:
    {
      unsigned int mWidth  = LOWORD(lParam);
      unsigned int mHeight = HIWORD(lParam);

      //controle de la taille minimum
      if (mWidth<584 ||mHeight<524)
      {
        RECT Rect;
        GetWindowRect(hwnd, &Rect);
        MoveWindow(hwnd,Rect.left,Rect.top,584+20,524+64,TRUE);
      }else RefreshSizeForm(hwnd, mWidth, mHeight);

      InvalidateRect(hwnd, NULL, TRUE);
    }
    break;
    case WM_COMMAND:
      switch (HIWORD(wParam))
      {
        case BN_CLICKED:Global_WM_COMMAND_wParam(wParam, lParam);break;
      }
    break;
    case WM_NOTIFY:
      TRI_DEBUG_VIEW = !TRI_DEBUG_VIEW;
      if (((LPNMHDR)lParam)->code == LVN_COLUMNCLICK)c_Tri(GetDlgItem(hwnd,LV_INFO),((LPNMLISTVIEW)lParam)->iSubItem,TRI_DEBUG_VIEW);
    break;
    case WM_DROPFILES://gestion du drag and drop de fichier ^^
    {
      if (!B_AUTOSEARCH)
      {
        HDROP H_DropInfo=(HDROP)wParam;//récupération de la liste
        char tmp[MAX_PATH];
        DWORD i,nb_path = DragQueryFile(H_DropInfo, 0xFFFFFFFF, tmp, MAX_PATH);
        for (i=0;i<nb_path;i++)
        {
          //traitement des données ^^
          DragQueryFile(H_DropInfo, i, tmp, MAX_PATH);

          //vérification du type de fichier + add^^
          FileToTreeView(tmp);
        }
        DragFinish(H_DropInfo);//libération de la mémoire

        //tri and clean
        CleanTreeViewFiles();

        //expend des branches
        SendDlgItemMessage(h_conf,TRV_FILES,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_FILES]);
        SendDlgItemMessage(h_conf,TRV_FILES,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_LOGS]);
        SendDlgItemMessage(h_conf,TRV_FILES,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_REGISTRY]);
        SendDlgItemMessage(h_conf,TRV_FILES,TVM_EXPAND, TVE_EXPAND,(LPARAM)TRV_HTREEITEM_CONF[FILES_TITLE_APPLI]);
      }
    }
    break;
    case WM_INITDIALOG:InitGUIConfig(hwnd); break;
    case WM_CLOSE:EndGUIConfig(hwnd);break;
  }
  return 0;
}
//------------------------------------------------------------------------------
//main function
//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  //init name
  snprintf(NOM_FULL_APPLI,DEFAULT_TMP_SIZE,"%s %s%s - %s",NOM_APPLI,FULLVERSION_STRING,STATUS_SHORT,URL_APPLI);

  //check if console or GUI mode
  if (argc == 1)//no params = GUI mode
  {
    CONSOL_ONLY = FALSE;
    return DialogBox(0, MAKEINTRESOURCE(DLG_MAIN), NULL, (DLGPROC)DialogProc_main);
  }else
  {
    if (sqlite3_open(DEFAULT_SQLITE_FILE, &db_scan) != SQLITE_OK)
    {
      //if tmp sqlite file exist free !!
      if (GetFileAttributes(DEFAULT_TM_SQLITE_FILE) != INVALID_FILE_ATTRIBUTES)
      {
        DeleteFile(DEFAULT_TM_SQLITE_FILE);
      }
      if (sqlite3_open(DEFAULT_SQLITE_FILE, &db_scan) != SQLITE_OK)
      {
        AddDebugMessage("init", "Error to open sqlite file !", "NOK", MSG_ERROR);
      }
    }
    SetDebugPrivilege(TRUE);

    nb_file_cmd         = 0;
    nb_path_cmd         = 0;
    export_type         = SAVE_TYPE_CSV;
    current_session_id  = 0;
    scan_in_stop_state  = FALSE;
    start_scan          = FALSE;

    DEBUG_CMD_MODE      = TRUE;
    CONSOL_ONLY         = TRUE;
    LOCAL_SCAN          = TRUE;
    DEBUG_MODE          = FALSE;
    FILE_ACL            = FALSE;
    FILE_ADS            = FALSE;
    FILE_SHA            = FALSE;
    REGISTRY_RECOVERY   = FALSE;

    FORMAT_CALBAK_READ_INFO fcri;
    char request[MAX_LINE_SIZE];

    //params
    unsigned int i;
    for (i=0;i<argc;i++)
    {
      if (argv[i][0] == '-') //params
      {
        switch(argv[i][1])
        {
          case 'h'://help
            printf(
                   "*******************************************************\n"
                   "** %s\n"
                   "*******************************************************\n"
                   "\n"
                   "SYNOPSIS\n"
                   "\tRtCA.exe [-l|-L|-t]\n"
                   "\t         [-d][-r][-0][-1][-2]\n"
                   "\t         [-f \"...\"]\n"
                   "\t         [-p \"...\"]\n"
                   "\t         [-a|-A|-s ...]\n"
                   "\t         [-o \"...\"][-F [CSV|XML|HTML]]\n"
                   "\n"
                   "DESCRITPION\n"
                   "\tTool To help forensic investigations :)\n"
                   "\n"
                   "OPTIONS\n"
                   "\t-l  List all sessions or select the session number.\n\t    Exemple: -l 125\n\n"
                   "\t-L  List all languages or select it for export.\n\t    Exemple for english export (default): -L 1\n\n"
                   "\t-t  List all tests.\n\n"
                   "\n"
                   "\t-d  Enable Debug mode.\n\n"
                   "\t-r  Enable registry recovery mode for registry files damaged.\n\n"
                   "\t-0  Enable ACL check in files test.\n\n"
                   "\t-1  Enable ADS check in files test.\n\n"
                   "\t-2  Enable SHA in files test.\n\n"
                   "\n"
                   "\t-f  Import file to check.\n\t    Exemple: -f \"c:\\file.reg\"\n\n"
                   "\t-p  Import path to check in file test.\n\t    Exemple: -p \"c:\\directory\\\"\n\n"
                   "\n"
                   "\t-a  Start all tests.\n\n"
                   "\t-A  Start all tests in safe mode with no Files and no Logs test.\n\n"
                   "\t-s  Select test to start.\n\t    Exemple: -s 0 1 2 3 4 5 6\n\n"
                   "\n"
                   "\t-o  Export all data to path.\n\t    Exemple: -o \"c:\\\"\n"
                   "\t-F  Format to export : CSV (default), XML or HTML\n"
                   "\n"
                   ,NOM_FULL_APPLI);
            system("PAUSE");
          break;
          case 'l'://list of all sessions + select
            printf("*******************************************************\n"
                   "** %s\n"
                   "*******************************************************\n"
                   "\n"
                   "List of sessions :\n",NOM_FULL_APPLI);
            if (i+1<argc)
            {
              i++;
              if (argv[i][0]!='-')
              {
                current_session_id = atoi(argv[i]);//select a session
              }else
              {
                fcri.type = CMD_TYPE_LIST_SESSION;
                snprintf(request,MAX_LINE_SIZE,"SELECT id,name FROM session;");
                sqlite3_exec(db_scan, request, callback_sqlite_CMD, &fcri, NULL);
              }
            }else
            {
              fcri.type = CMD_TYPE_LIST_SESSION;
              snprintf(request,MAX_LINE_SIZE,"SELECT id,name FROM session;");
              sqlite3_exec(db_scan, request, callback_sqlite_CMD, &fcri, NULL);
            }
            system("PAUSE");
          break;
          case 'L'://list of all languages supported + select
            item_n = 0;
            printf("*******************************************************\n"
                   "** %s\n"
                   "*******************************************************\n"
                   "\n"
                   "List of languages :\n",NOM_FULL_APPLI);
            if (i+1<argc)
            {
              i++;
              if (argv[i][0]!='-')
              {
                current_session_id = atoi(argv[i]);//select a session
              }else
              {
                fcri.type = CMD_TYPE_LIST_LANGUAGE;
                snprintf(request,MAX_LINE_SIZE,"SELECT name FROM language;");
                sqlite3_exec(db_scan, request, callback_sqlite_CMD, &fcri, NULL);
              }
            }else
            {
              fcri.type = CMD_TYPE_LIST_LANGUAGE;
              snprintf(request,MAX_LINE_SIZE,"SELECT id,name FROM language;");
              sqlite3_exec(db_scan, request, callback_sqlite_CMD, &fcri, NULL);
            }
            system("PAUSE");
          break;
          case 't'://list of all tests
            printf("*******************************************************\n"
                   "** %s\n"
                   "*******************************************************\n"
                   "\n"
                   "List of tests :\n",NOM_FULL_APPLI);
            fcri.type = CMD_TYPE_LIST_TESTS;
            snprintf(request,MAX_LINE_SIZE,"SELECT id_item,string FROM language_strings WHERE id_language=1 ORDER BY id_item;");
            sqlite3_exec(db_scan, request, callback_sqlite_CMD, &fcri, NULL);
            system("PAUSE");
          break;
          case 'd'://enable mode debug
            DEBUG_MODE = TRUE;
          break;
          case 'r'://enable registry recovery mode
            REGISTRY_RECOVERY = TRUE;
          break;
          case '0'://enable ACL for tests files
            FILE_ACL = TRUE;
          break;
          case '1'://enable ADS for tests files
            FILE_ADS = TRUE;
          break;
          case '2'://enable SHA hashes for tests files
            FILE_SHA = TRUE;
          break;

          case 'f'://import file to scan
            for (++i;i<argc;i++)
            {
              if (argv[i][0] == '-'){i--;break;}

              //get files
              if (nb_file_cmd<NB_MAX_ITEM_CMD)
              {
                strncpy(file_cmd[nb_file_cmd++].c,argv[i],MAX_PATH);
              }else break;
              LOCAL_SCAN = FALSE;
            }
          break;
          case 'p'://scan path for file test
            for (++i;i<argc;i++)
            {
              if (argv[i][0] == '-'){i--;break;}

              //get files
              if (nb_path_cmd<NB_MAX_ITEM_CMD)
              {
                strncpy(path_cmd[nb_path_cmd++].c,argv[i],MAX_PATH);
              }else break;
              LOCAL_SCAN = FALSE;
            }
          break;

          case 'a'://scan with all tests
            CMDScan((LPVOID)FALSE);
          break;
          case 'A'://scan with all tests in safe mode with no log and no file test
            CMDScan((LPVOID)TRUE);
          break;
          case 's'://scan for test with number 0 1 2 3
            //generate new session
            AddNewSession(LOCAL_SCAN,db_scan);

            for (++i;i<argc;i++)
            {
              if (argv[i][0] == '-'){i--;break;}

              CMDScanNum((LPVOID)atoi(argv[i]));
            }
          break;
          case 'o'://save alls data session to path
            if (i+1<argc)
            {
              i++;
              if (argv[i][0] == '-'){i--;break;}

              strncpy(current_test_export_path,argv[i],MAX_PATH);
              ExportStart = TRUE;
              SaveAll(NULL);
            }
          break;
          case 'F'://save alls data session to path
            if (i+1<argc)
            {
              i++;
              if (argv[i][0] == '-'){i--;break;}

              switch(argv[i][0])
              {
                case 'x':
                case 'X':export_type     = SAVE_TYPE_XML;break;
                case 'h':
                case 'H':export_type     = SAVE_TYPE_HTML;break;
              }
            }
          break;
        }
      }else continue;//error
    }
    SetDebugPrivilege(FALSE);
    return 0;
  }
}
