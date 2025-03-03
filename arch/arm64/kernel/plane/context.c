#include <linux/hugetlb.h>

#include <asm/io.h>
#include <asm/plane.h>

extern struct realm_config config;
#define CONFIG_AUX_PLANES_NUM (config.num_aux_planes)

struct plane_context aux_planes[PLANE_MAX_AUX_PLANES_NUM];
struct plane_context main_plane;

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
			pr_info("P0: Plane %d is selected\n", loop_aux_plane + 1);
			return loop_aux_plane + 1;
		}

		loop_aux_plane = (loop_aux_plane + 1) % CONFIG_AUX_PLANES_NUM;
	}
}

/**
 * plane_context_init - Initialize plane contexts
 */
void plane_context_init(void)
{
	main_plane.state = PLANE_STATE_ACTIVE;
	main_plane.index = 0;

	for (int i = 0; i <= CONFIG_AUX_PLANES_NUM; i++) {
		aux_planes[i].index = i + 1;
		aux_planes[i].state = PLANE_STATE_PENDING;
		aux_planes[i].run = (struct plane_run *)get_zeroed_page(GFP_KERNEL);

		pr_info("P0: Plane %d's context is initialized\n", i + 1);
	}
}
