//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void StateHC(LINE_ITEM *item, int col_date, char *user)
{
  if (col_date<0 || !State_Enable)return;
  if (!strlen(item[col_date].c) || item[col_date].c[0]=='N')return;

  //correlation pour ajout dans l'onglet state
  #define DATE_SIZE 20
  #define COL_START 0
  #define COL_END   1
  //si une date est présente
  //si aucun item dans la liste on en ajoute
  HANDLE hlv_r = GetDlgItem(Tabl[TABL_STATE],LV_VIEW_H);
  DWORD nb_items = ListView_GetItemCount(hlv_r);
  if (nb_items == 0)
  {
    //ajout de l'item
    LVITEM lvi;
    lvi.mask = LVIF_TEXT|LVIF_PARAM;
    lvi.iSubItem = 0;
    lvi.lParam = LVM_SORTITEMS;
    lvi.pszText="";
    lvi.iItem = 0;
    ListView_InsertItem(hlv_r, &lvi);

    ListView_SetItemText(hlv_r,0,COL_START,item[col_date].c);
    ListView_SetItemText(hlv_r,0,COL_END,item[col_date].c);
    ListView_SetItemText(hlv_r,0,2,"00:00:00");         //durée

    ListView_SetItemText(hlv_r,0,3,user);
  }else
  {
    DWORD i, duree;
    BOOL trouve = FALSE;
    char date_start[DATE_SIZE],date_end[DATE_SIZE],tmp_date[DATE_SIZE];
    for (i=0;i<nb_items && !trouve;i++)
    {
      //On test l'ensemble des items présents afin de vérifier
      //si il n'est pas plus récent ou plus ancien
      //même jour ?
      date_start[0]=0;
      ListView_GetItemText(hlv_r,i,COL_START,date_start,DATE_SIZE);
      if (date_start[0]!=0)
      {
        //année + séparateur + mois + jours
        if (date_start[0] == item[col_date].c[0] && date_start[1] == item[col_date].c[1] && date_start[2] == item[col_date].c[2] && date_start[3] == item[col_date].c[3] &&
            date_start[4] == item[col_date].c[4] && date_start[7] == item[col_date].c[7] && //les séparateurs ^^
            date_start[5] == item[col_date].c[5] && date_start[6] == item[col_date].c[6] && date_start[8] == item[col_date].c[8] && date_start[9] == item[col_date].c[9])
        {
          date_end[0]=0;
          ListView_GetItemText(hlv_r,i,COL_END,date_end,DATE_SIZE);

          //même jour
          trouve = TRUE;

          //on vérifie si inférieur au start
          if ((date_start[11] > item[col_date].c[11]) || (date_start[12] > item[col_date].c[12] && date_start[11] == item[col_date].c[11]) ||
              (date_start[11] == item[col_date].c[11] && date_start[12] == item[col_date].c[12] && date_start[14] > item[col_date].c[14])  ||
              (date_start[11] == item[col_date].c[11] && date_start[12] == item[col_date].c[12] && date_start[14] == item[col_date].c[14] && date_start[15] > item[col_date].c[15]) ||
              (date_start[11] == item[col_date].c[11] && date_start[12] == item[col_date].c[12] && date_start[14] == item[col_date].c[14] && date_start[15] == item[col_date].c[15] && date_start[17] > item[col_date].c[17]) ||
              (date_start[11] == item[col_date].c[11] && date_start[12] == item[col_date].c[12] && date_start[14] == item[col_date].c[14] && date_start[15] == item[col_date].c[15] && date_start[17] == item[col_date].c[17] && date_start[18] > item[col_date].c[18]))
          {
            ListView_SetItemText(hlv_r,i,COL_START,item[col_date].c);
            strcpy(date_start,item[col_date].c);

            ListView_SetItemText(hlv_r,i,3,user);

            //calcul de  la durée
            if (date_start[0]!=0 && date_end[0]!=0)
            {
                  duree = (CharToInt(date_end[11])*36000   + CharToInt(date_end[12])*3600   + CharToInt(date_end[14])*600    + CharToInt(date_end[15])*60   + CharToInt(date_end[17])*10   + CharToInt(date_end[18]))
                        - (CharToInt(date_start[11])*36000 + CharToInt(date_start[12])*3600 + CharToInt(date_start[14])*600  + CharToInt(date_start[15])*60 + CharToInt(date_start[17])*10 + CharToInt(date_start[18]));

              snprintf(tmp_date,DATE_SIZE,"%02lu:%02lu:%02lu",duree/3600,(duree%3600)/60,(duree%3600)%60);
              ListView_SetItemText(hlv_r,i,2,tmp_date);
            }
          }else //ou supérieur au end ^^
          {
            if (date_end[0]!=0)
            {
              //on vérifie si supérieur à end
              if ((date_end[11] < item[col_date].c[11]) || (date_end[12] < item[col_date].c[12] && date_end[11] == item[col_date].c[11]) ||
                  (date_end[11] == item[col_date].c[11] && date_end[12] == item[col_date].c[12] && date_end[14] < item[col_date].c[14])  ||
                  (date_end[11] == item[col_date].c[11] && date_end[12] == item[col_date].c[12] && date_end[14] == item[col_date].c[14] && date_end[15] < item[col_date].c[15]) ||
                  (date_end[11] == item[col_date].c[11] && date_end[12] == item[col_date].c[12] && date_end[14] == item[col_date].c[14] && date_end[15] == item[col_date].c[15] && date_end[17] < item[col_date].c[17]) ||
                  (date_end[11] == item[col_date].c[11] && date_end[12] == item[col_date].c[12] && date_end[14] == item[col_date].c[14] && date_end[15] == item[col_date].c[15] && date_end[17] == item[col_date].c[17] && date_end[18] < item[col_date].c[18]))
              {
                ListView_SetItemText(hlv_r,i,COL_END,item[col_date].c);
                strcpy(date_end,item[col_date].c);

                ListView_SetItemText(hlv_r,i,3,user);

                //calcul de  la durée
                if (date_start[0]!=0 && date_end[0]!=0)
                {
                  duree = (CharToInt(date_end[11])*36000   + CharToInt(date_end[12])*3600   + CharToInt(date_end[14])*600    + CharToInt(date_end[15])*60   + CharToInt(date_end[17])*10   + CharToInt(date_end[18]))
                        - (CharToInt(date_start[11])*36000 + CharToInt(date_start[12])*3600 + CharToInt(date_start[14])*600  + CharToInt(date_start[15])*60 + CharToInt(date_start[17])*10 + CharToInt(date_start[18]));

                  snprintf(tmp_date,DATE_SIZE,"%02lu:%02lu:%02lu",duree/3600,(duree%3600)/60,(duree%3600)%60);
                  ListView_SetItemText(hlv_r,i,2,tmp_date);
                }
              }
            }
          }
        }
      }
    }
    //si pas identifié on ajoute l'item
    if (!trouve)
    {
      //ajout de l'item
      LVITEM lvi;
      lvi.mask = LVIF_TEXT|LVIF_PARAM;
      lvi.iSubItem = 0;
      lvi.lParam = LVM_SORTITEMS;
      lvi.pszText="";
      lvi.iItem = ListView_GetItemCount(hlv_r);
      i = ListView_InsertItem(hlv_r, &lvi);

      ListView_SetItemText(hlv_r,i,COL_START,item[col_date].c);
      ListView_SetItemText(hlv_r,i,COL_END,item[col_date].c);

      ListView_SetItemText(hlv_r,i,2,"00:00:00");         //durée

      ListView_SetItemText(hlv_r,i,3,user);
    }
  }
}
//------------------------------------------------------------------------------
void StateH(LINE_ITEM *item, int col_date, int col_id)
{
  if (col_date<0 || !State_Enable)return;
  if (!strlen(item[col_date].c) || item[col_date].c[0]=='N')return;

  //correlation pour ajout dans l'onglet state
  #define DATE_SIZE 20
  #define COL_START 0
  #define COL_END   1
  //si une date est présente
  //si aucun item dans la liste on en ajoute
  HANDLE hlv_r = GetDlgItem(Tabl[TABL_STATE],LV_VIEW_H);
  DWORD nb_items = ListView_GetItemCount(hlv_r);
  if (nb_items == 0)
  {
    //ajout de l'item
    LVITEM lvi;
    lvi.mask = LVIF_TEXT|LVIF_PARAM;
    lvi.iSubItem = 0;
    lvi.lParam = LVM_SORTITEMS;
    lvi.pszText="";
    lvi.iItem = 0;
    ListView_InsertItem(hlv_r, &lvi);

    ListView_SetItemText(hlv_r,0,COL_START,item[col_date].c);
    ListView_SetItemText(hlv_r,0,COL_END,item[col_date].c);
    ListView_SetItemText(hlv_r,0,2,"00:00:00");         //durée

    if(col_id>-1)ListView_SetItemText(hlv_r,0,3,item[col_id].c);
  }else
  {
    DWORD i, duree;
    BOOL trouve = FALSE;
    char date_start[DATE_SIZE],date_end[DATE_SIZE],tmp_date[DATE_SIZE];
    for (i=0;i<nb_items && !trouve;i++)
    {
      //On test l'ensemble des items présents afin de vérifier
      //si il n'est pas plus récent ou plus ancien
      //même jour ?
      date_start[0]=0;
      ListView_GetItemText(hlv_r,i,COL_START,date_start,DATE_SIZE);
      if (date_start[0]!=0)
      {
        //année + séparateur + mois + jours
        if (date_start[0] == item[col_date].c[0] && date_start[1] == item[col_date].c[1] && date_start[2] == item[col_date].c[2] && date_start[3] == item[col_date].c[3] &&
            date_start[4] == item[col_date].c[4] && date_start[7] == item[col_date].c[7] && //les séparateurs ^^
            date_start[5] == item[col_date].c[5] && date_start[6] == item[col_date].c[6] && date_start[8] == item[col_date].c[8] && date_start[9] == item[col_date].c[9])
        {
          date_end[0]=0;
          ListView_GetItemText(hlv_r,i,COL_END,date_end,DATE_SIZE);

          //même jour
          trouve = TRUE;

          //on vérifie si inférieur au start
          if ((date_start[11] > item[col_date].c[11]) || (date_start[12] > item[col_date].c[12] && date_start[11] == item[col_date].c[11]) ||
              (date_start[11] == item[col_date].c[11] && date_start[12] == item[col_date].c[12] && date_start[14] > item[col_date].c[14])  ||
              (date_start[11] == item[col_date].c[11] && date_start[12] == item[col_date].c[12] && date_start[14] == item[col_date].c[14] && date_start[15] > item[col_date].c[15]) ||
              (date_start[11] == item[col_date].c[11] && date_start[12] == item[col_date].c[12] && date_start[14] == item[col_date].c[14] && date_start[15] == item[col_date].c[15] && date_start[17] > item[col_date].c[17]) ||
              (date_start[11] == item[col_date].c[11] && date_start[12] == item[col_date].c[12] && date_start[14] == item[col_date].c[14] && date_start[15] == item[col_date].c[15] && date_start[17] == item[col_date].c[17] && date_start[18] > item[col_date].c[18]))
          {
            ListView_SetItemText(hlv_r,i,COL_START,item[col_date].c);
            strcpy(date_start,item[col_date].c);

            if(col_id>-1)ListView_SetItemText(hlv_r,i,3,item[col_id].c);

            //calcul de  la durée
            if (date_start[0]!=0 && date_end[0]!=0)
            {
                  duree = (CharToInt(date_end[11])*36000   + CharToInt(date_end[12])*3600   + CharToInt(date_end[14])*600    + CharToInt(date_end[15])*60   + CharToInt(date_end[17])*10   + CharToInt(date_end[18]))
                        - (CharToInt(date_start[11])*36000 + CharToInt(date_start[12])*3600 + CharToInt(date_start[14])*600  + CharToInt(date_start[15])*60 + CharToInt(date_start[17])*10 + CharToInt(date_start[18]));

              snprintf(tmp_date,DATE_SIZE,"%02lu:%02lu:%02lu",duree/3600,(duree%3600)/60,(duree%3600)%60);
              ListView_SetItemText(hlv_r,i,2,tmp_date);
            }
          }else //ou supérieur au end ^^
          {
            if (date_end[0]!=0)
            {
              //on vérifie si supérieur à end
              if ((date_end[11] < item[col_date].c[11]) || (date_end[12] < item[col_date].c[12] && date_end[11] == item[col_date].c[11]) ||
                  (date_end[11] == item[col_date].c[11] && date_end[12] == item[col_date].c[12] && date_end[14] < item[col_date].c[14])  ||
                  (date_end[11] == item[col_date].c[11] && date_end[12] == item[col_date].c[12] && date_end[14] == item[col_date].c[14] && date_end[15] < item[col_date].c[15]) ||
                  (date_end[11] == item[col_date].c[11] && date_end[12] == item[col_date].c[12] && date_end[14] == item[col_date].c[14] && date_end[15] == item[col_date].c[15] && date_end[17] < item[col_date].c[17]) ||
                  (date_end[11] == item[col_date].c[11] && date_end[12] == item[col_date].c[12] && date_end[14] == item[col_date].c[14] && date_end[15] == item[col_date].c[15] && date_end[17] == item[col_date].c[17] && date_end[18] < item[col_date].c[18]))
              {
                ListView_SetItemText(hlv_r,i,COL_END,item[col_date].c);
                strcpy(date_end,item[col_date].c);

                if(col_id>-1)ListView_SetItemText(hlv_r,i,3,item[col_id].c);

                //calcul de  la durée
                if (date_start[0]!=0 && date_end[0]!=0)
                {
                  duree = (CharToInt(date_end[11])*36000   + CharToInt(date_end[12])*3600   + CharToInt(date_end[14])*600    + CharToInt(date_end[15])*60   + CharToInt(date_end[17])*10   + CharToInt(date_end[18]))
                        - (CharToInt(date_start[11])*36000 + CharToInt(date_start[12])*3600 + CharToInt(date_start[14])*600  + CharToInt(date_start[15])*60 + CharToInt(date_start[17])*10 + CharToInt(date_start[18]));

                  snprintf(tmp_date,DATE_SIZE,"%02lu:%02lu:%02lu",duree/3600,(duree%3600)/60,(duree%3600)%60);
                  ListView_SetItemText(hlv_r,i,2,tmp_date);
                }
              }
            }
          }
        }
      }
    }
    //si pas identifié on ajoute l'item
    if (!trouve)
    {
      //ajout de l'item
      LVITEM lvi;
      lvi.mask = LVIF_TEXT|LVIF_PARAM;
      lvi.iSubItem = 0;
      lvi.lParam = LVM_SORTITEMS;
      lvi.pszText="";
      lvi.iItem = ListView_GetItemCount(hlv_r);
      i = ListView_InsertItem(hlv_r, &lvi);

      ListView_SetItemText(hlv_r,i,COL_START,item[col_date].c);
      ListView_SetItemText(hlv_r,i,COL_END,item[col_date].c);

      ListView_SetItemText(hlv_r,i,2,"00:00:00");         //durée

      if(col_id>-1)ListView_SetItemText(hlv_r,i,3,item[col_id].c);
    }
  }
}
