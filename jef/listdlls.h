/* Adapted from PEDUMP - by Matt Pietrek 1994-1998 */

#ifdef __cplusplus
extern "C" {
#endif

enum {
  LISTDLL_SUCCESS=0,
  LISTDLL_ERR_CANT_OPEN,    /* File could not be opened */
  LISTDLL_ERR_TOO_BIG,      /* File is larger than 4GB */
  LISTDLL_ERR_MAP,          /* File could not be memory-mapped */
  LISTDLL_ERR_MAPVIEW,      /* Memory-mapped file could not be accessed */
  LISTDLL_ERR_NOT_EXE,      /* Not a Windows or DOS executable */
  LISTDLL_ERR_NOT_PE,       /* Not a Windows PE executable */
  LISTDLL_ERR_BAD_PE,       /* File header is corrupt (possibly UPX compressed) */
  LISTDLL_ERR_NO_IMP_SECT,  /* Could not find imports section */
  LISTDLL_ERR_NO_IMP_HDR,   /* Could not find imports header */
  LISTDLL_ERR_NO_IMP_DESC,  /* Could not find first import description */
};

typedef int (ListDllsCallback)(const char*name, void*user_data);

/*
  List all DLL files listed in the IMPORTS section of a Windows executable (PE) file.
  The callback will be called once for each DLL listed, passing the name of the DLL
  and the user_data pointer (user_data can be anything you wish, or NULL).
  The DLL name might be upper, lower, or even mixed case, and some DLL names might
  even be listed more than once.
  Return value is one of the LISTDLL_* codes above.
*/
int ListDlls(LPSTR exename, ListDllsCallback cb, void*user_data);

#ifdef __cplusplus
}
#endif

