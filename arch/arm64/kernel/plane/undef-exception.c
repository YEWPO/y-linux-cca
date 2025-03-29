#include <asm/plane.h>
#include <asm/esr.h>

static bool handle_aux_plane_sys64(struct aux_plane_context *plane)
{
	struct plane_exit *plane_exit = &plane->run->exit;

	unsigned long esr = plane_exit->esr_el2;

	/* rt = esr[9:5] */
	unsigned int rt = ESR_ELx_SYS64_ISS_RT(esr);
	/*
	 * op0 = esr[21:20]
	 * op2 = esr[19:17]
	 * op1 = esr[16:14]
	 * crn = esr[13:10]
	 * crm = esr[4:1]
	 * dir = esr[0]
	 */
	unsigned int sys_op = esr & ESR_ELx_SYS64_ISS_SYS_OP_MASK;

	if ((esr & ESR_ELx_SYS64_ISS_EL0_CACHE_OP_MASK) == ESR_ELx_SYS64_ISS_EL0_CACHE_OP_VAL) {
		pr_info("[p0]\tsys64 exception el0 cache op\n");
		return false;
	}

	if ((esr & ESR_ELx_SYS64_ISS_SYS_MRS_OP_MASK) == ESR_ELx_SYS64_ISS_SYS_MRS_OP_VAL) {
		pr_info("[p0]\tsys64 exception sys mrs op\n");
		return false;
	}

	unsigned int dir = esr & ESR_ELx_SYS64_ISS_DIR_MASK;

	if (dir == ESR_ELx_SYS64_ISS_DIR_READ) {
		pr_info("[p0]\tUnhandled sys_op 0x%x read, target rt %d\n", sys_op, rt);
	} else {
		pr_info("[p0]\tUnhandled sys_op 0x%x write, target rt %d, write value is 0x%016llx\n",
				sys_op, rt, plane->gprs[rt]);
	}

	skip_pc(plane);

	return true;
}

bool handle_aux_plane_undef_exception(struct aux_plane_context *plane)
{
	pr_info("[p0]\tHandle undefined exceptions\n");

	struct plane_exit *plane_exit = &plane->run->exit;

	unsigned long esr = plane_exit->esr_el2;
	bool ret = false;

	switch (ESR_ELx_EC(esr)) {
		case ESR_ELx_EC_SYS64:
			ret = handle_aux_plane_sys64(plane);
			break;
		default:
			pr_err("[p0]\tUnhandled undefined exception\n");
	}

	return ret;
}
