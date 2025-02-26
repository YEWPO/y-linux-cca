#include <linux/hugetlb.h>

#include <asm/io.h>
#include <asm/rsi.h>
#include <asm/p0.h>

void p0_main(void)
{
	if (!is_realm_world())
		return;

	struct plane_run *run = (struct plane_run *)get_zeroed_page(GFP_KERNEL);

	rsi_plane_enter(1, virt_to_phys(run));
}
