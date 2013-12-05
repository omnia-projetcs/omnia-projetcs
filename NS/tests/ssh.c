//----------------------------------------------------------------
// Project              : Network Scanner
// Author               : Hanteville Nicolas
// Licence              : GPLv3
//----------------------------------------------------------------
#include "../resources.h"
//----------------------------------------------------------------
//use libssh2 libraries : http://www.libssh2.org/ and lib http://josefsson.org/gnutls4win/libssh2-1.2.6.zip
void ssh_exec(DWORD iitem,char *ip, char*username, char*password)
{
  //for each command
  char buffer[0x4000],cmd[0x4000], msg[0x4000];
  unsigned int nb = 0;
  int rc;
  LIBSSH2_SESSION *session;
  LIBSSH2_CHANNEL *channel;
  DWORD i, _nb_i = SendDlgItemMessage(h_main,CB_T_SSH,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);
  for (i=0;i<_nb_i && scan_start;i++)
  {
    cmd[0] = 0;
    if (SendDlgItemMessage(h_main,CB_T_SSH,LB_GETTEXT,(WPARAM)i,(LPARAM)cmd))
    {
      if (cmd[0] == 0 || cmd[0] == '\r' || cmd[0] == '\n')continue;

      //init libssh2
      if (libssh2_init(0)==0)
      {
        //connexion
        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock != INVALID_SOCKET)
        {
          //connect
          struct sockaddr_in sin;
          sin.sin_family = AF_INET;
          sin.sin_port = htons(22);
          sin.sin_addr.s_addr = inet_addr(ip);
          if (connect(sock, (struct sockaddr*)(&sin),sizeof(struct sockaddr_in)) != SOCKET_ERROR)
          {
            //init a session sshv2
            session = libssh2_session_init();
            if (session != 0)
            {
              //start real session, pass welcome banners...
              if (!libssh2_session_startup(session, sock))
              {
                //authentication
                if (!libssh2_userauth_password(session, username, password))
                {
                  //create a channel for execute
                  if ((channel = libssh2_channel_open_session(session)) != 0)
                  {
                    //send command and read results
                    if (!libssh2_channel_exec(channel, cmd))
                    {
                      rc = libssh2_channel_read(channel, buffer, sizeof(buffer));
                      if (rc > 0)
                      {
                        snprintf(msg,0x4000,"[%s\\%s]\r\n",ip,cmd);
                        AddLSTVUpdateItem(msg, COL_SSH, iitem);
                        while(rc){
                          buffer[rc] = 0;
                          AddMsg(h_main,(char*)"FOUND (SSH)",cmd,buffer);
                          AddLSTVUpdateItem(buffer, COL_SSH, iitem);
                          rc = libssh2_channel_read(channel, buffer, sizeof(buffer));
                        }
                      }
                    }
                    //close the channel
                    libssh2_channel_close(channel);
                    libssh2_channel_free(channel);
                    channel = NULL;
                  }
                }
              }
              libssh2_session_disconnect(session,NULL);
              libssh2_session_free(session);
            }
          }
          closesocket(sock);
        }
        //clean
        libssh2_exit();
      }
    }
  }
}
//----------------------------------------------------------------
//use libssh libraries : https://red.libssh.org/projects/libssh/files
/*void ssh_exec(DWORD iitem,char *ip, char*login, char*mdp)
{
  ssh_session session = NULL;
  ssh_channel channel = NULL;

  int verbosity = SSH_LOG_PROTOCOL;
  int port = 22;

  char buffer[MAX_LINE_SIZE],cmd[MAX_LINE_SIZE];
  unsigned int nb = 0;
  DWORD i, _nb_i = SendDlgItemMessage(h_main,CB_T_SSH,LB_GETCOUNT,(WPARAM)NULL,(LPARAM)NULL);

  for (i=0;i<_nb_i && scan_start;i++)
  {
    if (SendDlgItemMessage(h_main,CB_T_SSH,LB_GETTEXT,(WPARAM)i,(LPARAM)cmd))
    {
      session = ssh_new();
      if (session != NULL)
      {
        ssh_options_set(session, SSH_OPTIONS_HOST, ip);
        ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
        ssh_options_set(session, SSH_OPTIONS_PORT, &port);
        ssh_options_set(session, SSH_OPTIONS_USER, login);

        if (ssh_connect(session) == SSH_OK)
        {
          if (ssh_userauth_password(session, NULL, mdp) == SSH_AUTH_SUCCESS)
          {
            channel = ssh_channel_new(session);
            if (channel != NULL)
            {
              if (ssh_channel_open_session(channel) == SSH_OK)
              {
                //execute command
                if (ssh_channel_request_exec(channel, cmd) == SSH_OK)
                {
                  nb = ssh_channel_read(channel, buffer, MAX_LINE_SIZE, 0);
                  while (nb)
                  {
                    AddMsg(h_main,(char*)"FOUND (SSH)",cmd,buffer);
                    nb = ssh_channel_read(channel, buffer, MAX_LINE_SIZE, 0);
                  }
                }
                ssh_channel_send_eof(channel);
                ssh_channel_close(channel);
              }
              ssh_channel_free(channel);
            }
          }
          ssh_disconnect(session);
        }
        ssh_free(session);
      }
    }
  }
}*/
