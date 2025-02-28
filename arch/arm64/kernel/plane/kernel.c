#include <linux/hugetlb.h>

#include <asm/io.h>
#include <asm/rsi.h>
#include <asm/plane.h>

void plane_kernel_main(void)
{
	if (!is_realm_world())
		return;

	pr_info("P0: P0 World!\n");

	plane_context_init();
}
