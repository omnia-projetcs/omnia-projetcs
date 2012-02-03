//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
#include "sqlite3.h"
//------------------------------------------------------------------------------
//type de fichier SQLite
#define SQLITE_TYPE_COOKIES                             0x00000000
#define SQLITE_TYPE_DOWNLOAD                            0x00000001
#define SQLITE_TYPE_FORMHISTORY                         0x00000002
#define SQLITE_TYPE_PLACES                              0x00000004
#define SQLITE_TYPE_ADDONS                              0x00000008
#define SQLITE_TYPE_PREFS                               0x00000010
#define SQLITE_TYPE_EXTENSION                           0x00000020
#define SQLITE_TYPE_PASSWORD                            0x00000040

//chrome
#define SQLITE_TYPE_CH_FORM_KEY                         0x00000080  //table keywords, keyword,url,date_created,last_modified  FILE :Web Data
#define SQLITE_TYPE_CH_URL_HISTORY                      0x00000100  //table urls, url,title,visit_count,last_vist_time, FILE :Archived History, History*
#define SQLITE_TYPE_CH_DOWNLOAD                         0x00000200  //table downloads, url,full_path,start_time, FILE :Archived History, History*
#define SQLITE_TYPE_CH_COOKIES                          0x00000400  //table cookies, name,value,host_key,path,creation_utc,last_access_utc, FILE:Cookies
#define SQLITE_TYPE_CH_FORM_WD                          0x00000800  //table autofill, name,value,pair_id + table autofill_dates, pair_id,date_created  FILE :Web Data
//manque fichier Preferences : en XML
//manque fichier Bookmarks : en XML

//Android
#define SQLITE_TYPE_ANDROID_CONF                        0x00001000
#define SQLITE_TYPE_ANDROID_CONTACT2                    0x00002000
#define SQLITE_TYPE_ANDROID_ACCOUNTS                    0x00004000
#define SQLITE_TYPE_ANDROID_CALLENDAR                   0x00008000
#define SQLITE_TYPE_ANDROID_SMS                         0x00010000
#define SQLITE_TYPE_ANDROID_MMS                         0x00010001
#define SQLITE_TYPE_ANDROID_OP                          0x00020000
#define SQLITE_TYPE_ANDROID_GPS_POS                     0x00040000
#define SQLITE_TYPE_ANDROID_MEMO                        0x00080000
#define SQLITE_TYPE_ANDROID_DOC                         0x00200000
#define SQLITE_TYPE_ANDROID_SEARCH                      0x00200000
#define SQLITE_TYPE_ANDROID_HISTORY                     0x00400000
#define SQLITE_TYPE_ANDROID_APP_FAVORIS                 0x00800000
#define SQLITE_TYPE_ANDROID_APP_CACHE                   0x00800001
#define SQLITE_TYPE_ANDROID_APP_CACHE_URL               0x00800002

#define SQLITE_TYPE_ANDROID_APP_MAP                     0x00800008
#define SQLITE_TYPE_ANDROID_APP_MAIL                    0x00800010
#define SQLITE_TYPE_ANDROID_APP_RSS                     0x00800011
#define SQLITE_TYPE_ANDROID_APP_BROWSER                 0x01000000
#define SQLITE_TYPE_ANDROID_APP_BROWSER_SEARCH          0x01000001
#define SQLITE_TYPE_ANDROID_APP_BROWSER_FORM            0x01000002
#define SQLITE_TYPE_ANDROID_APP_BROWSER_FORM_AUTH       0x01000004
#define SQLITE_TYPE_ANDROID_APP_BROWSER_CACHE           0x01000008
#define SQLITE_TYPE_ANDROID_APP_EVERNOTE                0x02000000
#define SQLITE_TYPE_ANDROID_APP_LINKEDIN                0x04000000
#define SQLITE_TYPE_ANDROID_APP_TWEET                   0x08000000
#define SQLITE_TYPE_ANDROID_APP_TWEET_CONTACT           0x08000001
#define SQLITE_TYPE_ANDROID_APP_TWEET_SEARCH            0x08000002
#define SQLITE_TYPE_ANDROID_APP_TWEET_MP                0x08000004
#define SQLITE_TYPE_ANDROID_APP_MEDIA                   0x10000000
#define SQLITE_TYPE_ANDROID_APP_MEDIA_VIDEO             0x10000001
#define SQLITE_TYPE_ANDROID_APP_MEDIA_PHOTO             0x10000002

#define SQLITE_TYPE_ALL                                 0xFFFFFFFF
typedef struct
{
  DWORD type;
  char path[MAX_PATH];
  HANDLE hlv;
}FORMAT_CALBAK_INFO;

//------------------------------------------------------------------------------
void convertFirefoxDatetoStr(char *fdate, char*resultat, unsigned int size_max)
{
  //cas des valeures nulles
  resultat[0]=0;
  if (fdate == NULL)return;
  if (fdate[0]<48 || fdate[0]>57 || fdate[1]==0)return;
  if (!strcmp(fdate,"1970/01/01-01:00:00"))return;

  snprintf(resultat,size_max,"%s",fdate);
}
//------------------------------------------------------------------------------
//affichage d'une ligne de la bdd
static int callback_sqlite(void *param, int argc, char **argv, char **azColName)
{
  LINE_ITEM lv_line[SIZE_UTIL_ITEM];
  strcpy(lv_line[0].c,((FORMAT_CALBAK_INFO*)param)->path);
  DWORD type        = ((FORMAT_CALBAK_INFO*)param)->type;
  HANDLE hlv        = ((FORMAT_CALBAK_INFO*)param)->hlv;

  strncpy(lv_line[3].c,"Firefox",MAX_LINE_SIZE);
  strncpy(lv_line[4].c,"History",MAX_LINE_SIZE);
  lv_line[5].c[0]=0;
  lv_line[6].c[0]=0;
  lv_line[7].c[0]=0;

  if (type == SQLITE_TYPE_COOKIES && argc>=6)
  {
    strncpy(lv_line[5].c,"Cookies",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"name,value,host,path,creationTime,lastAccessed",MAX_LINE_SIZE);

    //traitement de la date : création
    convertFirefoxDatetoStr(argv[4], lv_line[6].c, MAX_LINE_SIZE);
    //traitement de la date : last update
    convertFirefoxDatetoStr(argv[5], lv_line[7].c, MAX_LINE_SIZE);

    //datas
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s (%s) %s",argv[0],argv[1],argv[2],argv[3]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_DOWNLOAD && argc>=5)
  {
    strncpy(lv_line[5].c,"Download",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"name,source,target,startTime,endTime",MAX_LINE_SIZE);

    //traitement de la date : startTime
    convertFirefoxDatetoStr(argv[3], lv_line[6].c, MAX_LINE_SIZE);
    //traitement de la date : endTime
    convertFirefoxDatetoStr(argv[4], lv_line[7].c, MAX_LINE_SIZE);

    //datas
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s (%s)",argv[0],argv[1],argv[2]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);

  }else if (type == SQLITE_TYPE_FORMHISTORY && argc>=5)
  {
    strncpy(lv_line[5].c,"Form history",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"fieldname,value,timesUsed,firstUsed,lastUsed",MAX_LINE_SIZE);

    //traitement de la date : firstUsed
    convertFirefoxDatetoStr(argv[3], lv_line[6].c, MAX_LINE_SIZE);
    //traitement de la date : lastUsed
    convertFirefoxDatetoStr(argv[4], lv_line[7].c, MAX_LINE_SIZE);

    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s (use;%s)",argv[0],argv[1],argv[2]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_PLACES && argc>=4)
  {
    strncpy(lv_line[5].c,"URL history",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"url, title, frecency, last_visit_date",MAX_LINE_SIZE);

    //traitement de la date : last_visit_date
    convertFirefoxDatetoStr(argv[3], lv_line[7].c, MAX_LINE_SIZE);

    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s (use:%s)",argv[0],argv[1],argv[2]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ADDONS && argc>=5)
  {
    strncpy(lv_line[4].c,"Settings",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"List of addons",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"name,version,homepageURL,creator,description",MAX_LINE_SIZE);

    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s %s:%s (%s) %s",argv[0],argv[1],argv[2],argv[3],argv[4]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_EXTENSION && argc>=5)
  {
    strncpy(lv_line[4].c,"Settings",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"List of extensions",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"name,homepageURL,creator,installDate,updateDate",MAX_LINE_SIZE);

    //traitement de la date : installDate
    convertFirefoxDatetoStr(argv[3], lv_line[6].c, MAX_LINE_SIZE);
    //traitement de la date : updateDate
    convertFirefoxDatetoStr(argv[4], lv_line[7].c, MAX_LINE_SIZE);

    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s (%s)",argv[0],argv[1],argv[2]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_PREFS && argc>=2)
  {
    strncpy(lv_line[4].c,"Settings",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"name,value",MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s=%s",argv[0],argv[1]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_PASSWORD)
  {
    strncpy(lv_line[5].c,"Accounts and passwords history",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"hostname,formSubmitURL",MAX_LINE_SIZE);
    //hostname,formSubmitURL,usernameField,passwordField,encryptedUsername,encryptedPassword,guid,encType

    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s",argv[0],argv[1]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
//Chrome
  }else if (type == SQLITE_TYPE_CH_URL_HISTORY && argc>=4)
  {
    strncpy(lv_line[3].c,"Chrome",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"URL history",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"url,title,visit_count,last_vist_time",MAX_LINE_SIZE);

    //traitement de la date : last_visit_date
    convertFirefoxDatetoStr(argv[3], lv_line[7].c, MAX_LINE_SIZE);

    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s (use:%s)",argv[0],argv[1],argv[2]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_CH_DOWNLOAD && argc>=3)
  {
    strncpy(lv_line[3].c,"Chrome",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Download",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"url,full_path,start_time",MAX_LINE_SIZE);

    //traitement de la date : startTime
    convertFirefoxDatetoStr(argv[2], lv_line[6].c, MAX_LINE_SIZE);

    //datas
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s",argv[0],argv[1]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_CH_COOKIES && argc>=6)
  {
    strncpy(lv_line[3].c,"Chrome",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Cookies",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"name,value,host_key,path,creation_utc,last_access_utc",MAX_LINE_SIZE);

    //traitement de la date : création
    convertFirefoxDatetoStr(argv[4], lv_line[6].c, MAX_LINE_SIZE);
    //traitement de la date : last update
    convertFirefoxDatetoStr(argv[5], lv_line[7].c, MAX_LINE_SIZE);

    //datas
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s (%s) %s",argv[0],argv[1],argv[2],argv[3]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_CH_FORM_KEY && argc>=4)
  {
    strncpy(lv_line[3].c,"Chrome",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Form history",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"keyword,url,date_created,last_modified",MAX_LINE_SIZE);

    //traitement de la date : firstUsed
    convertFirefoxDatetoStr(argv[2], lv_line[6].c, MAX_LINE_SIZE);
    //traitement de la date : lastUsed
    convertFirefoxDatetoStr(argv[3], lv_line[7].c, MAX_LINE_SIZE);

    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s",argv[0],argv[1]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_CH_FORM_WD && argc>=3)
  {
    strncpy(lv_line[3].c,"Chrome",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Form history",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"name,value,date_created",MAX_LINE_SIZE);

    //traitement de la date : date_created
    convertFirefoxDatetoStr(argv[2], lv_line[6].c, MAX_LINE_SIZE);

    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s",argv[0],argv[1]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_CONF && argc>=2)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[4].c,"Settings",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"name,value",MAX_LINE_SIZE);

    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s",argv[0],argv[1]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_OP && argc>=2)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[4].c,"Settings",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Selected telecom operator",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"full,plmn",MAX_LINE_SIZE);

    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s (%s)",argv[0],argv[1]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_APP_FAVORIS && argc>=1)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[4].c,"Settings",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Favorite applications",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"title",MAX_LINE_SIZE);

    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s",argv[0]);
    if (!strcmp(lv_line[2].c,"(null)"))return 0;

    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_ACCOUNTS && argc>=3)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Accounts",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"name,password,type",MAX_LINE_SIZE);

    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s (%s)",argv[0],argv[2],argv[1]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_APP_BROWSER_FORM && argc>=3)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Form history/Cookies",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"name,value,url/domain",MAX_LINE_SIZE);

    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s (%s)",argv[0],argv[1],argv[2]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_APP_BROWSER_FORM_AUTH && argc>=3)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Form authentication history",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"username,password,host",MAX_LINE_SIZE);

    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s (%s)",argv[1],argv[2],argv[0]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_APP_CACHE && argc>=3)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Applications Cache",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"url,responseURL,headers",MAX_LINE_SIZE);

    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s (%s)",argv[0],argv[1],argv[2]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_APP_CACHE_URL && argc>=2)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Applications Cache",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"title,url_img,lat,lng,ts",MAX_LINE_SIZE);
    convertFirefoxDatetoStr(argv[2], lv_line[7].c, MAX_LINE_SIZE);

    snprintf(lv_line[2].c,MAX_LINE_SIZE,"[%s:%s] %s (%s)",argv[3],argv[4],argv[1],argv[0]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_CONTACT2 && argc>=11)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    snprintf(lv_line[5].c,MAX_LINE_SIZE,"Contact (%s/%s)",argv[3],argv[2]);
    strncpy(lv_line[1].c,"account_name,normalized_number,account_type,last_time_contacted",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[4], lv_line[7].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s",argv[0],argv[1]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_CALLENDAR && argc>=8)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    snprintf(lv_line[5].c,MAX_LINE_SIZE,"Calendar list of events (%s)",argv[3]);
    strncpy(lv_line[1].c,"_sync_account,title,eventLocation,description,lastDate",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[1], lv_line[4].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s [%s] %s",argv[0],argv[1],argv[2],argv[3]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_SMS && argc>=5)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strcpy(lv_line[5].c,"SMS/MMS/Call");
    strncpy(lv_line[1].c,"number,name,type",MAX_LINE_SIZE);

    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s (%s)",argv[0],argv[1]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_MMS && argc>=5)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strcpy(lv_line[5].c,"SMS/MMS/Call");
    strncpy(lv_line[1].c,"address,body,date",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[4], lv_line[6].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s",argv[0],argv[1]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_GPS_POS && argc>=3)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    snprintf(lv_line[5].c,MAX_LINE_SIZE,"Cached Geo position");
    strncpy(lv_line[1].c,"latitude,longitude,timestamp",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[2], lv_line[6].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"[%s:%s]",argv[0],argv[1]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_APP_MAP && argc>=3)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    snprintf(lv_line[5].c,MAX_LINE_SIZE,"GoogleMap recent position");
    strncpy(lv_line[1].c,"name,address,lastUpdated",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[2], lv_line[6].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s",argv[0],argv[1]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_MEMO && argc>=3)
  {
    MessageBox(0,"","TEST!",MB_OK|MB_TOPMOST);

    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Memo",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"content,create_t,modify_t",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[2], lv_line[6].c, MAX_LINE_SIZE);
    convertFirefoxDatetoStr(argv[1], lv_line[7].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s",argv[0]);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
    ConvertWA(lv_line[2].c);
  }else if (type == SQLITE_TYPE_ANDROID_DOC && argc>=5)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Google Documents",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"title,creator,creationTime,lastModifiedTime",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[3], lv_line[6].c, MAX_LINE_SIZE);
    convertFirefoxDatetoStr(argv[4], lv_line[7].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s (%s)",argv[0],argv[1]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_SEARCH && argc>=5)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Document search history",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"name,query,latitude,longitude,location,time",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[5], lv_line[6].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s (%s:%s:%s)",argv[0],argv[1],argv[2],argv[3],argv[4]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_HISTORY && argc>=2)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Search history",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"query,date",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[1], lv_line[6].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s",argv[0]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_APP_BROWSER && argc>=4)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Internet history",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"title,url,date,created",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[3], lv_line[6].c, MAX_LINE_SIZE);
    convertFirefoxDatetoStr(argv[2], lv_line[7].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s",argv[0],argv[1]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_APP_BROWSER_SEARCH && argc>=2)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Internet search history",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"search,date",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[1], lv_line[6].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s",argv[0]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_APP_BROWSER_CACHE && argc>=2)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Internet cache history",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"url,lastmodify",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[1], lv_line[7].c, MAX_LINE_SIZE);
    //snprintf(lv_line[7].c,MAX_LINE_SIZE,"%s",argv[1]);

    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s",argv[0]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_APP_MEDIA && argc>=3)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"History of reading media",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"_data,date_added,date_modified",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[1], lv_line[6].c, MAX_LINE_SIZE);
    convertFirefoxDatetoStr(argv[2], lv_line[7].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s",argv[0]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);

  }else if (type == SQLITE_TYPE_ANDROID_APP_MEDIA_VIDEO && argc>=6)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Movie",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"_display_name,_data, date_added,datetaken,latitude,longitude",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[3], lv_line[6].c, MAX_LINE_SIZE);
    convertFirefoxDatetoStr(argv[2], lv_line[7].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"[%s:%s] %s (%s)",argv[4],argv[5],argv[0],argv[1]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_APP_MEDIA_PHOTO && argc>=6)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Image",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"_display_name,_data, date_added,datetaken,latitude,longitude",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[3], lv_line[6].c, MAX_LINE_SIZE);
    convertFirefoxDatetoStr(argv[2], lv_line[7].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"[%s:%s] %s (%s)",argv[4],argv[5],argv[0],argv[1]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_APP_MAIL && argc>=6)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Mails",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"subject,fromAddress,toAddresses,joinedAttachmentInfos,dateSentMs,dateReceivedMs",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[4], lv_line[6].c, MAX_LINE_SIZE);
    convertFirefoxDatetoStr(argv[5], lv_line[7].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s (%s->%s) %s",argv[0],argv[1],argv[2],argv[3]);

    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_APP_RSS && argc>=4)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"RSS",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"title,html_url,newest_item_time,sync_time",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[2], lv_line[6].c, MAX_LINE_SIZE);
    convertFirefoxDatetoStr(argv[3], lv_line[7].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s (%s)",argv[0],argv[1]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_APP_EVERNOTE && argc>=3)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Evernote",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"title,created,updated,latitude,longitude",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[1], lv_line[6].c, MAX_LINE_SIZE);
    convertFirefoxDatetoStr(argv[2], lv_line[7].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"[%s:%s] %s",argv[3],argv[4],argv[0]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_APP_LINKEDIN && argc>=5)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Linkedin",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"first_name,last_name,headline",MAX_LINE_SIZE);

    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s %s (%s)",argv[0],argv[1],argv[3]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_APP_TWEET && argc>=5)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Tweeter - tweet",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"name,text,source,entities,created_at",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[4], lv_line[6].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s (%s) %s",argv[1],argv[0],argv[2],argv[3]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_APP_TWEET_CONTACT && argc>=7)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Tweeter - contact",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"username,name,description,web_url,location,profile_created,updated",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[5], lv_line[6].c, MAX_LINE_SIZE);
    convertFirefoxDatetoStr(argv[6], lv_line[7].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s (%s) %s (%s) %s",argv[0],argv[1],argv[2],argv[3],argv[4]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_APP_TWEET_CONTACT && argc>=3)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Tweeter - search",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"name,query,time",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[2], lv_line[6].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s:%s",argv[0],argv[1]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }else if (type == SQLITE_TYPE_ANDROID_APP_TWEET_MP && argc>=4)
  {
    strncpy(lv_line[3].c,"Android",MAX_LINE_SIZE);
    strncpy(lv_line[5].c,"Tweeter - Private Message",MAX_LINE_SIZE);
    strncpy(lv_line[1].c,"content,username,name,created",MAX_LINE_SIZE);

    convertFirefoxDatetoStr(argv[1], lv_line[6].c, MAX_LINE_SIZE);
    snprintf(lv_line[2].c,MAX_LINE_SIZE,"%s (%s) %s",argv[2],argv[3],argv[0]);
    ConvertWA(lv_line[2].c);
    AddToLVConf(hlv, lv_line, NB_COLONNE_LV[LV_CONFIGURATION_NB_COL]);
  }

  return 0;
}
//------------------------------------------------------------------------------
void OpenSQLite(HANDLE hlv, char *file, DWORD type)
{
  sqlite3 *db;

  FORMAT_CALBAK_INFO fci;
  strncpy(fci.path,file,MAX_PATH);
  fci.hlv = hlv;

  //ouverture de la base sqlite en lecture seule
  //par défaut la fonction "sqlite3_open" modifie le fichier
  //d'ou lutilisation de "sqlite3_open_v2"
  if (sqlite3_open_v2(file, &db,SQLITE_OPEN_READONLY,NULL) == SQLITE_OK)
  {
//FIREFOX
    if (type & SQLITE_TYPE_COOKIES)
    {
      fci.type = SQLITE_TYPE_COOKIES;
      sqlite3_exec(db, "SELECT name,value,host,path,strftime('%Y/%m/%d-%H:%M:%S',(creationTime/1000000), 'unixepoch', 'localtime'),strftime('%Y/%m/%d-%H:%M:%S',(lastAccessed/1000000), 'unixepoch', 'localtime') FROM moz_cookies;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_DOWNLOAD)
    {
      fci.type = SQLITE_TYPE_DOWNLOAD;
      sqlite3_exec(db, "SELECT name,source,target,strftime('%Y/%m/%d-%H:%M:%S',(startTime/1000000), 'unixepoch', 'localtime'),strftime('%Y/%m/%d-%H:%M:%S',(endTime/1000000), 'unixepoch', 'localtime') FROM moz_downloads;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_FORMHISTORY)
    {
      fci.type = SQLITE_TYPE_FORMHISTORY;
      sqlite3_exec(db, "SELECT fieldname,value,timesUsed,strftime('%Y/%m/%d-%H:%M:%S',(firstUsed/1000000), 'unixepoch', 'localtime'),strftime('%Y/%m/%d-%H:%M:%S',(lastUsed/1000000), 'unixepoch', 'localtime') FROM moz_formhistory;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_PLACES)
    {
      fci.type = SQLITE_TYPE_PLACES;
      sqlite3_exec(db, "SELECT url,title,frecency,strftime('%Y/%m/%d-%H:%M:%S',(last_visit_date/1000000), 'unixepoch', 'localtime') FROM moz_places;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_ADDONS)
    {
      fci.type = SQLITE_TYPE_ADDONS;
      sqlite3_exec(db, "SELECT name,version,homepageURL,creator,description FROM addon;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_EXTENSION)
    {
      fci.type = SQLITE_TYPE_EXTENSION;
      sqlite3_exec(db, "SELECT DISTINCT name,homepageURL,creator,strftime('%Y/%m/%d-%H:%M:%S',(installDate/1000000), 'unixepoch', 'localtime'),strftime('%Y/%m/%d-%H:%M:%S',(updateDate/1000000), 'unixepoch', 'localtime') FROM locale;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_PREFS)
    {
      fci.type = SQLITE_TYPE_PREFS;
      sqlite3_exec(db, "SELECT DISTINCT settings.name, prefs.value FROM settings, prefs WHERE prefs.settingID=settings.id;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_PASSWORD)
    {
      fci.type = SQLITE_TYPE_PASSWORD;
      sqlite3_exec(db, "SELECT hostname,formSubmitURL,usernameField,passwordField,encryptedUsername,encryptedPassword,guid,encType FROM moz_logins;", callback_sqlite, &fci, NULL);
    }

//CHROME
    if (type & SQLITE_TYPE_CH_URL_HISTORY)
    {
      fci.type = SQLITE_TYPE_CH_URL_HISTORY;
      sqlite3_exec(db, "SELECT url,title,visit_count,strftime('%Y/%m/%d-%H:%M:%S',(last_vist_time/1000000)-11644473600, 'unixepoch', 'localtime') FROM urls;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_CH_DOWNLOAD)
    {
      fci.type = SQLITE_TYPE_CH_DOWNLOAD;
      sqlite3_exec(db, "SELECT url,full_path,strftime('%Y/%m/%d-%H:%M:%S',start_time, 'unixepoch', 'localtime') FROM downloads;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_CH_COOKIES)
    {
      fci.type = SQLITE_TYPE_CH_COOKIES;
      sqlite3_exec(db, "SELECT name,value,host_key,path,strftime('%Y/%m/%d-%H:%M:%S',(creation_utc/1000000)-11644473600, 'unixepoch', 'localtime'),strftime('%Y/%m/%d-%H:%M:%S',(last_access_utc/1000000)-11644473600, 'unixepoch', 'localtime') FROM cookies;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_CH_FORM_KEY)
    {
      fci.type = SQLITE_TYPE_CH_FORM_KEY;
      sqlite3_exec(db, "SELECT keyword,url,strftime('%Y/%m/%d-%H:%M:%S',date_created, 'unixepoch', 'localtime'),strftime('%Y/%m/%d-%H:%M:%S',last_modified, 'unixepoch', 'localtime') FROM keywords;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_CH_FORM_WD)
    {
      fci.type = SQLITE_TYPE_CH_FORM_WD;
      sqlite3_exec(db, "SELECT DISTINCT autofill.name,autofill.value,strftime('%Y/%m/%d-%H:%M:%S',autofill_dates.date_created, 'unixepoch', 'localtime') FROM autofill, autofill_dates WHERE autofill.pair_id=autofill_dates.pair_id;", callback_sqlite, &fci, NULL);
    }

//ANDROID
    if (type & SQLITE_TYPE_ANDROID_CONF)//settings.db
    {
      fci.type = SQLITE_TYPE_ANDROID_CONF;
      sqlite3_exec(db, "SELECT name,value  FROM main;", callback_sqlite, &fci, NULL);
      sqlite3_exec(db, "SELECT name,value  FROM secure;", callback_sqlite, &fci, NULL);
      sqlite3_exec(db, "SELECT name,value  FROM system;", callback_sqlite, &fci, NULL);
      sqlite3_exec(db, "SELECT key, value  FROM extras;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_ANDROID_CONTACT2)//contacts2.db
    {
      fci.type = SQLITE_TYPE_ANDROID_CONTACT2;
      sqlite3_exec(db, "SELECT DISTINCT display_name,normalized_number,account_name,account_type,sync3,last_time_contacted,data1,data2,data4,data7,data8,data9 FROM raw_contacts, phone_lookup,data WHERE phone_lookup.raw_contact_id = raw_contacts._id and data.raw_contact_id = raw_contacts._id GROUP BY normalized_number;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_ANDROID_ACCOUNTS)//accounts.db
    {
      fci.type = SQLITE_TYPE_ANDROID_ACCOUNTS;
      sqlite3_exec(db, "SELECT name, type, password FROM accounts;", callback_sqlite, &fci, NULL);
      sqlite3_exec(db, "SELECT name, authtokens.type,authtoken FROM authtokens,accounts WHERE accounts_id=accounts ._id;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_ANDROID_CALLENDAR)//calendar.db
    {
      fci.type = SQLITE_TYPE_ANDROID_CALLENDAR;
      sqlite3_exec(db, "SELECT DISTINCT _sync_account, title, eventLocation, description, strftime('%Y/%m/%d-%H:%M:%S',lastDate/1000, 'unixepoch', 'localtime') FROM Events;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_ANDROID_SMS == SQLITE_TYPE_ANDROID_SMS)//logs.db
    {
      fci.type = SQLITE_TYPE_ANDROID_SMS;
      sqlite3_exec(db, "SELECT number,name,duration,type,m_content FROM logs;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_ANDROID_MMS == SQLITE_TYPE_ANDROID_MMS)//mmssms.db
    {
      fci.type = SQLITE_TYPE_ANDROID_MMS;
      sqlite3_exec(db, "SELECT address,body,service_center,read,strftime('%Y/%m/%d-%H:%M:%S',date/1000, 'unixepoch', 'localtime') FROM sms;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_ANDROID_OP)//optable.db
    {
      fci.type = SQLITE_TYPE_ANDROID_OP;
      sqlite3_exec(db, "SELECT full,plmn FROM operator;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_ANDROID_GPS_POS)//CachedGeoposition.db
    {
      fci.type = SQLITE_TYPE_ANDROID_GPS_POS;
      sqlite3_exec(db, "SELECT latitude,longitude,strftime('%Y/%m/%d-%H:%M:%S',timestamp/1000, 'unixepoch', 'localtime') FROM CachedPosition;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_ANDROID_MEMO)//Memo.db
    {
      fci.type = SQLITE_TYPE_ANDROID_MEMO;
      sqlite3_exec(db,"SELECT content,strftime('%Y/%m/%d-%H:%M:%S',modify_t/1000, 'unixepoch', 'localtime'),strftime('%Y/%m/%d-%H:%M:%S',create_t/1000, 'unixepoch', 'localtime') FROM memo;", callback_sqlite, &fci, NULL);
      //sqlite3_exec(db,"SELECT content,modify_t,create_t FROM memo;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_ANDROID_DOC)//DocList.db
    {
      fci.type = SQLITE_TYPE_ANDROID_DOC;
      sqlite3_exec(db, "SELECT title,creator,owner,strftime('%Y/%m/%d-%H:%M:%S',creationTime/1000, 'unixepoch', 'localtime'),strftime('%Y/%m/%d-%H:%M:%S',lastModifiedTime/1000, 'unixepoch', 'localtime'),strftime('%Y/%m/%d-%H:%M:%S',lastOpenedTime/1000, 'unixepoch', 'localtime') FROM Entry16;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_ANDROID_SEARCH)//DocList.db
    {
      fci.type = SQLITE_TYPE_ANDROID_SEARCH;
      sqlite3_exec(db, "SELECT name,query,latitude,longitude,location,strftime('%Y/%m/%d-%H:%M:%S',time/1000, 'unixepoch', 'localtime') FROM search_queries;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_ANDROID_HISTORY)//history.db
    {
      fci.type = SQLITE_TYPE_ANDROID_HISTORY;
      sqlite3_exec(db, "SELECT query,strftime('%Y/%m/%d-%H:%M:%S',date/1000, 'unixepoch', 'localtime') FROM suggestions;", callback_sqlite, &fci, NULL);
    }

    if ((type & SQLITE_TYPE_ANDROID_APP_FAVORIS) == SQLITE_TYPE_ANDROID_APP_FAVORIS)//launcher.db
    {
      fci.type = SQLITE_TYPE_ANDROID_APP_FAVORIS;
      sqlite3_exec(db, "SELECT title FROM favorites;", callback_sqlite, &fci, NULL);
    }

    if ((type & SQLITE_TYPE_ANDROID_APP_CACHE) == SQLITE_TYPE_ANDROID_APP_CACHE)//ApplicationCache.db
    {
      fci.type = SQLITE_TYPE_ANDROID_APP_CACHE;
      sqlite3_exec(db, "SELECT url,responseURL,headers FROM CacheResources;", callback_sqlite, &fci, NULL);
    }

    if ((type & SQLITE_TYPE_ANDROID_APP_CACHE_URL) == SQLITE_TYPE_ANDROID_APP_CACHE_URL)//beamy.db
    {
      fci.type = SQLITE_TYPE_ANDROID_APP_CACHE_URL;
      sqlite3_exec(db, "SELECT url_img,title,strftime('%Y/%m/%d-%H:%M:%S',ts/1000, 'unixepoch', 'localtime'),lat,lng FROM list_entry;", callback_sqlite, &fci, NULL);
    }

    if ((type & SQLITE_TYPE_ANDROID_APP_BROWSER) == SQLITE_TYPE_ANDROID_APP_BROWSER)//browser.db
    {
      fci.type = SQLITE_TYPE_ANDROID_APP_BROWSER;
      sqlite3_exec(db, "SELECT title,url,strftime('%Y/%m/%d-%H:%M:%S',date/1000, 'unixepoch', 'localtime'),strftime('%Y/%m/%d-%H:%M:%S',created/1000, 'unixepoch', 'localtime') FROM bookmarks;", callback_sqlite, &fci, NULL);
    }

    if ((type & SQLITE_TYPE_ANDROID_APP_BROWSER_SEARCH) == SQLITE_TYPE_ANDROID_APP_BROWSER_SEARCH)//browser.db
    {
      fci.type = SQLITE_TYPE_ANDROID_APP_BROWSER_SEARCH;
      sqlite3_exec(db, "SELECT search,strftime('%Y/%m/%d-%H:%M:%S',date/1000, 'unixepoch', 'localtime') FROM searches;", callback_sqlite, &fci, NULL);
    }

    if ((type & SQLITE_TYPE_ANDROID_APP_BROWSER_FORM) == SQLITE_TYPE_ANDROID_APP_BROWSER_FORM)//webview.db
    {
      fci.type = SQLITE_TYPE_ANDROID_APP_BROWSER_FORM;
      sqlite3_exec(db, "SELECT name,value,domain,path FROM cookies;", callback_sqlite, &fci, NULL);
      sqlite3_exec(db, "SELECT name,value,url FROM formdata,formurl WHERE formdata .urlid=formurl._id;", callback_sqlite, &fci, NULL);
    }

    if ((type & SQLITE_TYPE_ANDROID_APP_BROWSER_FORM_AUTH) == SQLITE_TYPE_ANDROID_APP_BROWSER_FORM_AUTH)//webview.db
    {
      fci.type = SQLITE_TYPE_ANDROID_APP_BROWSER_FORM_AUTH;
      sqlite3_exec(db, "SELECT host,username,password FROM httpauth;", callback_sqlite, &fci, NULL);
      sqlite3_exec(db, "SELECT host,username,password FROM password;", callback_sqlite, &fci, NULL);
    }

    if ((type & SQLITE_TYPE_ANDROID_APP_BROWSER_CACHE) == SQLITE_TYPE_ANDROID_APP_BROWSER_CACHE)//webviewCache.db
    {
      fci.type = SQLITE_TYPE_ANDROID_APP_BROWSER_CACHE;
      sqlite3_exec(db, "SELECT url,lastmodify FROM cache;", callback_sqlite, &fci, NULL);
    }
    //strftime('%Y/%m/%d-%H:%M:%S',profile_created/1000, 'unixepoch', 'localtime')



    if ((type & SQLITE_TYPE_ANDROID_APP_MEDIA) == SQLITE_TYPE_ANDROID_APP_MEDIA)//internal.db
    {
      fci.type = SQLITE_TYPE_ANDROID_APP_MEDIA;
      sqlite3_exec(db, "SELECT _data,strftime('%Y/%m/%d-%H:%M:%S',date_added, 'unixepoch', 'localtime'),strftime('%Y/%m/%d-%H:%M:%S',date_modified, 'unixepoch', 'localtime')  FROM audio_meta;", callback_sqlite, &fci, NULL);
    }

    if ((type & SQLITE_TYPE_ANDROID_APP_MEDIA_VIDEO) == SQLITE_TYPE_ANDROID_APP_MEDIA_VIDEO)//internal.db
    {
      fci.type = SQLITE_TYPE_ANDROID_APP_MEDIA_VIDEO;
      sqlite3_exec(db, "SELECT _display_name,_data, strftime('%Y/%m/%d-%H:%M:%S',date_added, 'unixepoch', 'localtime'),strftime('%Y/%m/%d-%H:%M:%S',datetaken, 'unixepoch', 'localtime'),latitude,longitude FROM video;", callback_sqlite, &fci, NULL);
    }

    if ((type & SQLITE_TYPE_ANDROID_APP_MEDIA_PHOTO) == SQLITE_TYPE_ANDROID_APP_MEDIA_PHOTO)//internal.db
    {
      fci.type = SQLITE_TYPE_ANDROID_APP_MEDIA_PHOTO;
      sqlite3_exec(db, "SELECT _display_name,_data, strftime('%Y/%m/%d-%H:%M:%S',date_added, 'unixepoch', 'localtime'),strftime('%Y/%m/%d-%H:%M:%S',datetaken, 'unixepoch', 'localtime'),latitude,longitude FROM images;", callback_sqlite, &fci, NULL);
    }

    if ((type & SQLITE_TYPE_ANDROID_APP_MAP) == SQLITE_TYPE_ANDROID_APP_MAP)//local_active_places.db
    {
      fci.type = SQLITE_TYPE_ANDROID_APP_MAP;
      sqlite3_exec(db, "SELECT name,address,strftime('%Y/%m/%d-%H:%M:%S',lastUpdated/1000, 'unixepoch', 'localtime') FROM places;", callback_sqlite, &fci, NULL);
    }

    if ((type & SQLITE_TYPE_ANDROID_APP_MAIL) == SQLITE_TYPE_ANDROID_APP_MAIL)//mailstore.*.db
    {
      fci.type = SQLITE_TYPE_ANDROID_APP_MAIL;
      sqlite3_exec(db, "SELECT subject,fromAddress,toAddresses,joinedAttachmentInfos,strftime('%Y/%m/%d-%H:%M:%S',dateSentMs/1000, 'unixepoch', 'localtime'),strftime('%Y/%m/%d-%H:%M:%S',dateReceivedMs/1000, 'unixepoch', 'localtime') FROM messages;", callback_sqlite, &fci, NULL);
    }

    if ((type & SQLITE_TYPE_ANDROID_APP_RSS) == SQLITE_TYPE_ANDROID_APP_RSS)//reader.db
    {
      fci.type = SQLITE_TYPE_ANDROID_APP_RSS;
      sqlite3_exec(db, "SELECT title,html_url, strftime('%Y/%m/%d-%H:%M:%S',newest_item_time, 'unixepoch', 'localtime'),strftime('%Y/%m/%d-%H:%M:%S',sync_time/1000, 'unixepoch', 'localtime') FROM subscription;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_ANDROID_APP_EVERNOTE)//Evernote.db
    {
      fci.type = SQLITE_TYPE_ANDROID_APP_EVERNOTE;
      sqlite3_exec(db, "SELECT title,strftime('%Y/%m/%d-%H:%M:%S',created/1000, 'unixepoch', 'localtime'),strftime('%Y/%m/%d-%H:%M:%S',updated/1000, 'unixepoch', 'localtime'),latitude,longitude FROM notes;", callback_sqlite, &fci, NULL);
    }

    if (type & SQLITE_TYPE_ANDROID_APP_LINKEDIN)//linkedin.db
    {
      fci.type = SQLITE_TYPE_ANDROID_APP_LINKEDIN;
      sqlite3_exec(db, "SELECT first_name,last_name,picture_url,headline,li_auth_token FROM connections;", callback_sqlite, &fci, NULL);
    }

    if ((type & SQLITE_TYPE_ANDROID_APP_TWEET) == SQLITE_TYPE_ANDROID_APP_TWEET)//*.db
    {
      fci.type = SQLITE_TYPE_ANDROID_APP_TWEET;
      sqlite3_exec(db, "SELECT text,name,source,entities,strftime('%Y/%m/%d-%H:%M:%S',created_at/1000, 'unixepoch', 'localtime') FROM tweet;", callback_sqlite, &fci, NULL);
    }

    if ((type & SQLITE_TYPE_ANDROID_APP_TWEET_CONTACT) == SQLITE_TYPE_ANDROID_APP_TWEET_CONTACT)//*.db
    {
      fci.type = SQLITE_TYPE_ANDROID_APP_TWEET_CONTACT;
      sqlite3_exec(db, "SELECT username,name,description,web_url,location,strftime('%Y/%m/%d-%H:%M:%S',profile_created/1000, 'unixepoch', 'localtime'),strftime('%Y/%m/%d-%H:%M:%S',updated/1000, 'unixepoch', 'localtime') FROM users;", callback_sqlite, &fci, NULL);
    }

    if ((type & SQLITE_TYPE_ANDROID_APP_TWEET_SEARCH) == SQLITE_TYPE_ANDROID_APP_TWEET_SEARCH)//*.db
    {
      fci.type = SQLITE_TYPE_ANDROID_APP_TWEET_SEARCH;
      sqlite3_exec(db, "SELECT name,query,strftime('%Y/%m/%d-%H:%M:%S',time/1000, 'unixepoch', 'localtime') FROM search_queries;", callback_sqlite, &fci, NULL);
    }

    if ((type & SQLITE_TYPE_ANDROID_APP_TWEET_MP) == SQLITE_TYPE_ANDROID_APP_TWEET_MP)//*.db
    {
      fci.type = SQLITE_TYPE_ANDROID_APP_TWEET_MP;
      sqlite3_exec(db, "SELECT content,strftime('%Y/%m/%d-%H:%M:%S',created/1000, 'unixepoch', 'localtime'),username,name,description,location,strftime('%Y/%m/%d-%H:%M:%S',profile_created/1000, 'unixepoch', 'localtime'),strftime('%Y/%m/%d-%H:%M:%S',updated/1000, 'unixepoch', 'localtime') FROM messages,users WHERE users.user_id = messages.sender_id;", callback_sqlite, &fci, NULL);
    }

    //close
    sqlite3_close(db);
  }
}
//------------------------------------------------------------------------------
/*#define format_xml    0x0000    // séparateurs : < >
#define format_conf   0x0001    // séparateurs : { } ,format: "param":"value"
#define format_conf_e 0x0002    // sans section directement entre {} comme conf
#define format_ini    0x0003    //fichier ini [section], format param:data\r\n
#define format_f_bm   0x0004    // bookmark firefox

typedef struct
{
  char c[MAX_PATH];
}XML_PARAMS;
typedef struct
{
  char section[MAX_PATH];
  unsigned int format;
  unsigned int nb_value;
  XML_PARAMS *params;
}XML_FORMAT_TO_LV;
//------------------------------------------------------------------------------
void XMLtoLSTV(HANDLE hlv, char *file, XML_FORMAT_TO_LV*xftl)
{
  //ouverte et mise en mémoire du fichier !!!
  //ouverture du fichier
  HANDLE Hfic = CreateFile(file,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,0,0);
  if (Hfic == INVALID_HANDLE_VALUE)
  {
    SB_add_T(SB_ONGLET_CONF-1, "CONF : File access error");
    return;
  }
  DWORD taille_fic = GetFileSize(Hfic,NULL);
  //0x5068 = 0x5000 = start + 0x68 = 104 = taille min d'un enregistrement
  if (taille_fic<0x5068 || taille_fic == INVALID_FILE_SIZE)
  {
    CloseHandle(Hfic);
    return;
  }

  //allocation
  char *buffer = (char *) HeapAlloc(GetProcessHeap(), 0, taille_fic+1);
  if (!buffer)
  {
    CloseHandle(Hfic);
    SB_add_T(SB_ONGLET_CONF-1, "CONF : Out of memory");
    return;
  }

  //chargement du fichier en mémoire par morceau de 10mo
  //(pour éviter les pertes de synchro sur certaines machines)
  DWORD copiee, position = 0, increm = 0;
  if (taille_fic > DIXM)increm = DIXM;
  else increm = taille_fic;

  while (position<taille_fic && increm!=0)//gestion pour éviter les bug de sync permet une ouverture de fichiers énormes ^^
  {
    copiee = 0;
    ReadFile(Hfic, buffer+position, increm,&copiee,0);
    position +=copiee;
    if (taille_fic-position < increm)increm = taille_fic-position ;
  }
  CloseHandle(Hfic);

  //traitement du buffer
  if (position>0)
  {
    taille_fic = position;
    char *b = buffer;
    char *s = xftl->section;  //terme de réference

   /* do
    {
      //recherche dans le fichier des termes associées



    }while (*b);*//*
  }

  //on libère la mémoire
  HeapFree(GetProcessHeap(), 0, buffer);
}*/
