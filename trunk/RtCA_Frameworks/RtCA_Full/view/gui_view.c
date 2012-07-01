//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
//view form
//------------------------------------------------------------------------------
BOOL CALLBACK DialogProc_view(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uMsg)
  {
    case WM_SIZE:
    {
      unsigned int mWidth         = LOWORD(lParam);
      unsigned int mHeight        = HIWORD(lParam);

      MoveWindow(GetDlgItem(hwnd,ED_SEARCH), 2, 0,mWidth-348,20, TRUE);
      MoveWindow(GetDlgItem(hwnd,BT_SEARCH), mWidth-346, 0,91,20, TRUE);
      MoveWindow(GetDlgItem(hwnd,CB_VIEW), mWidth-250, 0, 250,520, TRUE);

      if (VIEW_RESULTS_DBL)
      {
        MoveWindow(GetDlgItem(hwnd,TRV_VIEW), 2, HEIGHT_SEARCH, mWidth/2,mHeight-HEIGHT_SEARCH-5, TRUE);
        MoveWindow(GetDlgItem(hwnd,LV_VIEW), mWidth/2+4, HEIGHT_SEARCH, mWidth/2-6,mHeight-HEIGHT_SEARCH-5, TRUE);

        //column resise
        unsigned int i;
        if (nb_current_columns!=0)for (i=0;i<nb_current_columns;i++)redimColumn(hwnd,LV_VIEW,i,(mWidth/2-30)/nb_current_columns);
      }else
      {
        MoveWindow(GetDlgItem(hwnd,LV_VIEW), 2, HEIGHT_SEARCH, mWidth,mHeight-HEIGHT_SEARCH-5, TRUE);

        //column resise
        unsigned int i;
        if (nb_current_columns!=0)for (i=0;i<nb_current_columns;i++)redimColumn(hwnd,LV_VIEW,i,(mWidth-30)/nb_current_columns);
      }

      InvalidateRect(hwnd, NULL, TRUE);
    }
    break;
    case WM_COMMAND:
      switch (HIWORD(wParam))
      {
        case BN_CLICKED:Global_WM_COMMAND_wParam(wParam, lParam);break;
        case CBN_SELCHANGE:
        {
          //for change in combobox selection
          switch(LOWORD(wParam))
          {
            case CB_VIEW :
            {
              //test if selection is valid ! (for protect from crash)
              if (columns_params[SendDlgItemMessage((HWND)h_view,CB_VIEW,CB_GETCURSEL,(WPARAM)0,(LPARAM)0)].nb_columns==0)break;

              TRI_RESULT_VIEW = FALSE;//init tri!
              pos_search = 0;         //init search

              FORMAT_CALBAK_READ_INFO fcri;
              fcri.type = TYPE_SQLITE_FLAG_VIEW_CHANGE;
              SQLITE_LireData(&fcri, DEFAULT_SQLITE_FILE);
              AddDebugMessage("gui_view", "Change selection of section view", "OK", MSG_INFO);
            }
            break;
          }
        }
        break;
      }
    break;
    case WM_CONTEXTMENU://popup menu
      //if item
      if (SendDlgItemMessage(hwnd,LV_VIEW,LVM_GETITEMCOUNT,(WPARAM)NULL, (LPARAM)NULL)>0)
      {
        HMENU hmenu;
        if ((hmenu = LoadMenu(GetModuleHandle(0), MAKEINTRESOURCE(POPUP_LSTV)))!= NULL)
        {
          //set text !!!
          ModifyMenu(hmenu,POPUP_S_VIEW,MF_BYCOMMAND|MF_STRING,POPUP_S_VIEW,cps[TXT_POPUP_S_VIEW].c);
          ModifyMenu(hmenu,POPUP_S_SELECTION,MF_BYCOMMAND|MF_STRING,POPUP_S_SELECTION,cps[TXT_POPUP_S_SELECTION].c);
          ModifyMenu(hmenu,POPUP_A_SEARCH,MF_BYCOMMAND|MF_STRING,POPUP_A_SEARCH,cps[TXT_POPUP_A_SEARCH].c);


          //load column text
          char buffer[DEFAULT_TMP_SIZE]="";
          LVCOLUMN lvc;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = DEFAULT_TMP_SIZE;
          lvc.pszText = buffer;

          unsigned int i=0;
          while (SendDlgItemMessage(hwnd,LV_VIEW,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc))
          {
            ModifyMenu(hmenu,POPUP_I_00+i,MF_BYCOMMAND|MF_STRING,POPUP_I_00+i,buffer);

            //reinit
            buffer[0] = 0;
            lvc.mask = LVCF_TEXT;
            lvc.cchTextMax = DEFAULT_TMP_SIZE;
            lvc.pszText = buffer;
            i++;
          }

          //remove other items
          for (;i<NB_POPUP_I;i++)RemoveMenu(hmenu,POPUP_I_00+i,MF_BYCOMMAND|MF_GRAYED);

          //affichage du popup menu
          TrackPopupMenuEx(GetSubMenu(hmenu, 0), 0, LOWORD(lParam), HIWORD(lParam), hwnd, NULL);
          DestroyMenu(hmenu);
        }
      }
    break;
    case WM_NOTIFY: //column tri !
      TRI_RESULT_VIEW = !TRI_RESULT_VIEW;
      if (((LPNMHDR)lParam)->code == LVN_COLUMNCLICK)c_Tri(GetDlgItem(hwnd,LV_VIEW),((LPNMLISTVIEW)lParam)->iSubItem,TRI_RESULT_VIEW);
    break;
  }
  return 0;
}
