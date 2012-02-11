//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
typedef struct
{
  char token[MAX_PATH];
  char sha256[65];
  int exist;                  //existe ou pas dans la base
  char last_analysis_date[20];
  char detection_ratio[20];
}VIRUSTOTAL_STR;
//------------------------------------------------------------------------------
void GetCSRFToken(VIRUSTOTAL_STR *vts)
{
    //init connexion
    HINTERNET M_connexion = InternetOpen("",INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (M_connexion==NULL)return;

    HINTERNET M_session = InternetConnect(M_connexion, "www.virustotal.com",443,"","",INTERNET_SERVICE_HTTP,0,0);
    if (M_session==NULL)
    {
      InternetCloseHandle(M_connexion);
      return;
    }

    //connexion
    HINTERNET M_requete = HttpOpenRequest(M_session,"GET","www.virustotal.com",NULL,"",NULL,
                                          INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_SECURE
                                          |INTERNET_FLAG_IGNORE_CERT_CN_INVALID|INTERNET_FLAG_IGNORE_CERT_DATE_INVALID,0);
    if (M_requete==NULL)
    {
      InternetCloseHandle(M_session);
      InternetCloseHandle(M_connexion);
      return;
    }else if (HttpSendRequest(M_requete, NULL, 0, NULL, 0))
    {
      //traitement !!!
      char buffer[MAX_PATH];
      DWORD dwNumberOfBytesRead = MAX_PATH;

      if(HttpQueryInfo(M_requete,HTTP_QUERY_SET_COOKIE, buffer, &dwNumberOfBytesRead, 0))
      {
        if (dwNumberOfBytesRead>42)buffer[42]=0;

        //on passe : csrftoken=
        strcpy(vts->token,buffer+10);
      }
      InternetCloseHandle(M_requete);
    }
    //close
    InternetCloseHandle(M_session);
    InternetCloseHandle(M_connexion);
}
//------------------------------------------------------------------------------
void GetSHA256Info(VIRUSTOTAL_STR *vts)
{
    //init connexion
    HINTERNET M_connexion = InternetOpen("",INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (M_connexion==NULL)return;

    HINTERNET M_session = InternetConnect(M_connexion, "www.virustotal.com",443,"","",INTERNET_SERVICE_HTTP,0,0);
    if (M_session==NULL)
    {
      InternetCloseHandle(M_connexion);
      return;
    }
    //https://www.virustotal.com/file/upload/?sha256=0a864a03424ac6c2e43fd2dd15a9d57c73dbd5e49e4e2ac40f02931bf7213100
    char request[MAX_PATH] = "/file/upload/?sha256=";
    strncat(request,vts->sha256,MAX_PATH);
    strncat(request,"\0",MAX_PATH);

    //connexion
    HINTERNET M_requete = HttpOpenRequest(M_session,"GET",request,NULL,"https://www.virustotal.com/",NULL,
                                          INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_SECURE
                                          |INTERNET_FLAG_IGNORE_CERT_CN_INVALID|INTERNET_FLAG_IGNORE_CERT_DATE_INVALID,0);
    //Création du paramètre
    char cookie[MAX_PATH]="Cookie: csrftoken=";
    strncat(cookie,vts->token,MAX_PATH);
    strncat(cookie,"\0",MAX_PATH);

    if (M_requete==NULL)
    {
      InternetCloseHandle(M_session);
      InternetCloseHandle(M_connexion);
      return;
    }else if (HttpSendRequest(M_requete, cookie, strlen(cookie), NULL, NULL))
    {
      INTERNET_BUFFERS ib;
      ib.dwStructSize       = sizeof(INTERNET_BUFFERS);
      ib.lpcszHeader        = NULL;
      ib.dwHeadersLength    = NULL;
      ib.dwHeadersTotal     = NULL;
      ib.dwOffsetLow        = NULL;
      ib.dwOffsetHigh       = NULL;

      char resultat[16000];
      ib.lpvBuffer = resultat;
      ib.dwBufferLength = 16000-1;
      ib.dwBufferTotal = 16000-1;

      if(InternetReadFileEx(M_requete,&ib,IRF_NO_WAIT,NULL))
      {
        if (strlen(resultat)>16)
        {
          if (resultat[16] == 't') //dans la base
          {
            vts->exist = TRUE;

            //lecture + convertion : last_analysis_date
            char *c = resultat;
            while (*c && (*c != 'l' || *(c+1)!='a'))c++;

            if (*c == 'l')
            {
              c+=22;
              strncpy(vts->last_analysis_date,c,19);

              vts->last_analysis_date[4]='/';
              vts->last_analysis_date[7]='/';
              vts->last_analysis_date[10]='-';
              vts->last_analysis_date[19]=0;
            }

            //lecture : detection_ratio
            c = resultat;
            while (*c && (*c != '['))c++;
            if (*c == '[')
            {
              c++;
              strncpy(vts->detection_ratio,c,19);

              //recherche de la fin
              c = vts->detection_ratio;
              while (*c && *c!=']')c++;
              *c=0;
            }
          }else vts->exist = FALSE;
        }
      }
      InternetCloseHandle(M_requete);
    }

    //close
    InternetCloseHandle(M_session);
    InternetCloseHandle(M_connexion);
}
//------------------------------------------------------------------------------
void CheckItemToVirusTotal(HANDLE hlv, DWORD item, unsigned int column_sha256, unsigned int colum_sav, char *token)
{
  //init
  VIRUSTOTAL_STR vts;
  vts.token[0]              = 0;
  vts.exist                 = -1;
  vts.sha256[0]             = 0;
  vts.last_analysis_date[0] = 0;
  vts.detection_ratio[0]    = 0;
  ListView_GetItemText(hlv,item,column_sha256,vts.sha256,65);
  if (vts.sha256[0] == 0)return;

  //lecture du token
  if (token == NULL)GetCSRFToken(&vts);
  else strncpy(vts.token,token,MAX_PATH);

  //test du hash
  GetSHA256Info(&vts);

  //résultats
  char resultats[MAX_LINE_SIZE];
  switch(vts.exist)
  {
    case -1:strcpy(resultats,"Connection error");break;
    case 0:strcpy(resultats,"Unknow");break;
    case 1:snprintf(resultats,MAX_LINE_SIZE,"Ratio : %s (Last analysis : %s) Url : https://www.virustotal.com/file/%s/analysis/",vts.detection_ratio,vts.last_analysis_date,vts.sha256);break;
  }
  ListView_SetItemText(hlv,item,colum_sav,resultats);//owner
}
//------------------------------------------------------------------------------
typedef struct
{
  DWORD id;
  HANDLE hlv;
  char token[MAX_PATH];
}ST_VIRUSTOTAL;
HANDLE hSemaphore, hSemaphoreItem;

//------------------------------------------------------------------------------
DWORD WINAPI TCheckFileToVirusTotal(LPVOID lParam)
{
  ST_VIRUSTOTAL *s = (ST_VIRUSTOTAL *)lParam;
  DWORD item = s->id;
  HANDLE hlv = s->hlv;
  char token[MAX_PATH];
  strncpy(token,s->token,MAX_PATH);
  ReleaseSemaphore(hSemaphoreItem,1,NULL);
  CheckItemToVirusTotal(hlv, item, 13, 16,token);
  ReleaseSemaphore(hSemaphore,1,NULL);
  return 0;
}
//------------------------------------------------------------------------------
DWORD WINAPI CheckAllFileToVirusTotal(LPVOID lParam)
{
  DWORD i;
  ST_VIRUSTOTAL sv;
  sv.hlv = GetDlgItem(Tabl[TABL_FILES],LV_FILES_VIEW);

  //sémaphore pour gérer la limitte à 10 thread
  hSemaphore=CreateSemaphore(NULL,10,10,NULL);
  hSemaphoreItem=CreateSemaphore(NULL,1,1,NULL);

  //lecture du token
  VIRUSTOTAL_STR vts;
  vts.token[0] = 0;
  GetCSRFToken(&vts);

  //la gestion du nombre en lecture continue permet d'effetuer un scan pendant l'énumération
  for (i=0;i<SendDlgItemMessage(Tabl[TABL_FILES],LV_FILES_VIEW,LVM_GETITEMCOUNT,(WPARAM)0,(LPARAM)0);i++)
  {
    WaitForSingleObject(hSemaphore,INFINITE);
    WaitForSingleObject(hSemaphoreItem,INFINITE);
    sv.id = i;
    CreateThread(NULL,0,TCheckFileToVirusTotal,&sv,0,0);
  }

  unsigned int a;
  for (a=0;a<10;a++)WaitForSingleObject(hSemaphore,INFINITE);
  CloseHandle(hSemaphore);
  CloseHandle(hSemaphoreItem);

  AVIRUSTTAL = FALSE;
  return 0;
}
//------------------------------------------------------------------------------
DWORD WINAPI CheckSelectedItemToVirusTotal(LPVOID lParam)
{
  HANDLE hlv = GetDlgItem(Tabl[TABL_FILES],LV_FILES_VIEW);
  CheckItemToVirusTotal(hlv, SendMessage(hlv,LVM_GETNEXTITEM,-1,LVNI_FOCUSED), 13, 16, NULL);
  VIRUSTTAL = FALSE;
  return 0;
}
