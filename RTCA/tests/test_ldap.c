//------------------------------------------------------------------------------
// Projet RtCA          : Read to Catch All
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "../RtCA.h"
//------------------------------------------------------------------------------
void addLDAPtoDB(char *dit, char *value, char *type, char *data, unsigned int session_id, sqlite3 *db)
{
  char request[REQUEST_MAX_SIZE];
  snprintf(request,REQUEST_MAX_SIZE,
           "INSERT INTO extract_ldap (dit,value,type,data,session_id) "
           "VALUES(\"%s\",\"%s\",\"%s\",\"%s\",%d);",
           dit,value,type,data,session_id);
  sqlite3_exec(db,request, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
//http://msdn.microsoft.com/en-us/library/windows/desktop/aa367033%28v=vs.85%29.aspx
//http://msdn.microsoft.com/en-us/library/aa367016%28v=vs.85%29.aspx

//help from : http://msdn.microsoft.com/en-us/library/aa367016%28v=vs.85%29.aspx
//http://msdn.microsoft.com/en-us/library/exchange/ms998858%28v=exchg.65%29.aspx
//http://books.google.fr/books?id=STOAuoPkJdQC&pg=PA472&lpg=PA472&dq=ldap_get_values%28&source=bl&ots=hNQRoH0EwG&sig=jN7-7DoRWbGKAz9i3Z37Zw1K4aI&hl=fr&sa=X&ei=E7qYUdGWGYOLhQeZl4D4AQ&ved=0CFYQ6AEwBjge#v=onepage&q=ldap_get_values%28&f=false
//------------------------------------------------------------------------------
//init LDAP
LDAP* init_ldap_connection()
{
  LDAP* ldap_c = NULL;

  //init HANDLE
  if ((ldap_c = ldap_init(NULL,LDAP_PORT)) == NULL)return NULL;

  //LDAP V3
  ULONG version = LDAP_VERSION3;
  if (ldap_set_option(ldap_c,LDAP_OPT_PROTOCOL_VERSION,(void*)&version) == LDAP_SUCCESS)
  {
    if (ldap_connect(ldap_c,NULL) == LDAP_SUCCESS)
    {
      //use local user credentials
      if (ldap_bind_s(ldap_c,NULL,NULL,LDAP_AUTH_NEGOTIATE) != LDAP_SUCCESS)
      {
        ldap_unbind(ldap_c);
        return NULL;
      }
    }else
    {
      ldap_unbind(ldap_c);
      return NULL;
    }
  }else
  {
    ldap_unbind(ldap_c);
    return NULL;
  }

  return ldap_c;
}
//------------------------------------------------------------------------------
BOOL GetLdapValues(char *dc, char *filter, unsigned int session_id, sqlite3 *db)
{
  LDAP* ldap_c = init_ldap_connection();
  if (dc != NULL && ldap_c != NULL)
  {
    #ifdef DEV_DEBUG_MODE
    printf("\n[LDAP] Get init \"%s\" (GetLdapValues) OK!\n",dc);
    #endif

    //search for all values with attributs !!!
    LDAPMessage *pMsg_dc = NULL;
    if (ldap_search_s(ldap_c,dc,LDAP_SCOPE_SUBTREE,filter,NULL,FALSE,&pMsg_dc) == LDAP_SUCCESS)
    {
      if (pMsg_dc != NULL)
      {
        #ifdef DEV_DEBUG_MODE
        printf("[LDAP] Start enum values for \"%s\" (%s) OK!\n",dc,filter);
        #endif

        //get count resultas
        DWORD nb_resultst =ldap_count_entries(ldap_c,pMsg_dc);
        #ifdef DEV_DEBUG_MODE
        printf("[LDAP] Get nb_resultst %d (ldap_count_entries) OK!\n",nb_resultst);
        #endif

        if (nb_resultst>0)
        {
          LDAPMessage* pEntry = NULL;
          char *pAttribute = NULL;
          BerElement* pBer = NULL;
          char **ppValue = NULL;
          DWORD i =0, j, jValue;

          pEntry = ldap_first_entry(ldap_c, pMsg_dc);
          do
          {
            //error
            if(pEntry == NULL)
            {
              printf("[LDAP] failed with 0x%0lx\n", LdapGetLastError());
              break;
            }

            #ifdef DEV_DEBUG_MODE
            printf("[LDAP] entry %d OK!\n",i);
            #endif

            //get only name for path !
            /*char **ppszConfigDN = NULL;
            ppszConfigDN = ldap_get_values(ldap_c, pMsg_dc, "distinguishedName");
           /* if (ppszConfigDN!=NULL)
            {

            }*/

            //get atribute
            pAttribute = ldap_first_attribute(ldap_c,pEntry,&pBer);
            while(pAttribute != NULL)
            {
              //get value
              #ifdef DEV_DEBUG_MODE
              printf("[LDAP]  pAttribute %s :\n",pAttribute);
              #endif

              //Get the string values
              ppValue = ldap_get_values(ldap_c,pEntry,pAttribute);
              if (ppValue != NULL)
              {
                jValue = ldap_count_values(ppValue);
                for (j=0;j<jValue;j++)
                {
                  #ifdef DEV_DEBUG_MODE
                  printf("[LDAP]   ppValue %s\n",ppValue[j]);
                  #endif

                  addLDAPtoDB(dc, pAttribute, ""/*ppszConfigDN[0]*/, ppValue[j], session_id, db);
                }
                ldap_value_free(ppValue);
                ppValue = NULL;
              }

              //next
              ldap_memfree(pAttribute);
              pAttribute = ldap_next_attribute(ldap_c,pEntry,pBer);
            }
            pEntry = ldap_next_entry(ldap_c, pEntry);
            i++;
          }while(pEntry != NULL);
        }
        ldap_msgfree(pMsg_dc);
      }
    }
    ldap_unbind(ldap_c);
  }
}
//------------------------------------------------------------------------------
DWORD Scan_ldap_g(LPVOID lParam, unsigned int session_id, sqlite3 *db)
{
  LDAP* ldap_c = init_ldap_connection();
  if (ldap_c != NULL)
  {
    #ifdef DEV_DEBUG_MODE
    printf("[LDAP] init connection OK!\n");
    #endif

    //get init DN
    LDAPMessage *pMsg_dc = NULL;
    if (ldap_search_s(ldap_c,NULL,LDAP_SCOPE_BASE,NULL,NULL,FALSE,&pMsg_dc) == LDAP_SUCCESS)
    {
      if (pMsg_dc != NULL)
      {
        #ifdef DEV_DEBUG_MODE
        printf("[LDAP] Get init DN (ldap_search_s) OK!\n");
        #endif

        //get datas + new search with new datas !!!
        char **ppszConfigDN = NULL;
        ppszConfigDN = ldap_get_values(ldap_c, pMsg_dc, "configurationNamingContext");
        if (ppszConfigDN!=NULL)
        {
          DWORD i, iValue = ldap_count_values(ppszConfigDN);

          #ifdef DEV_DEBUG_MODE
          printf("[LDAP] Get iValue %d (ldap_count_values) OK!\n",iValue);
          #endif

          //get DN root values one by one
          LDAPMessage *pMsg = NULL;
          unsigned long int j=0;
          for (i=0;i<iValue;i++)
          {
            #ifdef DEV_DEBUG_MODE
            printf("[LDAP] Get iValue[%d] \"%s\" (ldap_get_values) OK!\n",i,ppszConfigDN[i]);
            #endif

            //get values
            j = Contient(ppszConfigDN[i],"DC=");
            if (j>0)
            {
              //extract DC and load datas
              char *c,tmp[DEFAULT_TMP_SIZE],request[DEFAULT_TMP_SIZE];
              snprintf(tmp,DEFAULT_TMP_SIZE,"%s",ppszConfigDN[i]);
              c = tmp+(j-3);

              #ifdef DEV_DEBUG_MODE
              printf("[LDAP] Get view DC : (%d) \"%s\" OK!\n",j,c);
              #endif

              //get standards informations :
              GetLdapValues(c,"(ObjectClass=*)", session_id, db);
              snprintf(request,DEFAULT_TMP_SIZE,"CN=Configuration,%s",c);
              GetLdapValues(request,"(ObjectClass=*)", session_id, db);
              snprintf(request,DEFAULT_TMP_SIZE,"CN=Schema,CN=Configuration,%s",c);
              GetLdapValues(request,"(ObjectClass=*)", session_id, db);
              snprintf(request,DEFAULT_TMP_SIZE,"DC=DomainDnsZones,%s",c);
              GetLdapValues(request,"(ObjectClass=*)", session_id, db);
              snprintf(request,DEFAULT_TMP_SIZE,"DC=ForestDnsZones,%s",c);
              GetLdapValues(request,"(ObjectClass=*)", session_id, db);
            }else GetLdapValues(ppszConfigDN[i],"(ObjectClass=*)", session_id, db);
          }
        }
      }
    }

    //clean
    ldap_unbind(ldap_c);
  }
}
//------------------------------------------------------------------------------
DWORD WINAPI Scan_ldap(LPVOID lParam)
{
  //check if local or not :)
  if (!LOCAL_SCAN)
  {
    h_thread_test[(unsigned int)lParam] = 0;
    check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);
    return 0;
  }

  //init
  if(!SQLITE_FULL_SPEED)sqlite3_exec(db_scan,"BEGIN TRANSACTION;", NULL, NULL, NULL);
  sqlite3 *db = (sqlite3 *)db_scan;
  unsigned int session_id = current_session_id;

  Scan_ldap_g(lParam,session_id,db);

  if(!SQLITE_FULL_SPEED)sqlite3_exec(db_scan,"END TRANSACTION;", NULL, NULL, NULL);
  check_treeview(htrv_test, H_tests[(unsigned int)lParam], TRV_STATE_UNCHECK);//db_scan
  h_thread_test[(unsigned int)lParam] = 0;
  return 0;
}
