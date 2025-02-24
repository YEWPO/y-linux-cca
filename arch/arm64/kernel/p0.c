#include <asm/io.h>
#include <asm/rsi.h>
#include <asm/p0.h>

void p0_main(void)
{
	if (!is_realm_world())
		return;

	rsi_plane_enter(1, 0);
}
