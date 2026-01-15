
#include <bur/plctypes.h>

#ifdef _DEFAULT_INCLUDES
#include <AsDefault.h>
#endif

void increase_counters(void);
void Homing(void);

void _INIT ProgramInit(void)
{
	AxisZ_HW.EncIf1_Typ = ncSDC_ENC16;
	AxisZ_HW.DiDoIf_Typ = ncSDC_DIDO;
	AxisZ_HW.DrvIf_Typ  = ncSDC_DRVSERVO16;

	strcpy(AxisZ_HW.EncIf1_Name, "AxisZ_EncIf");
	strcpy(AxisZ_HW.DrvIf_Name,   "AxisZ_DrvIf");
	strcpy(AxisZ_HW.DiDoIf_Name,  "AxisZ_DiDoIf");

	AxisZ_EncIf.iEncOK        = 1;
	AxisZ_DrvIf.iDrvOK        = 1;
	AxisZ_DrvIf.iStatusEnable = 1;
	AxisZ_DiDoIf.iDriveReady  = 1;

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
		AxisZ_DrvIf.oSetPos = -10000;
	}
	else if (posEnableRight.enable)
	{
		AxisZ_DrvIf.oSetPos = 10000;
	}

	increase_counters();
	AxisZ_EncIf.iActTime = (INT)AsIOTimeCyclicStart();

	AxisZ_DiDoIf.iPosHwEnd = axis_Z.endswitch_a_reached;
	AxisZ_DiDoIf.iNegHwEnd = axis_Z.endswitch_b_reached;


	if (coil_powered == 0)
	{
		coil_pwm_value = 0;

		fb_regulator.e = 0;
		FB_Regulator(&fb_regulator);

		axis_Z.u = 0;
		FB_Axis(&axis_Z);
	}
	else
	{
			increase_counters();

			axis_Z.reset_counter = 0;
			FB_Axis(&axis_Z);

			coil_pwm_value = 32767;

			iPosHwEnd = axis_Z.endswitch_a_reached;
			iNegHwEnd = axis_Z.endswitch_b_reached;
			FB_Axis(&axis_Z);

			fb_regulator.e = (AxisZ_DrvIf.oSetPos * 10) - axis_Z.speed;
			FB_Regulator(&fb_regulator);

			axis_Z.u = fb_regulator.u;
			FB_Axis(&axis_Z);
	}

}

void increase_counters(void)
{
	AxisZ_EncIf.iLifeCnt++;

	AxisZ_DiDoIf.iLifeCntDriveEnable++;
	AxisZ_DiDoIf.iLifeCntDriveReady++;
	AxisZ_DiDoIf.iLifeCntNegHwEnd++;
	AxisZ_DiDoIf.iLifeCntPosHwEnd++;
	AxisZ_DiDoIf.iLifeCntReference++;

	AxisZ_DrvIf.iLifeCnt++;
}

void _EXIT ProgramExit(void)
{
}
