//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
DWORD WINAPI BackupRegFile(LPVOID lParam)
{
  //choix du répertoire de destination ^^
  BROWSEINFO browser;
  LPITEMIDLIST lip;
  char tmp[MAX_PATH+1]="";
  char cmd[MAX_PATH*2]="";
  browser.hwndOwner = Tabl[TABL_MAIN];;
  browser.pidlRoot = 0;
  browser.lpfn = 0;
  browser.iImage = 0;
  browser.ulFlags = BIF_NEWDIALOGSTYLE; //permet la création d'un dossier
  browser.lParam = 0;
  browser.pszDisplayName = tmp;  //résultat ici
  browser.lpszTitle = "Directory to save :";
  lip = SHBrowseForFolder(&browser);
  if (lip != NULL)
  {
    SHGetPathFromIDList(lip,tmp);
    if (strlen(tmp)>0)
    {
      //http://msdn.microsoft.com/en-us/library/windows/desktop/ms724877%28v=vs.85%29.aspx
      //Software
      snprintf(cmd,MAX_PATH*2,"SAVE HKLM\\Software \"%s\\SOFTWARE\"",tmp);
      ShellExecute(Tabl[TABL_MAIN], "open","REG",cmd,NULL,SW_HIDE);

      //System
      snprintf(cmd,MAX_PATH*2,"SAVE HKLM\\System \"%s\\SYSTEM\"",tmp);
      ShellExecute(Tabl[TABL_MAIN], "open","REG",cmd,NULL,SW_HIDE);

      //SAM
      snprintf(cmd,MAX_PATH*2,"SAVE HKLM\\SAM \"%s\\SAM\"",tmp);
      ShellExecute(Tabl[TABL_MAIN], "open","REG",cmd,NULL,SW_HIDE);

      //Security
      snprintf(cmd,MAX_PATH*2,"SAVE HKLM\\Security \"%s\\SECURITY\"",tmp);
      ShellExecute(Tabl[TABL_MAIN], "open","REG",cmd,NULL,SW_HIDE);

      //Current user
      snprintf(cmd,MAX_PATH*2,"SAVE HKCU \"%s\\NTUSER.DAT\"",tmp);
      ShellExecute(Tabl[TABL_MAIN], "open","REG",cmd,NULL,SW_HIDE);

      //Default
      snprintf(cmd,MAX_PATH*2,"SAVE HKU\\.DEFAULT \"%s\\DEFAULT\"",tmp);
      ShellExecute(Tabl[TABL_MAIN], "open","REG",cmd,NULL,SW_HIDE);

      //Config hardware volatile keys
      snprintf(cmd,MAX_PATH*2,"SAVE HKLM\\HARDWARE \"%s\\HARDWARE_volatile_key\"",tmp);
      ShellExecute(Tabl[TABL_MAIN], "open","REG",cmd,NULL,SW_HIDE);

      MessageBox(0,tmp,"Done - Registry copy to:",MB_OK|MB_TOPMOST);
    }
  }
  return 0;
}
//------------------------------------------------------------------------------
void ExportLVSelecttoCSV(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne)
{
  //test si des enregistrements
  DWORD NBLigne=SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    if (nb_colonne>0)
    {
      //traitement
      HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        char ligne [MAX_LINE_SIZE], buffer[MAX_LINE_SIZE];
        DWORD copiee;

        //création de la première ligne de titre
        ligne[0]=0;
        LVCOLUMN lvc;
        lvc.mask = LVCF_TEXT;
        lvc.cchTextMax = MAX_LINE_SIZE;
        lvc.pszText = buffer;

        DWORD i =0,j;
        while (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc) && i<nb_colonne)
        {
          strncat(ligne,"\"",MAX_LINE_SIZE);
          strncat(ligne,buffer,MAX_LINE_SIZE);
          strncat(ligne,"\";\0",MAX_LINE_SIZE);

          //réinit
          buffer[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = buffer;
          i++;
        }
        strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
        WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);

        //traitement des autres lignes
        HANDLE hlv = GetDlgItem(Tabl[id_tabl],lv);
        for (j=0;j<NBLigne;j++)//ligne par ligne
        {
          //on vérifie que la ligne est bien sélectionnée
          if (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMSTATE,(WPARAM)j,(LPARAM)LVIS_SELECTED) == LVIS_SELECTED)
          {
            ligne[0]=0;
            for (i=0;i<nb_colonne;i++)//colonne par colonne
            {
              buffer[0]=0;
              ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
              strncat(ligne,"\"",MAX_LINE_SIZE);
              strncat(ligne,buffer,MAX_LINE_SIZE);
              strncat(ligne,"\";\0",MAX_LINE_SIZE);
            }
            strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
            WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
          }
        }
      }
      CloseHandle(MyhFile);
    }
  }
}
//------------------------------------------------------------------------------
void ExportLVtoCSV(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne)
{
  //test si des enregistrements
  DWORD NBLigne=SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    if (nb_colonne>0)
    {
      //traitement
      HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        char ligne [MAX_LINE_SIZE+1], buffer[MAX_LINE_SIZE+1];
        DWORD copiee;

        //création de la première ligne de titre
        ligne[0]=0;
        LVCOLUMN lvc;
        lvc.mask = LVCF_TEXT;
        lvc.cchTextMax = MAX_LINE_SIZE;
        lvc.pszText = buffer;

        DWORD i =0,j;
        while (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc) && i<nb_colonne)
        {
          if (strlen(buffer)>0)
          {
            strncat(ligne,"\"",MAX_LINE_SIZE);
            strncat(ligne,buffer,MAX_LINE_SIZE);
            strncat(ligne,"\";\0",MAX_LINE_SIZE);
          }

          //réinit
          buffer[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = buffer;
          i++;
        }
        strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
        copiee = 0;
        WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);

        //traitement des autres lignes
        HANDLE hlv = GetDlgItem(Tabl[id_tabl],lv);
        for (j=0;j<NBLigne;j++)//ligne par ligne
        {
          ligne[0]=0;
          for (i=0;i<nb_colonne;i++)//colonne par colonne
          {
            buffer[0]=0;
            ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              strncat(ligne,"\"",MAX_LINE_SIZE);
              strncat(ligne,buffer,MAX_LINE_SIZE-MAX_PATH);
              strncat(ligne,"\";",MAX_LINE_SIZE);
            }else strncat(ligne,"\"\";",MAX_LINE_SIZE);
          }
          strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
          copiee = 0;
          WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
        }
      }
      CloseHandle(MyhFile);
    }
  }
}
//------------------------------------------------------------------------------
void ExportLVtoHTML(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne)
{
  //test si des enregistrements
  DWORD NBLigne=SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    if (nb_colonne>0)
    {
      //traitement
      HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        char ligne [MAX_LINE_SIZE+1], buffer[MAX_LINE_SIZE+1];
        DWORD copiee;

        //start
        char head[]="<html>\r\n<head><title>RtCA report [http://code.google.com/p/omnia-projetcs/]</title></head>\r\n<table border=\"0\" width=\"100%\" cellspacing=\"1\" cellpadding=\"1\">\r\n<tr bgcolor=\"#C0C0C0\">";
        WriteFile(MyhFile,head,strlen(head),&copiee,0);

        //création de la première ligne de titre
        ligne[0]=0;
        LVCOLUMN lvc;
        lvc.mask = LVCF_TEXT;
        lvc.cchTextMax = MAX_LINE_SIZE;
        lvc.pszText = buffer;

        DWORD i =0,j;
        while (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc) && i<nb_colonne)
        {
          if (strlen(buffer)>0)
          {
            strncat(ligne,"<th>",MAX_LINE_SIZE);
            strncat(ligne,buffer,MAX_LINE_SIZE);
            strncat(ligne,"</th>\0",MAX_LINE_SIZE);
          }

          //réinit
          buffer[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = buffer;
          i++;
        }
        strncat(ligne,"</tr>\r\n\0",MAX_LINE_SIZE);
        copiee = 0;
        WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);

        //traitement des autres lignes
        HANDLE hlv = GetDlgItem(Tabl[id_tabl],lv);
        for (j=0;j<NBLigne;j++)//ligne par ligne
        {
          if (j%2==1)strcpy(ligne,"<tr bgcolor=\"#ddddff\">");
          else strcpy(ligne,"<tr>");

          for (i=0;i<nb_colonne;i++)//colonne par colonne
          {
            buffer[0]=0;
            ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              strncat(ligne,"<td>",MAX_LINE_SIZE);
              strncat(ligne,buffer,MAX_LINE_SIZE-MAX_PATH);
              strncat(ligne,"</td>",MAX_LINE_SIZE);
            }else strncat(ligne,"<td></td>",MAX_LINE_SIZE);
          }
          strncat(ligne,"</tr>\r\n\0",MAX_LINE_SIZE);
          copiee = 0;
          WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
        }

        //fin de ligne
        WriteFile(MyhFile,"</table>\r\n</html>",17,&copiee,0);
      }
      CloseHandle(MyhFile);
    }
  }
}
//------------------------------------------------------------------------------
void ExportLVSelecttoHTML(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne)
{
  //test si des enregistrements
  DWORD NBLigne=SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    if (nb_colonne>0)
    {
      //traitement
      HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        char ligne [MAX_LINE_SIZE], buffer[MAX_LINE_SIZE];
        DWORD copiee;

        //start
        char head[]="<html>\r\n<head><title>RtCA report [http://code.google.com/p/omnia-projetcs/]</title></head>\r\n<table border=\"0\" width=\"100%\" cellspacing=\"1\" cellpadding=\"1\">\r\n<tr bgcolor=\"#C0C0C0\">";
        WriteFile(MyhFile,head,strlen(head),&copiee,0);

        //création de la première ligne de titre
        ligne[0]=0;
        LVCOLUMN lvc;
        lvc.mask = LVCF_TEXT;
        lvc.cchTextMax = MAX_LINE_SIZE;
        lvc.pszText = buffer;

        DWORD i =0,j;
        while (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc) && i<nb_colonne)
        {
          strncat(ligne,"<th>",MAX_LINE_SIZE);
          strncat(ligne,buffer,MAX_LINE_SIZE);
          strncat(ligne,"</th>\0",MAX_LINE_SIZE);

          //réinit
          buffer[0]=0;
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = buffer;
          i++;
        }
        strncat(ligne,"</tr>\r\n\0",MAX_LINE_SIZE);
        WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);

        //traitement des autres lignes
        HANDLE hlv = GetDlgItem(Tabl[id_tabl],lv);
        for (j=0;j<NBLigne;j++)//ligne par ligne
        {
          //on vérifie que la ligne est bien sélectionnée
          if (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMSTATE,(WPARAM)j,(LPARAM)LVIS_SELECTED) == LVIS_SELECTED)
          {
            if (j%2==1)strcpy(ligne,"<tr bgcolor=\"#ddddff\">");
            else strcpy(ligne,"<tr>");
            for (i=0;i<nb_colonne;i++)//colonne par colonne
            {
              buffer[0]=0;
              ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
              strncat(ligne,"<td>",MAX_LINE_SIZE);
              strncat(ligne,buffer,MAX_LINE_SIZE);
              strncat(ligne,"</td>",MAX_LINE_SIZE);
            }
            strncat(ligne,"</tr>\r\n\0",MAX_LINE_SIZE);
            WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
          }
        }

        //fin de ligne
        WriteFile(MyhFile,"</table>\r\n</html>",17,&copiee,0);
      }
      CloseHandle(MyhFile);
    }
  }
}
//------------------------------------------------------------------------------
void ExportLVtoXML(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne)
{
  //test si des enregistrements
  DWORD NBLigne=SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    if (nb_colonne>0)
    {
      //traitement
      HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        char ligne [MAX_LINE_SIZE+1], buffer[MAX_LINE_SIZE+1];
        DWORD copiee;
        LINE_ITEM lv_line[SIZE_UTIL_ITEM+1];

        //start
        char head[]="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n<RtCA>\r\n  <Description><![CDATA[RtCA report [http://code.google.com/p/omnia-projetcs/]]]></Description>\r\n";
        WriteFile(MyhFile,head,strlen(head),&copiee,0);

        //création de la première ligne de titre
        ligne[0]=0;
        LVCOLUMN lvc;
        lvc.mask = LVCF_TEXT;
        lvc.cchTextMax = MAX_LINE_SIZE;
        lvc.pszText = lv_line[0].c;

        DWORD i =0,j;
        while (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc) && i<nb_colonne)
        {
          //réinit
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = lv_line[++i].c;
        }

        //traitement des autres lignes
        HANDLE hlv = GetDlgItem(Tabl[id_tabl],lv);
        for (j=0;j<NBLigne;j++)//ligne par ligne
        {
          WriteFile(MyhFile,"  <Data>\r\n",10,&copiee,0);
          ligne[0]=0;
          for (i=0;i<nb_colonne;i++)//colonne par colonne
          {
            buffer[0]=0;
            ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
            if (buffer != NULL && strlen(buffer)>0)
            {
              snprintf(ligne,MAX_LINE_SIZE,"     <%s><![CDATA[%s]]><%s>\r\n",lv_line[i].c,buffer,lv_line[i].c);
              WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
            }
          }
          WriteFile(MyhFile,"  </Data>\r\n",11,&copiee,0);
        }
        //fin de ligne
        WriteFile(MyhFile,"</RtCA>",6,&copiee,0);
      }
      CloseHandle(MyhFile);
    }
  }
}
//------------------------------------------------------------------------------
void ExportLVSelecttoXML(char *path, unsigned int id_tabl, int lv, unsigned short nb_colonne)
{
  //test si des enregistrements
  DWORD NBLigne=SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    if (nb_colonne>0)
    {
      //traitement
      HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
      if (MyhFile != INVALID_HANDLE_VALUE)
      {
        char ligne [MAX_LINE_SIZE], buffer[MAX_LINE_SIZE];
        DWORD copiee;
        LINE_ITEM lv_line[SIZE_UTIL_ITEM];

        //start
        char head[]="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\r\n<RtCA>\r\n  <Description><![CDATA[RtCA report [http://code.google.com/p/omnia-projetcs/]]]></Description>\r\n";
        WriteFile(MyhFile,head,strlen(head),&copiee,0);

        //création de la première ligne de titre
        ligne[0]=0;
        LVCOLUMN lvc;
        lvc.mask = LVCF_TEXT;
        lvc.cchTextMax = MAX_LINE_SIZE;
        lvc.pszText = lv_line[0].c;

        DWORD i =0,j;
        while (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETCOLUMN,(WPARAM)i,(LPARAM)&lvc) && i<nb_colonne)
        {
          //réinit
          lvc.mask = LVCF_TEXT;
          lvc.cchTextMax = MAX_LINE_SIZE;
          lvc.pszText = lv_line[++i].c;
        }

        //traitement des autres lignes
        HANDLE hlv = GetDlgItem(Tabl[id_tabl],lv);
        for (j=0;j<NBLigne;j++)//ligne par ligne
        {
          //on vérifie que la ligne est bien sélectionnée
          if (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMSTATE,(WPARAM)j,(LPARAM)LVIS_SELECTED) == LVIS_SELECTED)
          {
            WriteFile(MyhFile,"  <Data>\r\n",10,&copiee,0);
            ligne[0]=0;
            for (i=0;i<nb_colonne;i++)//colonne par colonne
            {
              buffer[0]=0;
              ListView_GetItemText(hlv,j,i,buffer,MAX_LINE_SIZE);
              if (buffer != NULL && strlen(buffer)>0)
              {
                snprintf(ligne,MAX_LINE_SIZE,"     <%s><![CDATA[%s]]><%s>\r\n",lv_line[i].c,buffer,lv_line[i].c);
                WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
              }
            }
            WriteFile(MyhFile,"  </Data>\r\n",11,&copiee,0);
          }
        }

        //fin de ligne
        WriteFile(MyhFile,"</RtCA>",6,&copiee,0);
      }
      CloseHandle(MyhFile);
    }
  }
}
//------------------------------------------------------------------------------
void ExportLVColto(char *path, unsigned int id_tabl, int lv, unsigned short col)
{
  //test si des enregistrements
  DWORD NBLigne=SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    //traitement
    HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
    if (MyhFile != INVALID_HANDLE_VALUE)
    {
      char ligne [MAX_LINE_SIZE];
      DWORD copiee;
      DWORD j;

      //traitement des lignes
      HANDLE hlv = GetDlgItem(Tabl[id_tabl],lv);
      for (j=0;j<NBLigne;j++)//ligne par ligne
      {
        ligne[0]=0;
        ListView_GetItemText(hlv,j,col,ligne,MAX_LINE_SIZE);
        if (ligne[0]!=0)
        {
          strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
          WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
        }
      }
    }
    CloseHandle(MyhFile);
  }
}
//------------------------------------------------------------------------------
void ExportLVSelectColto(char *path, unsigned int id_tabl, int lv, unsigned short col)
{
  //test si des enregistrements
  DWORD NBLigne=SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);
  if (NBLigne>0)
  {
    //traitement
    HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
    if (MyhFile != INVALID_HANDLE_VALUE)
    {
      char ligne [MAX_LINE_SIZE];
      DWORD copiee;
      DWORD j;

      //traitement des autres lignes
      HANDLE hlv = GetDlgItem(Tabl[id_tabl],lv);
      for (j=0;j<NBLigne;j++)//ligne par ligne
      {
        //on vérifie que la ligne est bien sélectionnée
        if (SendDlgItemMessage(Tabl[id_tabl],lv,LVM_GETITEMSTATE,(WPARAM)j,(LPARAM)LVIS_SELECTED) == LVIS_SELECTED)
        {
          ligne[0]=0;
          ListView_GetItemText(hlv,j,col,ligne,MAX_LINE_SIZE);
          if (ligne[0]!=0)
          {
            strncat(ligne,"\r\n\0",MAX_LINE_SIZE);
            WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
          }
        }
      }
    }
    CloseHandle(MyhFile);
  }
}
//------------------------------------------------------------------------------
void LVSaveAll(unsigned int id_tabl, int lv,unsigned short nb_colonne,BOOL selection_only,BOOL pwdump)
{
  char path[MAX_PATH]="";

  OPENFILENAME ofn;
  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = Tabl[TABL_MAIN];
  ofn.lpstrFile = path;
  ofn.nMaxFile = MAX_PATH;
  if (pwdump) ofn.lpstrFilter ="File CSV\0*.csv\0File HTML\0*.html\0File XML\0*.xml\0Pwdump file\0*.pwdump\0";
  else ofn.lpstrFilter ="File CSV\0*.csv\0File HTML\0*.html\0File XML\0*.xml\0";

  ofn.nFilterIndex = 1;
  ofn.Flags =OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
  ofn.lpstrDefExt =".csv\0";

  if (GetSaveFileName(&ofn)==TRUE)
  {
    if (ofn.nFilterIndex == 1) //csv
    {
      //sauvegarde
      if (selection_only)ExportLVSelecttoCSV(path, id_tabl, lv, nb_colonne);
      else ExportLVtoCSV(path, id_tabl, lv, nb_colonne);
    }else if (ofn.nFilterIndex == 2) //html
    {
      if (selection_only)ExportLVSelecttoHTML(path, id_tabl, lv, 9);
      else ExportLVtoHTML(path, id_tabl, lv, 9);
    }else if (ofn.nFilterIndex == 3) //XML
    {
      if (selection_only)ExportLVSelecttoXML(path, id_tabl, lv, 9);
      else ExportLVtoXML(path, id_tabl, lv, 9);
    }else if (ofn.nFilterIndex == 4) //pwdump
    {
      if (selection_only)ExportLVSelectColto(path, id_tabl, lv, 9);
      else ExportLVColto(path, id_tabl, lv, 9);
    }
  }
}
//------------------------------------------------------------------------------
void ExportTrv(HANDLE htree, char *path)
{
  //traitement
  HANDLE MyhFile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, 0);
  if (MyhFile != INVALID_HANDLE_VALUE)
  {
    char ligne [MAX_PATH], tmp[MAX_PATH];
    DWORD copiee;
    TVITEM tvitem;

    //création de la première ligne de titre
    strcpy(ligne,"\"Type\";\"Path\"\r\n");
    WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);

    //traitement des ruches une par une
    //récupération du 1er fils dans le treeview
    HTREEITEM hitem = (HTREEITEM)SendMessage(htree, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM[TRV_LOGS]);
    do
    {
      //récupération du texte de l'item
      tmp[0]=0;
      tvitem.hItem = hitem;
      tvitem.mask = TVIF_TEXT;
      tvitem.pszText = tmp;
      tvitem.cchTextMax = MAX_PATH;
      if (SendMessage(htree, TVM_GETITEM,(WPARAM)0, (LPARAM)&tvitem))
      {
        snprintf(ligne,MAX_PATH,"\"LOGS\";\"%s\"\r\n",tmp);
        WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
      }

    }while((hitem = (HTREEITEM)SendMessage(htree, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem)));

    hitem = (HTREEITEM)SendMessage(htree, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM[TRV_FILES]);
    do
    {
      //récupération du texte de l'item
      tmp[0]=0;
      tvitem.hItem = hitem;
      tvitem.mask = TVIF_TEXT;
      tvitem.pszText = tmp;
      tvitem.cchTextMax = MAX_PATH;
      if (SendMessage(htree, TVM_GETITEM,(WPARAM)0, (LPARAM)&tvitem))
      {
        snprintf(ligne,MAX_PATH,"\"FILES\";\"%s\"\r\n",tmp);
        WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
      }

    }while((hitem = (HTREEITEM)SendMessage(htree, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem)));

    hitem = (HTREEITEM)SendMessage(htree, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM[TRV_REGISTRY]);
    do
    {
      //récupération du texte de l'item
      tmp[0]=0;
      tvitem.hItem = hitem;
      tvitem.mask = TVIF_TEXT;
      tvitem.pszText = tmp;
      tvitem.cchTextMax = MAX_PATH;
      if (SendMessage(htree, TVM_GETITEM,(WPARAM)0, (LPARAM)&tvitem))
      {
        snprintf(ligne,MAX_PATH,"\"REGISTRY\";\"%s\"\r\n",tmp);
        WriteFile(MyhFile,ligne,strlen(ligne),&copiee,0);
      }

    }while((hitem = (HTREEITEM)SendMessage(htree, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem)));
  }
  CloseHandle(MyhFile);
}
//------------------------------------------------------------------------------
void TreeExport(HANDLE htree)
{
  char path[MAX_PATH]="";

  OPENFILENAME ofn;
  ZeroMemory(&ofn, sizeof(OPENFILENAME));
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = Tabl[TABL_MAIN];
  ofn.lpstrFile = path;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrFilter ="File CSV\0*.csv\0";
  ofn.nFilterIndex = 1;
  ofn.Flags =OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
  ofn.lpstrDefExt =".csv\0";

  if (GetSaveFileName(&ofn)==TRUE)
  {
    //sauvegarde
    ExportTrv(htree, path);
  }
}
//------------------------------------------------------------------------------
//export global des résultats
DWORD WINAPI Export(LPVOID lParam)
{
  //emplacement de la sauvegarde répertoire
  BROWSEINFO browser;
  ITEMIDLIST *lip;
  char *c,tmp[MAX_PATH]="",path[MAX_PATH]="";

   browser.hwndOwner          = Tabl[TABL_MAIN];
   browser.pidlRoot           = NULL;
   browser.pszDisplayName     = path;
   browser.lpszTitle          = "Destination directory :";
   browser.ulFlags            = BIF_NEWDIALOGSTYLE; //permet la création de répertoire en direct
   browser.lpfn               = NULL;
   if((lip=SHBrowseForFolder(&browser))!=NULL)
   {
     SHGetPathFromIDList(lip,path);
     if (strlen(path)>0)
     {
      //création du path initial + date
      strncat(path,"\\RTCA[\0",MAX_PATH);
      time_t dateEtHMs;
      time(&dateEtHMs);

      //on supprime les caractères incompatibles avec le nom d'un fichier
      strncpy(tmp,(char *)ctime(&dateEtHMs),MAX_PATH);
      tmp[strlen(tmp)-1]=0;
      c = tmp;
      *(c+=3) = '_';
      *(c+=4) = '_';
      *(c+=3) = '_';
      *(c+=3) = 'h';
      *(c+=3) = 'm';
      *(c+=3) = '_';
      strncat(path,tmp,MAX_PATH);
      strncat(path,"]\0",MAX_PATH);

     //création des rapports
     strcpy(tmp,path);
     strncat(tmp,"_AUDIT.csv",MAX_PATH);
     ExportLVtoCSV(tmp, TABL_LOGS, LV_LOGS_VIEW, NB_COLONNE_LV[LV_LOGS_VIEW_NB_COL]);
     strcpy(tmp,path);
     strncat(tmp,"_FILES.csv",MAX_PATH);
     ExportLVtoCSV(tmp, TABL_FILES, LV_FILES_VIEW, NB_COLONNE_LV[LV_FILES_VIEW_NB_COL]);
     strcpy(tmp,path);
     strncat(tmp,"_REG_REGISTRY.csv",MAX_PATH);
     ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_VIEW, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
     strcpy(tmp,path);
     strncat(tmp,"_REG_CONGIGURATION.csv",MAX_PATH);
     ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_CONF, NB_COLONNE_LV[LV_REGISTRY_CONF_NB_COL]);
     strcpy(tmp,path);
     strncat(tmp,"_REG_SOFTWARE.csv",MAX_PATH);
     ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_LOGICIEL, NB_COLONNE_LV[LV_REGISTRY_LOGICIEL_NB_COL]);
     strcpy(tmp,path);
     strncat(tmp,"_REG_UPDATE.csv",MAX_PATH);
     ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_MAJ, NB_COLONNE_LV[LV_REGISTRY_MAJ_NB_COL]);
     strcpy(tmp,path);
     strncat(tmp,"_REG_SERVICES_DRIVERS.csv",MAX_PATH);
     ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_SERVICES, NB_COLONNE_LV[LV_REGISTRY_SERVICES_NB_COL]);
     strcpy(tmp,path);
     strncat(tmp,"_REG_USERASSIST.csv",MAX_PATH);
     ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_HISTORIQUE, NB_COLONNE_LV[LV_REGISTRY_HISTORIQUE_NB_COL]);
     strcpy(tmp,path);
     strncat(tmp,"_REG_USB.csv",MAX_PATH);
     ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_USB, NB_COLONNE_LV[LV_REGISTRY_USB_NB_COL]);
     strcpy(tmp,path);
     strncat(tmp,"_REG_START.csv",MAX_PATH);
     ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_START, NB_COLONNE_LV[LV_REGISTRY_START_NB_COL]);
     strcpy(tmp,path);
     strncat(tmp,"_REG_NETWORK.csv",MAX_PATH);
     ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_LAN, NB_COLONNE_LV[LV_REGISTRY_LAN_NB_COL]);
     strcpy(tmp,path);
     strncat(tmp,"_REG_USERS.csv",MAX_PATH);
     ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_USERS, NB_COLONNE_LV[LV_REGISTRY_USERS_NB_COL]);
     strcpy(tmp,path);
     strncat(tmp,"_REG_PASSWORD.csv",MAX_PATH);
     ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_PASSWORD, NB_COLONNE_LV[LV_REGISTRY_PASSWORD_NB_COL]);
     strcpy(tmp,path);
     strncat(tmp,"_REG_MRU.csv",MAX_PATH);
     ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_MRU, NB_COLONNE_LV[LV_REGISTRY_MRU_NB_COL]);
     strcpy(tmp,path);
     strncat(tmp,"_REG_Path.csv",MAX_PATH);
     ExportLVtoCSV(tmp, TABL_REGISTRY, LV_REGISTRY_PATH, NB_COLONNE_LV[LV_REGISTRY_PATH_NB_COL]);
     strcpy(tmp,path);
     strncat(tmp,"_PROCESS.csv",MAX_PATH);
     ExportLVtoCSV(tmp, TABL_PROCESS, LV_VIEW, NB_COLONNE_LV[LV_PROCESS_VIEW_NB_COL]);
     strcpy(tmp,path);
     strncat(tmp,"_ALL_STATE.csv",MAX_PATH);
     ExportLVtoCSV(tmp, TABL_STATE, LV_VIEW, NB_COLONNE_LV[LV_STATE_VIEW_NB_COL]);
     strcpy(tmp,path);
     strncat(tmp,"_DAY_STATE.csv",MAX_PATH);
     ExportLVtoCSV(tmp, TABL_STATE, LV_VIEW, NB_COLONNE_LV[LV_STATE_H_VIEW_NB_COL]);
     }
   }

  //fin
  ExportStart = FALSE;
  SetWindowText(GetDlgItem(Tabl[TABL_CONF],BT_CONF_EXPORT),"Export");
  return 0;
}
