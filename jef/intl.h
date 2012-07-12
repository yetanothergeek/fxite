#undef _

#ifdef ENABLE_NLS
# include <libintl.h>
# define _(S) gettext(S)
#else
# define _
#endif
