#ifndef __ASM_PLANES_H
#define __ASM_PLANES_H

/* Planes State */
#define PLANE_STATE_IDLE		0
#define PLANE_STATE_PENDING		1
#define PLANE_STATE_ACTIVE		2
#define PLANE_STATE_ABORT		3
#define PLANE_STATE_STOPPED		4

#define PLANE_GPRS_NUM			31

#define PLANE_MAX_AUX_PLANES_NUM	3

/* Planes Context */
struct plane_context {
	u64 plane_state;
	u64 plane_index;

	u64 pc;
	u64 gprs[PLANE_GPRS_NUM];
};

/* Kernel */
void plane_kernel_main(void);

/* Context */
void plane_context_init(void);
struct plane_context *get_switchable_plane(void);

#endif // __ASM_PLANES_H_
