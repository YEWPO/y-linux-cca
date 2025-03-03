#include <linux/hugetlb.h>

#include <asm/io.h>
#include <asm/rsi.h>
#include <asm/plane.h>

static void plane_mainloop(void)
{
	for (;;) {
		int switchable_plane_index = get_switchable_plane();

		/**
		 * TODO: switch to the switchable plane
		 */
		switch_to_aux_plane(switchable_plane_index);
	}
}

void plane_kernel_main(void)
{
	if (!is_realm_world())
		return;

	pr_info("P0: P0 World!\n");

	plane_context_init();

	plane_mainloop();
}
