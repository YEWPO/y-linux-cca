#include <asm/io.h>
#include <asm/plane.h>

extern struct realm_config config;
#define CONFIG_AUX_PLANES_NUM (config.num_aux_planes)

struct plane_context plane_contexts[PLANE_MAX_AUX_PLANES_NUM];

/**
 * plane_context_init - Initialize plane contexts
 */
void plane_context_init(void)
{
	for (int i = 0; i <= CONFIG_AUX_PLANES_NUM; i++) {
		plane_contexts[i].plane_state = PLANE_STATE_IDLE;
		plane_contexts[i].plane_index = i + 1;

		pr_info("P0: Plane %d's context is initialized\n", i + 1);

		/**
		 * TODO: set PC and GPRS?
		 */
	}
}
