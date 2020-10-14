#include "client.h"

deque<strafe_t> StrafeDraw;

float FrameCount;
float FpsCount;
float InterpFps;
float PreStrafe;
float JumpOff;

float Keyforwardm = false;
float Keymoveleft = false;
float Keymoveright = false;
float Keyback = false;
float Keyduck = false;
float Keyjump = false;

bool Strafe = false;
bool Fastrun = false;
bool Gstrafe = false;
bool Bhop = false;
bool Jumpbug = false;
bool Cstrafe = false;

Vector vStart, vEnd;
Vector vStartjumppos;
float flJumpdist;
float flJumpmesstime;
bool bJumped;

float YawForVec(float* fwd)
{
	if (fwd[1] == 0 && fwd[0] == 0)
		return 0;
	else
	{
		float yaw = (atan2(fwd[1], fwd[0]) * 180 / M_PI);
		if (yaw < 0)yaw += 360;
		return yaw;
	}
}

void StrafeHack(struct usercmd_s* cmd)
{
	static bool packet = false;
	if (Strafe && !(pmove->flags & FL_ONGROUND) && (pmove->movetype != MOVETYPE_FLY) && !(cmd->buttons & IN_ATTACK) && !(cmd->buttons & IN_ATTACK2 && IsCurWeaponKnife()))
	{
		if (cvar.kz_strafe_silent?!packet:1)
		{
			if(cvar.kz_strafe_silent)bSendpacket(false);
			if (sqrt(POW(pmove->velocity[0]) + POW(pmove->velocity[1])) < 15)
				cmd->forwardmove = 400, cmd->sidemove = 0;

			float dir = 0;
			if (cmd->buttons & IN_MOVERIGHT)
				dir = 90;
			if (cmd->buttons & IN_BACK)
				dir = 180;
			if (cmd->buttons & IN_MOVELEFT)
				dir = -90;

			Vector ViewAngles;
			g_Engine.GetViewAngles(ViewAngles);
			ViewAngles.y += dir;
			float vspeed[3] = { pmove->velocity.x / pmove->velocity.Length(),pmove->velocity.y / pmove->velocity.Length(),0.0f };
			float va_speed = YawForVec(vspeed);
			float adif = va_speed - ViewAngles.y;
			while (adif < -180)adif += 360;
			while (adif > 180)adif -= 360;
			cmd->sidemove = (437.8928) * (adif > 0 ? 1 : -1);
			cmd->forwardmove = 0;
			bool onlysidemove = (abs(adif) >= atan(30.f / sqrt(POW(pmove->velocity[0]) + POW(pmove->velocity[1]))) / M_PI * 180);
			cmd->viewangles[1] -= (-adif);
			float fs = 0;
			if (!onlysidemove)
			{
				static float lv = 0;
				Vector fw = g_Local.vPostForward; fw[2] = 0; fw = fw.Normalize();
				float vel = POW(fw[0] * pmove->velocity[0]) + POW(fw[1] * pmove->velocity[1]);

				fs = lv;
				lv = sqrt(69.f * 100000 / vel);
				static float lastang = 0;
				float ca = abs(adif);
				lastang = ca;
			}

			float ang = atan(fs / cmd->sidemove) / M_PI * 180;
			cmd->viewangles.y += ang;

			float sdmw = cmd->sidemove;
			float fdmw = cmd->forwardmove;

			if (cmd->buttons & IN_MOVERIGHT)
				cmd->forwardmove = -sdmw, cmd->sidemove = fdmw;
			if (cmd->buttons & IN_BACK)
				cmd->forwardmove = -fdmw, cmd->sidemove = -sdmw;
			if (cmd->buttons & IN_MOVELEFT)
				cmd->forwardmove = sdmw, cmd->sidemove = -fdmw;
		}
		if (cvar.kz_strafe_silent)packet = !packet;
	}
	else
		packet = false;
}

void FastRun(struct usercmd_s *cmd)
{
	if(Fastrun && sqrt(POW(pmove->velocity[0]) + POW(pmove->velocity[1])) && pmove->flFallVelocity == 0 && !Gstrafe && pmove->flags&FL_ONGROUND)
	{
		static bool Run = false;
		if((cmd->buttons&IN_FORWARD && cmd->buttons&IN_MOVELEFT) || (cmd->buttons&IN_BACK && cmd->buttons&IN_MOVERIGHT))
		{
			if (Run)
			{
				Run = false;
				cmd->sidemove -= 89.6f;
				cmd->forwardmove -= 89.6f;
			}
			else
			{ 
				Run = true;
				cmd->sidemove += 89.6f; 
				cmd->forwardmove += 89.6f; 
			}
		} 
		else if((cmd->buttons&IN_FORWARD && cmd->buttons&IN_MOVERIGHT) || (cmd->buttons&IN_BACK && cmd->buttons&IN_MOVELEFT))
		{
			if (Run)
			{ 
				Run = false;
				cmd->sidemove -= 89.6f; 
				cmd->forwardmove += 89.6f; 
			}
			else			
			{ 
				Run = true;
				cmd->sidemove += 89.6f; 
				cmd->forwardmove -= 89.6f; 
			}
		} 
		else if(cmd->buttons&IN_FORWARD || cmd->buttons&IN_BACK)
		{
			if (Run)
			{ 
				Run = false;
				cmd->sidemove -= 126.6f; 
			}
			else			
			{ 
				Run = true;
				cmd->sidemove += 126.6f; 
			}
		} 
		else if(cmd->buttons&IN_MOVELEFT || cmd->buttons&IN_MOVERIGHT)
		{
			if (Run)
			{ 
				Run = false;
				cmd->forwardmove -= 126.6f; 
			}
			else			
			{ 
				Run = true;
				cmd->forwardmove += 126.6f; 
			}
		}
	}
}

void GroundStrafe(struct usercmd_s *cmd)
{
	if(Gstrafe && !Jumpbug)
	{
		static int gs_state = 0;
		if(gs_state == 0 && pmove->flags&FL_ONGROUND)
		{
			cmd->buttons |=IN_DUCK;
			gs_state = 1;
		}
		else if(gs_state == 1)
		{
			cmd->buttons &= ~IN_DUCK;
			gs_state = 0;
		}
	}
}

float HeightOrigin()
{
	Vector vTempOrigin = pmove->origin;
	vTempOrigin[2] -= 8192;
	pmtrace_t pTrace;
	g_pEngine->pEventAPI->EV_SetTraceHull((pmove->flags & FL_DUCKING) ? 1 : 0);
	g_pEngine->pEventAPI->EV_PlayerTrace(pmove->origin, vTempOrigin, PM_STUDIO_BOX, -1, &pTrace);

	float flHeightorigin = abs(pTrace.endpos.z - pmove->origin.z);
	Vector endpos = pTrace.endpos;
	g_pEngine->pEventAPI->EV_SetTraceHull((pmove->flags & FL_DUCKING) ? 1 : 0);
	g_pEngine->pEventAPI->EV_PlayerTrace(pmove->origin, endpos, PM_STUDIO_BOX, -1, &pTrace);
	if (pTrace.fraction < 1.0f)//not working
	{
		flHeightorigin = abs(pTrace.endpos.z - pmove->origin.z);

		int i = g_pEngine->pEventAPI->EV_IndexFromTrace(&pTrace);
		if (i > 0 && i <= g_Engine.GetMaxClients())
		{
			cl_entity_s* ent = g_Engine.GetEntityByIndex(i);
			if (ent)
			{
				float dst = pmove->origin.z - (pmove->flags & FL_DUCKING ? 18 : 32) - ent->curstate.origin.z - flHeightorigin;
				if (dst < 30) flHeightorigin -= 14.0;
			}
		}
	}
	return flHeightorigin;
}

inline float EndSpeed(float StartSpeed, float gravity, float frametime, float distance)
{
	while (distance > 0)
	{
		StartSpeed += gravity * frametime;
		float dist = StartSpeed * frametime;
		distance -= dist;
	}
	return StartSpeed;
}

inline float interp(float s1, float s2, float s3, float f1, float f3)
{
	if (s2 == s1)return f1;
	if (s2 == s3)return f3;
	if (s3 == s1)return f1;
	return f1 + ((s2 - s1) / (s3 - s1)) * ((f3 - f1)/*/1*/);
}

float Damage()
{
	Vector start = pmove->origin;
	Vector vForward, vecEnd;
	float va[3];
	g_Engine.GetViewAngles(va);
	g_Engine.pfnAngleVectors(va, vForward, NULL, NULL);
	vecEnd[0] = start[0] + vForward[0] * 8192; vecEnd[1] = start[1] + vForward[1] * 8192; vecEnd[2] = start[2] + vForward[2] * 8192;
	pmtrace_t* trace = g_Engine.PM_TraceLine(start, vecEnd, 1, 2, -1);
	float fDistance = ((start.z) - (trace->endpos.z)) - (pmove->usehull == 0 ? (36) : (18));
	float endSpeed = EndSpeed(pmove->flFallVelocity, 800, 1 / 1000.0f, fDistance);
	if (interp(504.80001f, endSpeed, 1000, 1, 100) > 0)
		return interp(504.80001f, endSpeed, 1000, 1, 100);
	else return 0;
}

float Damage2()
{
	float endSpeed = EndSpeed(pmove->flFallVelocity, 800, 1 / 1000.0f, HeightOrigin());
	if (interp(504.80001f, endSpeed, 1000, 1, 100) > 0)
		return interp(504.80001f, endSpeed, 1000, 1, 100);
	else return 0;
}

void BHop(struct usercmd_s* cmd)
{
	static int bhopcount;
	static bool jumped = false;
	int maxbhop;
	if (cvar.kz_bhop_triple)
		maxbhop = 3;
	else if (cvar.kz_bhop_double)
		maxbhop = 2;
	else
		maxbhop = 1;
	if (Bhop)
	{
		cmd->buttons &= ~IN_JUMP;
		if (pmove->flags & FL_ONGROUND)
		{
			bhopcount = 1;
			cmd->buttons |= IN_JUMP;
		}
		if (maxbhop > 1)
		{
			if (pmove->flFallVelocity < 0)
				jumped = true;
			if (pmove->flFallVelocity > 0)
			{
				if (jumped && bhopcount < maxbhop)
				{
					bhopcount++;
					cmd->buttons |= IN_JUMP;
					jumped = false;
				}
			}
		}
	}
}

double _my_abs(double n) 
{
	if (n >= 0)return n; //if positive, return without ant change
	else return 0 - n; //if negative, return a positive version
}

float GroundAngle()
{
	Vector vTemp1 = pmove->origin;
	vTemp1[2] -= 8192;
	pmtrace_t* trace = g_Engine.PM_TraceLine(pmove->origin, vTemp1, 1, (pmove->flags & FL_DUCKING) ? 1 : 0, -1);

	return acos(trace->plane.normal[2]) / M_PI * 180;
}

void JumpBug(float frametime, struct usercmd_s *cmd)
{
	static int state = 0;

	bool autojb = false;

	if (cvar.kz_jump_bug_auto && pmove->flFallVelocity >= 404.8f)
	{
		if (HeightOrigin() - (pmove->flFallVelocity * frametime / cvar.misc_wav_speed * 15) <= 0)
			autojb = true;
	}

	if ((Jumpbug || autojb) && pmove->flFallVelocity > 0)
	{
		bool curveang = false;
		float fpheight = 0;
		if (GroundAngle() > 1)
		{
			curveang = true;
			Vector vTemp = pmove->origin;
			vTemp[2] -= 8192;
			pmtrace_t* trace = g_Engine.PM_TraceLine(pmove->origin, vTemp, 1, 2, -1);
			fpheight = abs(pmove->origin.z - trace->endpos.z - (pmove->usehull == 1 ? 18.0f : 36.0f));
		}
		else fpheight = HeightOrigin();


		static float last_h = 0.0f;
		float cur_frame_zdist = abs((pmove->flFallVelocity + (800 * frametime)) * frametime);
		cmd->buttons |= IN_DUCK;
		cmd->buttons &= ~IN_JUMP;
		switch (state)
		{
		case 1:
			cmd->buttons &= ~IN_DUCK;
			cmd->buttons |= IN_JUMP;
			state = 2;
			break;
		case 2:
			state = 0;
			break;
		default:
			if (_my_abs(fpheight - cur_frame_zdist * 1.5) <= (20.0) && cur_frame_zdist > 0.0f)
			{
				float needspd = _my_abs(fpheight - (19.0));
				float scale = abs(needspd / cur_frame_zdist);
				AdjustSpeed(scale);
				state = 1;
			}
			break;
		}
		last_h = fpheight;
	}
	else state = 0;
}

inline float EdgeDistance() {
#define TraceEdge(x,y){\
    Vector start=pmove->origin;\
	start[2]-=0.1f;\
	Vector end=start;\
	end[1]+=x*mind;\
	end[0]+=y*mind;\
	pmtrace_s* t1 = g_Engine.PM_TraceLine(end,start,1,pmove->usehull,-1);\
	if(!(t1->startsolid))mind=(t1->endpos-start).Length2D();\
	}
	float mind = 250;
	TraceEdge(-1, 0);
	TraceEdge(1, 0);
	TraceEdge(0, 1);
	TraceEdge(0, -1);
	TraceEdge(-1, -1);
	TraceEdge(1, 1);
	TraceEdge(1, -1);
	TraceEdge(-1, 1);
	return mind;
}

void LongJump()
{
	if (flJumpmesstime > GetTickCount())
	{
		strafe_t Strafe;
		Strafe.Pos1 = vStart;
		Strafe.Pos2 = vEnd;
		StrafeDraw.push_back(Strafe);
	}
}

void Kz(float frametime, struct usercmd_s *cmd)
{
	if (bAliveLocal())
	{
		if (cvar.kz_strafe)
			StrafeHack(cmd);
		if (cvar.kz_fast_run)
			FastRun(cmd);
		if (cvar.kz_ground_strafe)
			GroundStrafe(cmd);
		if (cvar.kz_bhop)
			BHop(cmd);
		if (cvar.kz_jump_bug || cvar.kz_jump_bug_auto)
			JumpBug(frametime, cmd);
		if (cvar.kz_show_kz)
			LongJump();
	}
	if (bJumped && (pmove->flags & FL_ONGROUND || pmove->movetype == MOVETYPE_FLY))
	{
		Vector endpos = pmove->origin;
		endpos.z -= pmove->usehull == 0 ? 36.0 : 18.0;
		vEnd = endpos;
		if (endpos.z == vStartjumppos.z)
		{
			Vector lj = endpos - vStartjumppos;
			float dist = lj.Length() + 32.0625f + 0.003613;
			if (dist >= 200)
			{
				InterpFps = FpsCount / FrameCount;
				FpsCount = 0;
				FrameCount = 0;
				flJumpdist = dist;
				flJumpmesstime = GetTickCount() + (int)cvar.kz_display_time * 1000;
			}
		}
		bJumped = false;
	}
	if (!bJumped && (pmove->flags & FL_ONGROUND) && cmd->buttons & IN_JUMP)
	{
		PreStrafe = sqrt(POW(pmove->velocity[0]) + POW(pmove->velocity[1]));
		if (EdgeDistance() != 250)
			JumpOff = EdgeDistance();
		else JumpOff = 0;
		vStartjumppos = pmove->origin;
		vStartjumppos.z -= pmove->usehull == 0 ? 36.0 : 18.0;
		vStart = vStartjumppos;
		bJumped = true;
	}
	if (cmd->buttons & IN_FORWARD) { Keyforwardm = true; }
	else { Keyforwardm = false; }
	if (cmd->buttons & IN_MOVELEFT) { Keymoveleft = true; }
	else { Keymoveleft = false; }
	if (cmd->buttons & IN_MOVERIGHT) { Keymoveright = true; }
	else { Keymoveright = false; }
	if (cmd->buttons & IN_BACK) { Keyback = true; }
	else { Keyback = false; }
	if (cmd->buttons & IN_DUCK) { Keyduck = true; }
	else { Keyduck = false; }
	if (cmd->buttons & IN_JUMP) { Keyjump = true;; }
	else { Keyjump = false; }
}

void KzFameCount()
{
	if (bJumped)
	{
		FrameCount += 1;
		FpsCount += (1 / pmove->frametime);
	}
}

void DrawLongJump()
{
	for (strafe_t Strafe : StrafeDraw)
	{
		float VecScreenMin[2];
		float VecScreenMax[2];
		if (WorldToScreen(Strafe.Pos1, VecScreenMin) && WorldToScreen(Strafe.Pos2, VecScreenMax))
		{
			int x = VecScreenMin[0], y = VecScreenMin[1], xx = VecScreenMax[0], yy = VecScreenMax[1];
			ImGui::GetCurrentWindow()->DrawList->AddLine({ (float)x, (float)y }, { (float)xx, (float)yy }, Wheel1());
		}

		if (WorldToScreen(Strafe.Pos1, VecScreenMin))
		{
			int x = VecScreenMin[0], y = VecScreenMin[1];
			ImGui::GetCurrentWindow()->DrawList->AddRectFilled({ (float)x - 1, (float)y - 1 }, { (float)x + 2, (float)y + 2 }, Wheel2());
		}

		if (WorldToScreen(Strafe.Pos2, VecScreenMax))
		{
			int x = VecScreenMax[0], y = VecScreenMax[1];
			ImGui::GetCurrentWindow()->DrawList->AddRectFilled({ (float)x - 1, (float)y - 1 }, { (float)x + 2, (float)y + 2 }, Wheel2());
		}

		if (WorldToScreen(Strafe.Pos1, VecScreenMin))
		{
			int label_size = ImGui::CalcTextSize("Start", NULL, true).x;
			int x = VecScreenMin[0], y = VecScreenMin[1];
			ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)x - label_size / 2 - 2, (float)y - 24 }, { (float)x - label_size / 2 + label_size + 3, (float)y - 10 }, Wheel1());
			ImGui::GetCurrentWindow()->DrawList->AddText({ (float)x - label_size / 2, (float)y - 25 }, White(), "Start");
		}

		if (WorldToScreen(Strafe.Pos2, VecScreenMax))
		{
			int label_size = ImGui::CalcTextSize("Stop", NULL, true).x;
			int x = VecScreenMax[0], y = VecScreenMax[1];
			ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)x - label_size / 2 - 2, (float)y - 24 }, { (float)x - label_size / 2 + label_size + 3, (float)y - 10 }, Wheel1());
			ImGui::GetCurrentWindow()->DrawList->AddText({ (float)x - label_size / 2, (float)y - 25 }, White(), "Stop");
		}
	}
}

void DrawKzWindows()
{
	if (cvar.kz_show_kz && bAliveLocal())
	{
		ImVec2 windowpos;
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.25f));
		ImGui::SetNextWindowPos(ImVec2(20, (ImGui::GetIO().DisplaySize.y / 2) - ImGui::GetIO().DisplaySize.y / 20 * 1), ImGuiCond_Once);
		ImGui::Begin("kz", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
		{
			ImVec4 col_default_text = ImGui::GetStyleColorVec4(ImGuiCol_Text);
			ImVec4 col = col_default_text;
			ImVec4 col2 = col_default_text;
			static float fMaxPspeed = 0.f;
			if (sqrt(POW(pmove->velocity[0]) + POW(pmove->velocity[1])) == 0)
				fMaxPspeed = 0.0;
			if (sqrt(POW(pmove->velocity[0]) + POW(pmove->velocity[1])) > fMaxPspeed)
				fMaxPspeed = sqrt(POW(pmove->velocity[0]) + POW(pmove->velocity[1]));
			if (Damage() >= g_Local.iPostHealth)
				col = ImColor(1.f, 0.f, 0.f, 1.0f);
			if (Damage2() >= g_Local.iPostHealth)
				col2 = ImColor(1.f, 0.f, 0.f, 1.0f);

			if (flJumpmesstime > GetTickCount())
			{
				ImGui::Text("Display Time:   %.1f", (flJumpmesstime - GetTickCount()) / 1000);
				ImGui::Separator();
				ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Fps:            %.1f", InterpFps);
				ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Jump Off Edge:  %.1f", JumpOff);
				ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Jump Distance:  %.1f", flJumpdist);
				ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Start speed:    %.1f", PreStrafe);
			}
			ImGui::TextColored(col, "Damage Predict: %.1f", Damage());
			ImGui::TextColored(col2, "Damage In Fall: %.1f", Damage2());
			ImGui::Text("Height:         %.1f", HeightOrigin());
			ImGui::Text("Ground Angle:   %.1f", GroundAngle());
			ImGui::Text("Speed:          %.1f", sqrt(POW(pmove->velocity[0]) + POW(pmove->velocity[1])));
			ImGui::Text("Speed Max:      %.1f", fMaxPspeed);
			ImGui::Text("Speed In Fall:  %.1f", pmove->flFallVelocity);
			ImGui::TextColored(ImVec4(1.f, 0.f, EdgeDistance(), 1.f), "Edge Distance:  %.1f", EdgeDistance());

			windowpos.x = ImGui::GetWindowPos().x + ImGui::GetWindowSize().x + 5;
			windowpos.y = ImGui::GetWindowPos().y;
		}
		ImGui::PopStyleColor();
		ImGui::End();

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.25f));
		ImGui::SetNextWindowPos(ImVec2(windowpos.x, windowpos.y), ImGuiCond_Always);
		if (ImGui::Begin("kzkeys", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("%s", Keyforwardm ? "  W" : "  |");
			ImGui::Text("%s %s %s", Keymoveleft ? "A" : "-", Keyback ? "S" : "+", Keymoveright ? "D" : "-");
			ImGui::Text(Keyduck ? " Duck" : "  |");
			ImGui::Text(Keyjump ? " Jump" : "  |");
		}
		ImGui::PopStyleColor();
		ImGui::End();
	}
}