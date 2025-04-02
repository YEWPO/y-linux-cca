#include <asm/plane.h>

void init_aux_plane_timer(struct timer_sysregs *timer)
{
	timer->cntp_ctl = 0;
	timer->cntp_cval = 0;
	timer->cntv_ctl = 0;
	timer->cntv_cval = 0;
}
