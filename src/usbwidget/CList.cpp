/****************************************************************************\
 *                    ���� CLIST.CPP, ��㯯� "GPCLASS".                     *
 *                        ��������� ����� ᯨ᪠.                         *
 *                                                                          *
 *                      General Purpose Class Library                       *
 * (c) Copyright 1998-99, Igor Vanin, 2:5030/448@fidonet, 55:1055/1@general *
\****************************************************************************/


#ifdef MEMORY_EXTENDED
static char *__mx_filename=__FILE__;
#endif


#include "Clist.hpp"


/****************************************************************************\
                            ���������� CList
\****************************************************************************/
CList::CList() {
  First = 0; last = 0;
  Length = 0;
  info = 0;
  delete_data = 1;
}

CList::CList(unsigned long _del_data) {
  First = 0; last = 0;
  Length = 0;
  info = 0;
  delete_data = _del_data;
}


/****************************************************************************\
                              �������� CList
\****************************************************************************/
CList::~CList() {
  clear();
  if (info && delete_data) delete info;
}


/****************************************************************************\
                         add -- ���������� � ᯨ᮪
\****************************************************************************/
void CList::add(void *data) {
  CListData *newelement = new CListData;
  newelement->data = data;
  newelement->next = 0;
  if (!First)
    First = newelement;
  else
    last->next = newelement;
  last = newelement;
  Length++;
}


/****************************************************************************\
                   insert -- ��⠢�� � �� ���� ᯨ᪠
\****************************************************************************/
void CList::insert(void *data, unsigned long before) {
  CListData *newelement = new CListData;
  newelement->data = data;
  newelement->next = 0;
  if (before==0 || !First) {
    newelement->next = First;
    First = newelement;
  }
  else {
    CListData *p = First;
    before--;
    while (p->next && before) { p = p->next; before--; }
    newelement->next = p->next;
    p->next = newelement;
  }
  if (!newelement->next) last = newelement;
  Length++;
}


/****************************************************************************\
                           clear -- ���⪠ ᯨ᪠
\****************************************************************************/
void CList::clear(void) {
  if (!First) return;
  CListData *c,*n;
  c = First;
  while (c) {
    n = c->next;
    if (delete_data && c->data)
      delete c->data;
    delete c;
    c = n;
  }

  First = 0; last = 0; Length = 0;
}


/****************************************************************************\
             remove -- 㤠����� �ந����쭮�� ����� �� ᯨ᪠
\****************************************************************************/
void CList::remove(unsigned long number) {
  if (number >= Length) return;
  if (number == 0) {
    CListData *f = First;
    First = First->next;
    if (f->data) delete f->data;
    delete f;
    if (!First) last = 0;
  }
  else {
    CListData *d = First, *p;
    for (unsigned long i=number; i>0; i--) {
      if (d) {
        p = d;
        d = d->next;
      }
      if (!d) return;
    }
    p->next = d->next;
    if (d->data) delete d->data;
    delete d;
    if (!p->next) last = p;
  }
  Length--;
}

/****************************************************************************\
             remove -- 㤠����� �ந����쭮�� ����� �� ᯨ᪠ MOE
\****************************************************************************/
void CList::remove(CListData *killme)                    // moe
{
  if(!killme) return;


  CListData *d = First, *p;

  int counter=0;
  while(d){
     if(d==killme){
        // ��諨 ��� - �����

        // �᫨ �� ����, �
        if(counter==0){
           CListData *f = First;
           First = First->next;
           if (f->data) delete f->data;
           delete f;
           if (!First) last = 0;
           //fprintf(log1, " <<< 㤠��� �� ᯨ᪠ ��ࢮ��\n");
        }
        else{
           p->next = d->next;
           if (d->data) delete d->data;
           delete d;
           if (!p->next) last = p;
           //fprintf(log1, " <<< 㤠��� �� ᯨ᪠\n");
        }

        Length--;
        return;
     }
     p = d;
     d = d->next;
     counter++;
  }

  return;
}



/****************************************************************************\
             detach -- ����⨥ �ந����쭮�� ����� �� ᯨ᪠
\****************************************************************************/
void *CList::detach(long number) {
  void *ret;
  if (number == -1) {
    ret = info;
    info = 0;
    return ret;
  }
  if (number >= Length) return 0;
  if (number == 0) {
    CListData *f = First;
    First = First->next;
    ret = f->data;
    delete f;
    if (!First) last = 0;
  }
  else {
    CListData *d = First, *p;
    for (unsigned long i=number; i>0; i--) {
      if (d) {
        p = d;
        d = d->next;
      }
      if (!d) return 0;
    }
    p->next = d->next;
    ret = d->data;
    delete d;
    if (!p->next) last = p;
  }
  Length--;
  return ret;
}


/****************************************************************************\
          query -- ���� ���祭�� �ந����쭮�� ����� �� ᯨ᪠
\****************************************************************************/
void *CList::query(unsigned long number) {
  if (number == 0) return First ? First->data : 0;
  CListData *d = First;
  for (unsigned long i=number; i>0; i--) {
    if (d) d = d->next;
    else return 0;
  }
  return d ? d->data : 0;
}


/****************************************************************************\
       search_str -- ���� ��ப� � ᯨ᪥ (�᫨ ᯨ᮪ ᮤ�ন� ��ப�)
\****************************************************************************/
#include <string.h>
long CList::search_str(char *str, BOOL ignore_case) {
  CListData *d = First;
  long num = 0;
  while (d) {
    if (d->data) {
      if (ignore_case) {
        if (!strcmpi((char *)d->data, str)) return num;
      }
      else {
        if (!strcmp((char *)d->data, str)) return num;
      }
    }
    num++;
    d = d->next;
  }
  return -1;
}


/****************************************************************************\
          search_data -- ���� ����� 䨪�஢����� ����� � ᯨ᪥
\****************************************************************************/
long CList::search_data(void *data, ULONG datasize) {
  CListData *d = First;
  long num = 0;
  while (d) {
    if (d->data) {
      if (!memcmp((void *)d->data, data, datasize)) return num;
    }
    num++;
    d = d->next;
  }
  return -1;
}
