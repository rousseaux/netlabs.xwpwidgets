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

/* LngOpenDialect - создаёт структуру, необходимую для получения сообщений
 * указанного диалекта из файла, и возвращает её хэндл.
 * Если Dialect = '', то используется значение %LANG%
 * Если Dialect = 'xx_' - используется первый диалект, начинающийся с 'xx_'
 * Если Dialect = '_xx' - используется первый диалект, заканчивающийся на '_xx'
 * Если Dialect = '_'   - используется первый попавшийся диалект
 * Если CodePage = 0, то используется кодовая страница процесса
 * Возвращает результаты DosAllocMem, DosFreeMem, DosScanEnv, DosSearchPath,
 * DosOpen, DosRead, DosSetFilePtr, DosQueryCp, UniMapCpToUcsCp, UniCreateUconvObject,
 * а также:
 * ERROR_BAD_FORMAT - если файл неправильный,
 * ERROR_INVALID_NAME - если имя диалекта неправильное,
 * ERROR_NO_ITEMS - если диалект не найден
 */
APIRET APIENTRY LngOpenDialect(PCHAR MsgFile, PLNGHANDLE Handle);

/* LngLoadDialect - делает то же, что и LngOpenDialect, плюс загружает в память
 * все сообщения указанного диалекта для максимально быстрого доступа к ним.
 * Параметры и результат полностью идентичны LngOpenDialect.
 */
APIRET APIENTRY LngLoadDialect(PCHAR nlsfile, PLNGHANDLE phandle);

/* LngCloseDialect - уничтожает дескриптор диалекта (вместе с сообщениями,
 * если они были загружены).
 * Значение переменной Handle не меняется.
 * Вовращает результаты DosFreeMem, DosClose, UniFreeUconvObject
 * Кроме того: ERROR_INVALID_HANDLE
 */
APIRET APIENTRY LngCloseDialect(LNGHANDLE Handle);

/* LngGetString - получает сообщение без подстановки параметроа.
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
Функции из YUPLANGE. Много поменялось, по сравнению с оригиналом.
*/

#if 0
/* LngGetDialects - возвращает информацию о запрошенных диaлектах.
 * Dialect = ''    - имя диалекта - в %LANG%
 * Dialect = 'xx_' - все диалекты, начинающиеся с 'xx_'
 * Dialect = '_xx' - все диалекты, заканчивающиеся на '_xx'
 * Dialect = '_'   - все диалекты
 * Буфер заполняется последовательностью записей типа DialectInfo.
 * Если буфер мал, то в DialsCount возвращается необходимая длина.
 * Возвращает результаты DosScanEnv, DosSearchPath, DosOpen, DosRead,
 * DosAllocMem, DosFreeMem,
 * а также:
 * ERROR_BAD_FORMAT - если файл неправильный,
 * ERROR_INVALID_NAME - если имя диалекта неправильное,
 * ERROR_INSUFFICIENT_BUFFER - если BufSize не достаточно. В этом случае в
 *   DialsCount возвращается необходимый размер в элементах.
 */

APIRET LngGetDialects
( PCHAR          MsgFile        /* I  имя файла                            */
, PCHAR          Dialect        /* I  строка диалекта                      */
, ULONG          BufSize        /* I  размер буфера в записях DialectInfo  */
, DialectInfo *  Buffer         /* I  буфер для возвращаемой информации    */
, PULONG         DialsCount     /*  O необходимая длина буфера в элементах */
);



/* LngGetMessage - возвращает сообщение с подстановкой переменных.
 * Если строка не поместилась в буфер, то в RequiredLength возвращается
 * необходимая длина.
 * Возвращает результаты DosAllocMem, DosFreeMem, DosRead, DosSetFilePtr,
 * UniStrToUcs, UniUconvFromUcs, а также:
 * ERROR_INVALID_HANDLE - хэндл диалекта не верен
 * ERROR_NO_ITEMS - сообщение не найдено
 * ERROR_BAD_FORMAT - с файлом что-то не так
 * ERROR_INSUFFICIENT_BUFFER - BufSize не достаточно. В этом случае в
 *   RequiredLength возвращается необходимый размер в байтах.
 * ERROR_META_EXPANSION_TOO_LONG - длина полного сообщения превысила MAX(CARDINAL)
 */

APIRET LngGetMessage
( LngHandle  Handle             /* I  хэндл диалекта                        */
, PCHAR      MessageId          /* I  имя сообщения                         */
, ULONG      VarsCount          /* I  количество переменных в массиве       */
, PCHAR *    Vars               /* I  массив указателей на строки           */
                                /*    подстановочных переменных             */
, ULONG      BufSize            /* I  размер буфера для сообщения в байтах  */
, PVOID      BufferPtr          /* I  адрес буфера для сообщения            */
, PULONG     RequiredLength     /*  O реальная длина текста                 */
);




/* LngGetMessagePtr - возвращает указатель на предварительно загруженное с
 * помощью LngLoadDialect сообщение или NIL, если хэндл неверный, сообщение
 * не найдено или не загружено.
 */

PVOID PROCEDURE LngGetMessagePtr
( LngHandle  Handle             /* I  хэндл диалекта   */
, PCHAR      MessageId          /* I  имя сообщения    */
);

/* LngPeekMessage - возвращает сообщение с подстановкой переменных без
 * предварительного создания дескриптора.
 * Если строка не поместилась в буфер, то в RequiredLength возвращается
 * необходимая длина.
 * Возвращает результаты DosAllocMem, DosFreeMem, DosScanEnv, DosSearchPath,
 * DosOpen, DosRead, DosSetFilePtr, -DosClose, DosQueryCp, UniMapCpToUcsCp,
 * UniCreateUconvObject, UniStrToUcs, UniUconvFromUcs, -UniFreeUconvObject,
 * а также:
 * ERROR_INVALID_HANDLE - хэндл диалекта не верен
 * ERROR_NO_ITEMS - не найдено сообщение или диалект
 * ERROR_BAD_FORMAT - с файлом что-то не так
 * ERROR_INVALID_NAME - имя диалекта неправильное
 * ERROR_INSUFFICIENT_BUFFER - BufSize не достаточно. В этом случае в
 *   RequiredLength возвращается необходимый размер.
 * ERROR_META_EXPANSION_TOO_LONG - длина полного сообщения превысила MAX(CARDINAL)
 */

APIRET LngPeekMessage
( PCHAR    MsgFile              /* I  имя файла                             */
, PCHAR    Dialect              /* I  диалект                               */
, ULONG    CodePage             /* I  кодовая страница                      */
, PCHAR    MessageId            /* I  имя сообщения                         */
, ULONG    VarsCount            /* I  количество переменных в массиве       */
, PCHAR *  Vars                 /* I  массив указателей на строки           */
                                /*    подстановочных переменных             */
, ULONG    BufSize              /* I  размер буфера для сообщения в байтах  */
, PVOID    BufferPtr            /* I  адрес буфера для сообщения            */
, ULONG *  RequiredLength       /*  O реальная длина текста                 */
);


#endif

#ifdef __cplusplus
        }
#endif


#endif
/* LANGE_H_INCLUDED */
