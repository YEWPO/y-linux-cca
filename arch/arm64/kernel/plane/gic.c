#include <asm/plane.h>

void init_aux_plane_gic(struct gic_sysregs *gic)
{
	gic->gicv3_hcr = 0;
	gic->gicv3_misr = 0;
	gic->gicv3_vmcr = 0;

	for (int i = 0; i < PLANE_GIC_LRS_NUM; i++) {
		gic->gicv3_lrs[i] = 0;
	}
}
