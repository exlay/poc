#include <sys/types.h>

#define EXLAY_PROG ((ulong)0x20000001)
#define EXLAY_VER ((ulong)1)

#define EX_CREATE_STACK_PROC	((u_long)1)
#define EX_SET_BINDING_PROC		((u_long)2)
#define EX_SET_REMOTE_PROC		((u_long)3)
#define EX_SET_BIND_STACK_PROC	((u_long)4)
#define EX_LISTEN_STACK_PROC	((u_long)5)
#define EX_DIAL_STACK_PROC		((u_long)6)
#define EX_SEND_STACK_PROC		((u_long)7)
#define EX_RECV_STACK_PROC		((u_long)8)
#define EX_CLOSE_STACK_PROC		((u_long)9)
