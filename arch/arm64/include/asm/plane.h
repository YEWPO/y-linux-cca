#ifndef __ASM_PLANES_H
#define __ASM_PLANES_H

#include <asm/io.h>

/* Planes State */
#define PLANE_STATE_IDLE		0
#define PLANE_STATE_PENDING		1
#define PLANE_STATE_ACTIVE		2
#define PLANE_STATE_ABORT		3
#define PLANE_STATE_STOPPED		4

#define PLANE_GPRS_NUM			31
#define PLANE_GIC_LRS_NUM		16

#define PLANE_MAX_AUX_PLANES_NUM	3

/* Planes GIC */
struct gic_sysregs {
	u64 gicv3_hcr;
	u64 gicv3_lrs[PLANE_GIC_LRS_NUM];
	u64 gicv3_misr;
	u64 gicv3_vmcr;
};

/* Planes Timer */
struct timer_sysregs {
	u64 cntp_ctl;
	u64 cntp_cval;
	u64 cntv_ctl;
	u64 cntv_cval;
};

/* Planes Context */
struct aux_plane_context {
	u64 state;
	u64 index;
	struct plane_run *run;

	u64 pc;
	u64 gprs[PLANE_GPRS_NUM];
	u64 pstate;

	struct gic_sysregs gic;
	struct timer_sysregs timer;
};

/* Kernel */
void plane_kernel_main(void);

/* Context */
void plane_context_init(void);
int get_switchable_plane(void);
void switch_to_aux_plane(int plane_index);

/* Exception */
bool handle_aux_plane_exception(struct aux_plane_context *plane);

/* Sysreg */
bool handle_aux_plane_sys64_exception(struct aux_plane_context *plane);

/* GIC */
void init_aux_plane_gic(struct gic_sysregs *gic);

/* Timer */
void init_aux_plane_timer(struct timer_sysregs *timer);

#endif // __ASM_PLANES_H_
