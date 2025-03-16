#include <linux/hugetlb.h>

#include <asm/plane.h>

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
	memcpy(plane->gprs, run->exit.gprs, sizeof(run->exit.gprs));

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

	/* Restore the aux plane context */
	restore_aux_plane_context(plane_index);

	/* Enter the aux plane */
	rsi_plane_enter(plane_index, virt_to_phys(run));

	/* Save the aux plane context */
	save_aux_plane_context(plane_index);

	handle_aux_plane_exception(plane);
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
		if (aux_planes[loop_aux_plane].state == PLANE_STATE_PENDING) {
			pr_info("[p0]\tPlane %d is selected\n", ARR_TO_PLANE_INDEX(loop_aux_plane));
			return ARR_TO_PLANE_INDEX(loop_aux_plane);
		}

		loop_aux_plane = (loop_aux_plane + 1) % CONFIG_AUX_PLANES_NUM;
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

		pr_info("[p0]\tPlane %d's context is initialized\n", ARR_TO_PLANE_INDEX(i));
	}
}
