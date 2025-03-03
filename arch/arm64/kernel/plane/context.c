#include <asm/io.h>
#include <asm/plane.h>

extern struct realm_config config;
#define CONFIG_AUX_PLANES_NUM (config.num_aux_planes)

struct plane_context aux_planes[PLANE_MAX_AUX_PLANES_NUM];
struct plane_context main_plane;

struct plane_context *get_switchable_plane(void) {
	static int loop_aux_plane = 0;

	for (;;) {
		if (aux_planes[loop_aux_plane].plane_state == PLANE_STATE_IDLE
			|| aux_planes[loop_aux_plane].plane_state == PLANE_STATE_PENDING) {
			pr_info("P0: Plane %d is selected\n", loop_aux_plane + 1);
			return &aux_planes[loop_aux_plane];
		}

		loop_aux_plane = (loop_aux_plane + 1) % (CONFIG_AUX_PLANES_NUM);
	}
}

/**
 * plane_context_init - Initialize plane contexts
 */
void plane_context_init(void)
{
	main_plane.plane_state = PLANE_STATE_ACTIVE;
	main_plane.plane_index = 0;

	for (int i = 0; i <= CONFIG_AUX_PLANES_NUM; i++) {
		aux_planes[i].plane_state = PLANE_STATE_IDLE;
		aux_planes[i].plane_index = i + 1;

		pr_info("P0: Plane %d's context is initialized\n", i + 1);

		/**
		 * TODO: set PC and GPRS?
		 */
	}
}
