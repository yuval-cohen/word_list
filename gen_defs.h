#ifndef _GEN_DEFS_H_
#define _GEN_DEFS_H_

/* program's return codes */
typedef enum
{
   /* external */
   RC_NO_ERROR,
   RC_BAD_FORMAT,
   RC_NO_MEM,
   RC_FILE_NOT_FOUND,

   /* internal */
   RC_EOF,

} RETURN_CODE;

#endif // _GEN_DEFS_H_
