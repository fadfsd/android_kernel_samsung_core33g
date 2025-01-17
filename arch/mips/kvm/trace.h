/*
* This file is subject to the terms and conditions of the GNU General Public
* License.  See the file "COPYING" in the main directory of this archive
* for more details.
*
* Copyright (C) 2012  MIPS Technologies, Inc.  All rights reserved.
* Authors: Sanjay Lal <sanjayl@kymasys.com>
*/

#if !defined(_TRACE_KVM_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_KVM_H

#include <linux/tracepoint.h>

#undef TRACE_SYSTEM
#define TRACE_SYSTEM kvm
#define TRACE_INCLUDE_PATH .
#define TRACE_INCLUDE_FILE trace

/*
 * Tracepoints for VM eists
 */
extern char *kvm_mips_exit_types_str[MAX_KVM_MIPS_EXIT_TYPES];

TRACE_EVENT(kvm_exit,
	    TP_PROTO(struct kvm_vcpu *vcpu, unsigned int reason),
	    TP_ARGS(vcpu, reason),
	    TP_STRUCT__entry(
<<<<<<< HEAD
			__field(unsigned long, pc)
=======
			__field(struct kvm_vcpu *, vcpu)
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
			__field(unsigned int, reason)
	    ),

	    TP_fast_assign(
<<<<<<< HEAD
			__entry->pc = vcpu->arch.pc;
=======
			__entry->vcpu = vcpu;
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
			__entry->reason = reason;
	    ),

	    TP_printk("[%s]PC: 0x%08lx",
		      kvm_mips_exit_types_str[__entry->reason],
<<<<<<< HEAD
		      __entry->pc)
=======
		      __entry->vcpu->arch.pc)
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource
);

#endif /* _TRACE_KVM_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
