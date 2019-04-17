#ifndef __LINUX_COMPILER_H
#error "Please don't include <linux/compiler-intel.h> directly, include <linux/compiler.h> instead."
#endif

#ifdef __ECC

/* Some compiler specific definitions are overwritten here
 * for Intel ECC compiler
 */

#include <asm/intrinsics.h>

/* Intel ECC compiler doesn't support gcc specific asm stmts.
 * It uses intrinsics to do the equivalent things.
 */
#undef barrier
#undef RELOC_HIDE
<<<<<<< HEAD
#undef OPTIMIZER_HIDE_VAR
=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

#define barrier() __memory_barrier()

#define RELOC_HIDE(ptr, off)					\
  ({ unsigned long __ptr;					\
     __ptr = (unsigned long) (ptr);				\
    (typeof(ptr)) (__ptr + (off)); })

<<<<<<< HEAD
/* This should act as an optimization barrier on var.
 * Given that this compiler does not have inline assembly, a compiler barrier
 * is the best we can do.
 */
#define OPTIMIZER_HIDE_VAR(var) barrier()

=======
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
/* Intel ECC compiler doesn't support __builtin_types_compatible_p() */
#define __must_be_array(a) 0

#endif

<<<<<<< HEAD
=======
#define uninitialized_var(x) x

>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
#ifndef __HAVE_BUILTIN_BSWAP16__
/* icc has this, but it's called _bswap16 */
#define __HAVE_BUILTIN_BSWAP16__
#define __builtin_bswap16 _bswap16
#endif

