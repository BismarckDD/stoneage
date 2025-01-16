#ifndef __MAIL_H__
#define __MAIL_H__

void receiveMail(const char *id_from, const char *char_name_from,
                 const char *id_to, const char *char_name_to,
                 const char *message, const int option,
                 const unsigned use_msgid, const unsigned msg_id);
void receiveMailAck(const char *id, const char *char_name, const int a,
                    const unsigned mesgid);
void flushMail(const int fd, const char *id, const char *char_name);
void expireMail();
int readMail(const char *dir);

#endif
