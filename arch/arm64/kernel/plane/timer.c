#include <asm/plane.h>

#define CNTx_CTL_ENABLE			(1 << 0)
#define CNTx_CTL_IMASK			(1 << 1)
#define CNTx_CTL_ISTATUS		(1 << 2)

#define TIMER_ASSERTED(ctl) \
	(((ctl) & CNTx_CTL_ENABLE) && \
	!((ctl) & CNTx_CTL_IMASK) && \
	((ctl) & CNTx_CTL_ISTATUS))

bool check_aux_plane_timer_pending(struct timer_sysregs *timer)
{
	if (TIMER_ASSERTED(timer->cntp_ctl) || TIMER_ASSERTED(timer->cntv_ctl)) {
		return true;
	}

	return false;
}

void init_aux_plane_timer(struct timer_sysregs *timer)
{
	timer->cntp_ctl = 0;
	timer->cntp_cval = 0;
	timer->cntv_ctl = 0;
	timer->cntv_cval = 0;
}
