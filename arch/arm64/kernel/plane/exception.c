#include <asm/esr.h>
#include <asm/plane.h>

static void handle_sync_exception(struct aux_plane_context *plane)
{
	pr_info("[p0]\tHandling synchronous exception\n");

	struct plane_exit *plane_exit = &plane->run->exit;

	unsigned long elr = plane_exit->elr_el2;
	unsigned long esr = plane_exit->esr_el2;
	unsigned long far = plane_exit->far_el2;
	unsigned long hpfar = plane_exit->hpfar_el2;

	pr_info("[p0]\tELR_EL2: 0x%lx\tESR_EL2: 0x%lx\tFAR_EL2: 0x%lx\tHPFAR_EL2: 0x%lx\n",
		elr, esr, far, hpfar);

	switch (ESR_ELx_EC(esr)) {
		case ESR_ELx_EC_WFx:
			pr_info("[p0]\tWFX exception\n");
			break;
		case ESR_ELx_EC_DABT_LOW:
			pr_info("[p0]\tData abort exception\n");
			break;
		case ESR_ELx_EC_IABT_LOW:
			pr_info("[p0]\tInstruction abort exception\n");
			break;
		case ESR_ELx_EC_HVC64:
			pr_info("[p0]\tHVC64 exception\n");
			break;
		case ESR_ELx_EC_SMC64:
			pr_info("[p0]\tSMC64 exception\n");
			break;
		default:
			pr_info("[p0]\tUnknown exception\n");
			break;
	}
}

static void handle_irq_exception(struct aux_plane_context *plane)
{
	pr_info("[p0]\tHandling IRQ exception\n");
}

static void handle_host_exception(struct aux_plane_context *plane)
{
	pr_info("[p0]\tHandling host exception\n");
}

/**
 * handle_aux_plane_exception - Handle the exception for the auxiliary plane
 *
 * @plane: The auxiliary plane context
 */
void handle_aux_plane_exception(struct aux_plane_context *plane)
{
	int plane_index = plane->index;
	struct plane_exit *plane_exit = &plane->run->exit;

	pr_info("[p0]\tHandling exception for plane %d\n", plane_index);
	WARN_ON(plane->state != PLANE_STATE_STOPPED);
	WARN_ON(plane_exit == NULL);

	switch (plane_exit->reason) {
	case RSI_EXIT_SYNC:
		handle_sync_exception(plane);
		break;
	case RSI_EXIT_IRQ:
		handle_irq_exception(plane);
		break;
	case RSI_EXIT_HOST:
		handle_host_exception(plane);
		break;
	default:
		pr_err("[p0]\tPlane %d exited with unknown reason\n", plane_index);
		break;
	}

	for (;;);
}
