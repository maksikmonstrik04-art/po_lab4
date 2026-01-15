
#include <bur/plctypes.h>

#ifdef _DEFAULT_INCLUDES
#include <AsDefault.h>
#endif

void increase_counters(void);
void Homing(void);

void _INIT ProgramInit(void)
{
	AxisX_HW.EncIf1_Typ = ncSDC_ENC16;
	AxisX_HW.DiDoIf_Typ = ncSDC_DIDO;
	AxisX_HW.DrvIf_Typ  = ncSDC_DRVSERVO16;

	strcpy(AxisX_HW.EncIf1_Name, "AxisX_EncIf");
	strcpy(AxisX_HW.DrvIf_Name,   "AxisX_DrvIf");
	strcpy(AxisX_HW.DiDoIf_Name,  "AxisX_DiDoIf");

	AxisX_EncIf.iEncOK        = 1;
	AxisX_DrvIf.iDrvOK        = 1;
	AxisX_DrvIf.iStatusEnable = 1;
	AxisX_DiDoIf.iDriveReady  = 1;

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
		AxisX_DrvIf.oSetPos = -10000;
	}
	else if (posEnableRight.enable)
	{
		AxisX_DrvIf.oSetPos = 10000;
	}

	increase_counters();
	AxisX_EncIf.iActTime = (INT)AsIOTimeCyclicStart();

	AxisX_DiDoIf.iPosHwEnd = axis_X.endswitch_a_reached;
	AxisX_DiDoIf.iNegHwEnd = axis_X.endswitch_b_reached;


	if (coil_powered == 0)
	{
		coil_pwm_value = 0;

		fb_regulator.e = 0;
		FB_Regulator(&fb_regulator);

		axis_X.u = 0;
		FB_Axis(&axis_X);
	}
	else
	{
			increase_counters();

			axis_X.reset_counter = 0;
			FB_Axis(&axis_X);

			coil_pwm_value = 32767;

			iPosHwEnd = axis_X.endswitch_a_reached;
			iNegHwEnd = axis_X.endswitch_b_reached;
			FB_Axis(&axis_X);

			fb_regulator.e = (AxisX_DrvIf.oSetPos * 10) - axis_X.speed;
			FB_Regulator(&fb_regulator);

			axis_X.u = fb_regulator.u;
			FB_Axis(&axis_X);
	}

}

void increase_counters(void)
{
	AxisX_EncIf.iLifeCnt++;

	AxisX_DiDoIf.iLifeCntDriveEnable++;
	AxisX_DiDoIf.iLifeCntDriveReady++;
	AxisX_DiDoIf.iLifeCntNegHwEnd++;
	AxisX_DiDoIf.iLifeCntPosHwEnd++;
	AxisX_DiDoIf.iLifeCntReference++;

	AxisX_DrvIf.iLifeCnt++;
}

void _EXIT ProgramExit(void)
{
}
