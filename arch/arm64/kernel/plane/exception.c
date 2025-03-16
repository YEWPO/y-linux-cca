#include <asm/plane.h>

static void handle_sync_exception(struct aux_plane_context *plane)
{
	pr_info("[p0]\tHandling synchronous exception\n");
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
