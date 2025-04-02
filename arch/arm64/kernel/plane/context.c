#include <linux/hugetlb.h>

#include <asm/plane.h>

#include <uapi/asm/ptrace.h>

extern struct realm_config config;
#define CONFIG_AUX_PLANES_NUM (config.num_aux_planes)

struct aux_plane_context aux_planes[PLANE_MAX_AUX_PLANES_NUM];
#define ARR_TO_PLANE_INDEX(x) ((x) + 1)
#define PLANE_TO_ARR_INDEX(x) ((x) - 1)

static void save_aux_plane_context(int plane_index)
{
	WARN_ON(plane_index < 1 || plane_index > CONFIG_AUX_PLANES_NUM);

	struct aux_plane_context *plane = &aux_planes[PLANE_TO_ARR_INDEX(plane_index)];
	struct plane_run *run = plane->run;

	WARN_ON(plane->state != PLANE_STATE_ACTIVE);

	memset(&run->enter, 0, sizeof(run->enter));

	/* Save the aux plane context */
	plane->pc = run->exit.elr_el2;
	plane->pstate = run->exit.spsr_el2;
	memcpy(plane->gprs, run->exit.gprs, sizeof(run->exit.gprs));

	/* Save the aux plane GIC context */
	plane->gic.gicv3_hcr = run->exit.gicv3_hcr;
	plane->gic.gicv3_misr = run->exit.gicv3_misr;
	memcpy(plane->gic.gicv3_lrs, run->exit.gicv3_lrs, sizeof(run->exit.gicv3_lrs));
	plane->gic.gicv3_vmcr = run->exit.gicv3_vmcr;

	/* Save the aux plane timer context */
	plane->timer.cntp_ctl = run->exit.cntp_ctl;
	plane->timer.cntp_cval = run->exit.cntp_cval;
	plane->timer.cntv_ctl = run->exit.cntv_ctl;
	plane->timer.cntv_cval = run->exit.cntv_cval;

	plane->state = PLANE_STATE_STOPPED;
}

static void restore_aux_plane_context(int plane_index)
{
	WARN_ON(plane_index < 1 || plane_index > CONFIG_AUX_PLANES_NUM);

	struct aux_plane_context *plane = &aux_planes[PLANE_TO_ARR_INDEX(plane_index)];
	struct plane_run *run = plane->run;

	WARN_ON(plane->state != PLANE_STATE_PENDING);

	memset(&run->exit, 0, sizeof(run->exit));

	/* Restore the aux plane context */
	run->enter.pc = plane->pc;
	run->enter.spsr_el2 = plane->pstate;
	memcpy(run->enter.gprs, plane->gprs, sizeof(run->enter.gprs));

	plane->state = PLANE_STATE_ACTIVE;
}

/**
 * switch_to_aux_plane - Switch to the auxiliary plane
 *
 * @plane_index: The auxiliary plane index
 */
void switch_to_aux_plane(int plane_index)
{
	struct aux_plane_context *plane = &aux_planes[PLANE_TO_ARR_INDEX(plane_index)];
	int state = plane->state;
	struct plane_run *run = plane->run;

	WARN_ON(plane_index < 1 || plane_index > CONFIG_AUX_PLANES_NUM);
	WARN_ON(state != PLANE_STATE_PENDING);

	/* Check if the aux plane is pending */
	if (check_aux_plane_timer_pending(&plane->timer)) {
		pr_info("[p0]\tP%d's timer is pending\n", plane_index);
		for (;;) {
			static int dead_loop = 0;
			dead_loop++;
			if (dead_loop % 10000000 == 0) {
				pr_info("[p0]\tDead loop %d\n", dead_loop / 10000000);
			}
		}
	}

	/* Restore the aux plane context */
	restore_aux_plane_context(plane_index);

	/* Enter the aux plane */
	rsi_plane_enter(plane_index, virt_to_phys(run));

	/* Save the aux plane context */
	save_aux_plane_context(plane_index);

	/* Handle the aux plane exit */
	if (handle_aux_plane_exception(plane)) {
		plane->state = PLANE_STATE_PENDING;
		pr_info("[p0]\tHandled P%d's exception %d\n", plane_index, run->exit.reason);
	} else {
		plane->state = PLANE_STATE_ABORT;
		pr_info("[p0]\tUnhandled P%d's exception %d\n", plane_index, run->exit.reason);

		for(;;) {
			static int dead_loop = 0;
			dead_loop++;
			if (dead_loop % 10000000 == 0) {
				pr_info("[p0]\tDead loop %d\n", dead_loop / 10000000);
			}
		}
	}
}

/**
 * get_switchable_plane - Get the switchable plane
 *
 * Return: The switchable plane index
 */
int get_switchable_plane(void)
{
	static int loop_aux_plane = 0;

	for (;;) {
		loop_aux_plane = (loop_aux_plane + 1) % CONFIG_AUX_PLANES_NUM;

		if (aux_planes[loop_aux_plane].state == PLANE_STATE_PENDING) {
			pr_info("[p0]\tPlane %d is selected\n", ARR_TO_PLANE_INDEX(loop_aux_plane));
			return ARR_TO_PLANE_INDEX(loop_aux_plane);
		}
	}
}

/**
 * plane_context_init - Initialize plane contexts
 */
void plane_context_init(void)
{
	for (int i = 0; i < CONFIG_AUX_PLANES_NUM; i++) {
		aux_planes[i].index = ARR_TO_PLANE_INDEX(i);
		aux_planes[i].state = PLANE_STATE_PENDING;
		aux_planes[i].run = (struct plane_run *)get_zeroed_page(GFP_KERNEL);

		aux_planes[i].pc = 0x80000000;
		aux_planes[i].gprs[0] = 0x8fe00000;
		aux_planes[i].pstate = PSR_MODE_EL1h
					| PSR_F_BIT | PSR_I_BIT | PSR_A_BIT | PSR_D_BIT;

		init_aux_plane_gic(&aux_planes[i].gic);
		init_aux_plane_timer(&aux_planes[i].timer);

		pr_info("[p0]\tPlane %d's context is initialized\n", ARR_TO_PLANE_INDEX(i));
	}
}
