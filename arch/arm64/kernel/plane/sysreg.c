#include <asm/plane.h>
#include <asm/esr.h>
#include <asm/sysreg-defs.h>

bool handle_aux_plane_sys64_exception(struct aux_plane_context *plane)
{
	struct plane_exit *plane_exit = &plane->run->exit;

	unsigned long esr = plane_exit->esr_el2;

	/* rt = esr[9:5] */
	unsigned int rt = ESR_ELx_SYS64_ISS_RT(esr);
	WARN_ON(rt == 31U);
	/*
	 * op0 = esr[21:20]
	 * op2 = esr[19:17]
	 * op1 = esr[16:14]
	 * crn = esr[13:10]
	 * crm = esr[4:1]
	 */
	unsigned int sysreg = esr_sys64_to_sysreg(esr);

	if ((esr & ESR_ELx_SYS64_ISS_EL0_CACHE_OP_MASK) == ESR_ELx_SYS64_ISS_EL0_CACHE_OP_VAL) {
		pr_info("[p0]\tsys64 exception el0 cache op\n");
		return false;
	}

	if ((esr & ESR_ELx_SYS64_ISS_SYS_MRS_OP_MASK) == ESR_ELx_SYS64_ISS_SYS_MRS_OP_VAL) {
		pr_info("[p0]\tsys64 exception sys mrs op\n");

		unsigned long value;
		rsi_plane_sysreg_read(plane->index, sys_reg_to_rsi_sys_reg(sysreg), &value, (void *)0);

		plane->gprs[rt] = value;
		pr_info("[p0]\tHandled sys mrs op, write sysreg 0x%x's value 0x%016lx to gprs[%d]\n", sysreg, value, rt);

		return true;
	}

	unsigned int dir = esr & ESR_ELx_SYS64_ISS_DIR_MASK;
	unsigned int op0 = sys_reg_Op0(sysreg);
	unsigned int op1 = sys_reg_Op1(sysreg);
	unsigned int crn = sys_reg_CRn(sysreg);
	unsigned int crm = sys_reg_CRm(sysreg);
	unsigned int op2 = sys_reg_Op2(sysreg);

	if (dir == ESR_ELx_SYS64_ISS_DIR_READ) {
		pr_info("[p0]\tUnhandled sysreg Op0 %d Op1 %d CRn %d CRm %d Op2 %d read, target rt %d\n",
				op0, op1, crn, crm, op2, rt);
		plane->gprs[rt] = 0UL;
	} else {
		pr_info("[p0]\tUnhandled sysreg Op0 %d Op1 %d CRn %d CRm %d Op2 %d write, target rt %d, write value is 0x%016llx\n",
				op0, op1, crn, crm, op2, rt, plane->gprs[rt]);
	}

	return true;
}
