#ifndef   __CLIST_HPP__
#define   __CLIST_HPP__
#ifdef    __cplusplus
  extern "C" {
#endif




#ifndef _CLIST_HPP_INCLUDED
#define _CLIST_HPP_INCLUDED

#ifndef __BSEDEV__
#define INCL_BASE
#include <os2.h>
#endif


struct CListData {
  void *data;
  CListData *next;
};


class CList {

// data members:
private:
  CListData *First;
  CListData *last;
  unsigned long Length;

// member functions:
public:
  CList();
  CList(unsigned long _del_data);
  ~CList();

  unsigned long delete_data;
  void  add(void *data);
  void  insert(void *data, unsigned long before);
  void  remove(unsigned long number);
  void  remove(CListData *killme);
  void *detach(long number);
  void  clear(void);

  inline unsigned long length(void) { return Length; }
  inline CListData *first(void) { return First; }
  void *query(unsigned long number);
  inline void *operator[](unsigned long number) { return query(number); }
  long search_str(char *str, BOOL ignore_case);
  long search_data(void *data, ULONG datasize);

  void *info;
};

#endif

#ifdef    __cplusplus
  }
#endif

#endif // __CLIST_HPP__
