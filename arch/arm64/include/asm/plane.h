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

#define PLANE_MAX_AUX_PLANES_NUM	3

/* Planes Context */
struct aux_plane_context {
	u64 state;
	u64 index;
	struct plane_run *run;

	u64 pc;
	u64 gprs[PLANE_GPRS_NUM];
	u64 pstate;
};

/* Kernel */
void plane_kernel_main(void);

/* Context */
void plane_context_init(void);
int get_switchable_plane(void);
void switch_to_aux_plane(int plane_index);

/* Exception */
bool handle_aux_plane_exception(struct aux_plane_context *plane);
bool handle_aux_plane_undef_exception(struct aux_plane_context *plane);
void skip_pc(struct aux_plane_context *plane);

#endif // __ASM_PLANES_H_
