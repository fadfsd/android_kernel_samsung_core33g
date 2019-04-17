#ifndef _NF_DEFRAG_IPV6_H
#define _NF_DEFRAG_IPV6_H

extern void nf_defrag_ipv6_enable(void);

extern int nf_ct_frag6_init(void);
extern void nf_ct_frag6_cleanup(void);
extern struct sk_buff *nf_ct_frag6_gather(struct sk_buff *skb, u32 user);
<<<<<<< HEAD
extern void nf_ct_frag6_consume_orig(struct sk_buff *skb);
=======
extern void nf_ct_frag6_output(unsigned int hooknum, struct sk_buff *skb,
			       struct net_device *in,
			       struct net_device *out,
			       int (*okfn)(struct sk_buff *));
>>>>>>> a8f179a4cb19... core33g: Import SM-T113NU_SEA_KK_Opensource

struct inet_frags_ctl;

#endif /* _NF_DEFRAG_IPV6_H */
