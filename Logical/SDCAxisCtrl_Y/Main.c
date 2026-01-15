
#include <bur/plctypes.h>

#ifdef _DEFAULT_INCLUDES
#include <AsDefault.h>
#endif

void increase_counters(void);
void Homing(void);

void _INIT ProgramInit(void)
{
	AxisY_HW.EncIf1_Typ = ncSDC_ENC16;
	AxisY_HW.DiDoIf_Typ = ncSDC_DIDO;
	AxisY_HW.DrvIf_Typ  = ncSDC_DRVSERVO16;

	strcpy(AxisY_HW.EncIf1_Name, "AxisY_EncIf");
	strcpy(AxisY_HW.DrvIf_Name,   "AxisY_DrvIf");
	strcpy(AxisY_HW.DiDoIf_Name,  "AxisY_DiDoIf");

	AxisY_EncIf.iEncOK        = 1;
	AxisY_DrvIf.iDrvOK        = 1;
	AxisY_DrvIf.iStatusEnable = 1;
	AxisY_DiDoIf.iDriveReady  = 1;

	fb_regulator.dt            = 0.002;
	fb_regulator.k_i           = 0.16;
	fb_regulator.k_p           = 0.0064;
	fb_regulator.max_abc_value = 24.0;

	pwm_period = 200;
}

void _CYCLIC ProgramCyclic(void)
{
	if (posEnable.enable)
	{
		AxisY_DrvIf.oSetPos = -10000;
	}
	else if (posEnableRight.enable)
	{
		AxisY_DrvIf.oSetPos = 10000;
	}

	increase_counters();
	AxisY_EncIf.iActTime = (INT)AsIOTimeCyclicStart();

	AxisY_DiDoIf.iPosHwEnd = axis_Y.endswitch_a_reached;
	AxisY_DiDoIf.iNegHwEnd = axis_Y.endswitch_b_reached;


	if (coil_powered == 0)
	{
		coil_pwm_value = 0;

		fb_regulator.e = 0;
		FB_Regulator(&fb_regulator);

		axis_Y.u = 0;
		FB_Axis(&axis_Y);
	}
	else
	{
			increase_counters();

			axis_Y.reset_counter = 0;
			FB_Axis(&axis_Y);

			coil_pwm_value = 32767;

			iPosHwEnd = axis_Y.endswitch_a_reached;
			iNegHwEnd = axis_Y.endswitch_b_reached;
			FB_Axis(&axis_Y);

			fb_regulator.e = (AxisY_DrvIf.oSetPos * 10) - axis_Y.speed;
			FB_Regulator(&fb_regulator);

			axis_Y.u = fb_regulator.u;
			FB_Axis(&axis_Y);
	}

}

void increase_counters(void)
{
	AxisY_EncIf.iLifeCnt++;

	AxisY_DiDoIf.iLifeCntDriveEnable++;
	AxisY_DiDoIf.iLifeCntDriveReady++;
	AxisY_DiDoIf.iLifeCntNegHwEnd++;
	AxisY_DiDoIf.iLifeCntPosHwEnd++;
	AxisY_DiDoIf.iLifeCntReference++;

	AxisY_DrvIf.iLifeCnt++;
}

void _EXIT ProgramExit(void)
{
}
