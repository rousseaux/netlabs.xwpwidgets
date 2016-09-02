/****************************************************************************\
 *                    Файл CLIST.CPP, группа "GPCLASS".                     *
 *                        Реализация класса списка.                         *
 *                                                                          *
 *                      General Purpose Class Library                       *
 * (c) Copyright 1998-99, Igor Vanin, 2:5030/448@fidonet, 55:1055/1@general *
\****************************************************************************/


#ifdef MEMORY_EXTENDED
static char *__mx_filename=__FILE__;
#endif


#include "Clist.hpp"


/****************************************************************************\
                            Конструкторы CList
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
                              Деструктор CList
\****************************************************************************/
CList::~CList() {
  clear();
  if (info && delete_data) delete info;
}


/****************************************************************************\
                         add -- добавление в список
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
                   insert -- вставка в любое место списка
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
                           clear -- очистка списка
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
             remove -- удаление произвольного элемента из списка
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
             remove -- удаление произвольного элемента из списка MOE
\****************************************************************************/
void CList::remove(CListData *killme)                    // moe
{
  if(!killme) return;


  CListData *d = First, *p;

  int counter=0;
  while(d){
     if(d==killme){
        // нашли его - стереть

        // если это первый, то
        if(counter==0){
           CListData *f = First;
           First = First->next;
           if (f->data) delete f->data;
           delete f;
           if (!First) last = 0;
           //fprintf(log1, " <<< удалил из списка первого\n");
        }
        else{
           p->next = d->next;
           if (d->data) delete d->data;
           delete d;
           if (!p->next) last = p;
           //fprintf(log1, " <<< удалил из списка\n");
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
             detach -- изъятие произвольного элемента из списка
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
          query -- опрос значения произвольного элемента из списка
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
       search_str -- поиск строки в списке (если список содержит строки)
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
          search_data -- поиск элемента фиксированной длины в списке
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
