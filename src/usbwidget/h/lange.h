/* lange.h */
#ifndef  LANGE_H_INCLUDED

#define  LANGE_H_INCLUDED

#include <os2.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LNGHANDLE LHANDLE
typedef LNGHANDLE * PLNGHANDLE;
#define LANGE LHANDLE
typedef LANGE * PLANGE;

#define ERR_LNG_NO_ERROR 0
#define ERR_LNG_NO_KEY 1
#define ERR_LNG_NO_TARGET_BUFFER 2
#define ERR_LNG_NO_SEARCH_KEY 3

/* LngOpenDialect - ᮧ���� ��������, ����室���� ��� ����祭�� ᮮ�饭��
 * 㪠������� ������� �� 䠩��, � �����頥� �� ���.
 * �᫨ Dialect = '', � �ᯮ������ ���祭�� %LANG%
 * �᫨ Dialect = 'xx_' - �ᯮ������ ���� �������, ��稭��騩�� � 'xx_'
 * �᫨ Dialect = '_xx' - �ᯮ������ ���� �������, �����稢��騩�� �� '_xx'
 * �᫨ Dialect = '_'   - �ᯮ������ ���� �����訩�� �������
 * �᫨ CodePage = 0, � �ᯮ������ ������� ��࠭�� �����
 * �����頥� १����� DosAllocMem, DosFreeMem, DosScanEnv, DosSearchPath,
 * DosOpen, DosRead, DosSetFilePtr, DosQueryCp, UniMapCpToUcsCp, UniCreateUconvObject,
 * � ⠪��:
 * ERROR_BAD_FORMAT - �᫨ 䠩� ���ࠢ����,
 * ERROR_INVALID_NAME - �᫨ ��� ������� ���ࠢ��쭮�,
 * ERROR_NO_ITEMS - �᫨ ������� �� ������
 */
APIRET APIENTRY LngOpenDialect(PCHAR MsgFile, PLNGHANDLE Handle);

/* LngLoadDialect - ������ � ��, �� � LngOpenDialect, ���� ����㦠�� � ������
 * �� ᮮ�饭�� 㪠������� ������� ��� ���ᨬ��쭮 ����ண� ����㯠 � ���.
 * ��ࠬ���� � १���� ��������� ������� LngOpenDialect.
 */
APIRET APIENTRY LngLoadDialect(PCHAR nlsfile, PLNGHANDLE phandle);

/* LngCloseDialect - 㭨�⮦��� ���ਯ�� ������� (����� � ᮮ�饭�ﬨ,
 * �᫨ ��� �뫨 ����㦥��).
 * ���祭�� ��६����� Handle �� �������.
 * ����頥� १����� DosFreeMem, DosClose, UniFreeUconvObject
 * �஬� ⮣�: ERROR_INVALID_HANDLE
 */
APIRET APIENTRY LngCloseDialect(LNGHANDLE Handle);

/* LngGetString - ����砥� ᮮ�饭�� ��� ����⠭���� ��ࠬ��஠.
 *
 */
APIRET APIENTRY LngGetString(LNGHANDLE handle, PCHAR searchkey, PCHAR target, ULONG target_maxlength);

/****************************************************************************/
/*                                                                          */
/* All following functions are obsoled and not documented. Use at your risk */
/*                                                                          */
/****************************************************************************/

// Obsoled. Don't use.
APIRET APIENTRY LngLoadLanguage(PCHAR nlsfile, PCHAR dialect, PLNGHANDLE phandle);
APIRET APIENTRY LngUnLoadLanguage(LNGHANDLE handle);

// Backward compatability with old version
#define LangeLoadLanguage LngLoadLanguage
#define LangeGetString LngGetString
#define LangeUnLoadLanguage LngUnLoadLanguage

/*
�㭪樨 �� YUPLANGE. ����� �����﫮��, �� �ࠢ����� � �ਣ������.
*/

#if 0
/* LngGetDialects - �����頥� ���ଠ�� � ����襭��� ��a�����.
 * Dialect = ''    - ��� ������� - � %LANG%
 * Dialect = 'xx_' - �� ��������, ��稭��騥�� � 'xx_'
 * Dialect = '_xx' - �� ��������, �����稢��騥�� �� '_xx'
 * Dialect = '_'   - �� ��������
 * ���� ���������� ��᫥����⥫쭮���� ����ᥩ ⨯� DialectInfo.
 * �᫨ ���� ���, � � DialsCount �����頥��� ����室���� �����.
 * �����頥� १����� DosScanEnv, DosSearchPath, DosOpen, DosRead,
 * DosAllocMem, DosFreeMem,
 * � ⠪��:
 * ERROR_BAD_FORMAT - �᫨ 䠩� ���ࠢ����,
 * ERROR_INVALID_NAME - �᫨ ��� ������� ���ࠢ��쭮�,
 * ERROR_INSUFFICIENT_BUFFER - �᫨ BufSize �� �����筮. � �⮬ ��砥 �
 *   DialsCount �����頥��� ����室��� ࠧ��� � ������.
 */

APIRET LngGetDialects
( PCHAR          MsgFile        /* I  ��� 䠩��                            */
, PCHAR          Dialect        /* I  ��ப� �������                      */
, ULONG          BufSize        /* I  ࠧ��� ���� � ������� DialectInfo  */
, DialectInfo *  Buffer         /* I  ���� ��� �����頥��� ���ଠ樨    */
, PULONG         DialsCount     /*  O ����室���� ����� ���� � ������ */
);



/* LngGetMessage - �����頥� ᮮ�饭�� � ����⠭����� ��६�����.
 * �᫨ ��ப� �� �����⨫��� � ����, � � RequiredLength �����頥���
 * ����室���� �����.
 * �����頥� १����� DosAllocMem, DosFreeMem, DosRead, DosSetFilePtr,
 * UniStrToUcs, UniUconvFromUcs, � ⠪��:
 * ERROR_INVALID_HANDLE - ��� ������� �� ��७
 * ERROR_NO_ITEMS - ᮮ�饭�� �� �������
 * ERROR_BAD_FORMAT - � 䠩��� ��-� �� ⠪
 * ERROR_INSUFFICIENT_BUFFER - BufSize �� �����筮. � �⮬ ��砥 �
 *   RequiredLength �����頥��� ����室��� ࠧ��� � �����.
 * ERROR_META_EXPANSION_TOO_LONG - ����� ������� ᮮ�饭�� �ॢ�ᨫ� MAX(CARDINAL)
 */

APIRET LngGetMessage
( LngHandle  Handle             /* I  ��� �������                        */
, PCHAR      MessageId          /* I  ��� ᮮ�饭��                         */
, ULONG      VarsCount          /* I  ������⢮ ��६����� � ���ᨢ�       */
, PCHAR *    Vars               /* I  ���ᨢ 㪠��⥫�� �� ��ப�           */
                                /*    ����⠭������ ��६�����             */
, ULONG      BufSize            /* I  ࠧ��� ���� ��� ᮮ�饭�� � �����  */
, PVOID      BufferPtr          /* I  ���� ���� ��� ᮮ�饭��            */
, PULONG     RequiredLength     /*  O ॠ�쭠� ����� ⥪��                 */
);




/* LngGetMessagePtr - �����頥� 㪠��⥫� �� �।���⥫쭮 ����㦥���� �
 * ������� LngLoadDialect ᮮ�饭�� ��� NIL, �᫨ ��� ������, ᮮ�饭��
 * �� ������� ��� �� ����㦥��.
 */

PVOID PROCEDURE LngGetMessagePtr
( LngHandle  Handle             /* I  ��� �������   */
, PCHAR      MessageId          /* I  ��� ᮮ�饭��    */
);

/* LngPeekMessage - �����頥� ᮮ�饭�� � ����⠭����� ��६����� ���
 * �।���⥫쭮�� ᮧ����� ���ਯ��.
 * �᫨ ��ப� �� �����⨫��� � ����, � � RequiredLength �����頥���
 * ����室���� �����.
 * �����頥� १����� DosAllocMem, DosFreeMem, DosScanEnv, DosSearchPath,
 * DosOpen, DosRead, DosSetFilePtr, -DosClose, DosQueryCp, UniMapCpToUcsCp,
 * UniCreateUconvObject, UniStrToUcs, UniUconvFromUcs, -UniFreeUconvObject,
 * � ⠪��:
 * ERROR_INVALID_HANDLE - ��� ������� �� ��७
 * ERROR_NO_ITEMS - �� ������� ᮮ�饭�� ��� �������
 * ERROR_BAD_FORMAT - � 䠩��� ��-� �� ⠪
 * ERROR_INVALID_NAME - ��� ������� ���ࠢ��쭮�
 * ERROR_INSUFFICIENT_BUFFER - BufSize �� �����筮. � �⮬ ��砥 �
 *   RequiredLength �����頥��� ����室��� ࠧ���.
 * ERROR_META_EXPANSION_TOO_LONG - ����� ������� ᮮ�饭�� �ॢ�ᨫ� MAX(CARDINAL)
 */

APIRET LngPeekMessage
( PCHAR    MsgFile              /* I  ��� 䠩��                             */
, PCHAR    Dialect              /* I  �������                               */
, ULONG    CodePage             /* I  ������� ��࠭��                      */
, PCHAR    MessageId            /* I  ��� ᮮ�饭��                         */
, ULONG    VarsCount            /* I  ������⢮ ��६����� � ���ᨢ�       */
, PCHAR *  Vars                 /* I  ���ᨢ 㪠��⥫�� �� ��ப�           */
                                /*    ����⠭������ ��६�����             */
, ULONG    BufSize              /* I  ࠧ��� ���� ��� ᮮ�饭�� � �����  */
, PVOID    BufferPtr            /* I  ���� ���� ��� ᮮ�饭��            */
, ULONG *  RequiredLength       /*  O ॠ�쭠� ����� ⥪��                 */
);


#endif

#ifdef __cplusplus
        }
#endif


#endif
/* LANGE_H_INCLUDED */
