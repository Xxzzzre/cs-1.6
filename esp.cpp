#include "client.h"

deque<playeresp_t> PlayerEsp;
deque<playeresphitboxmulti_t> PlayerEspHitboxMulti[33];

void Box(float x, float y, float w, float h, ImU32 team)
{
	if (!cvar.visual_box) return;
	ImGui::GetCurrentWindow()->DrawList->AddRect({ x, y }, { x + w, y + h - 1 }, team);
}

void Health(int id, float x, float y, float h)
{
	if (!cvar.visual_health) return;
	int hp = g_Player[id].iHealth;
	if (hp < 10)
		hp = 10;
	else if (hp > 100)
		hp = 100;
	if (hp > 99) ImGui::GetCurrentWindow()->DrawList->AddRect({ x - 7, y                    }, { x - 1, y + h / 100.f * 10.f - 1 }, ImColor(0.1f, 1.0f, 0.0f, 1.0f));
	if (hp > 89) ImGui::GetCurrentWindow()->DrawList->AddRect({ x - 7, y + h / 100.f * 10.f }, { x - 1, y + h / 100.f * 20.f - 1 }, ImColor(0.2f, 0.9f, 0.0f, 1.0f));
	if (hp > 79) ImGui::GetCurrentWindow()->DrawList->AddRect({ x - 7, y + h / 100.f * 20.f }, { x - 1, y + h / 100.f * 30.f - 1 }, ImColor(0.3f, 0.8f, 0.0f, 1.0f));
	if (hp > 69) ImGui::GetCurrentWindow()->DrawList->AddRect({ x - 7, y + h / 100.f * 30.f }, { x - 1, y + h / 100.f * 40.f - 1 }, ImColor(0.4f, 0.7f, 0.0f, 1.0f));
	if (hp > 59) ImGui::GetCurrentWindow()->DrawList->AddRect({ x - 7, y + h / 100.f * 40.f }, { x - 1, y + h / 100.f * 50.f - 1 }, ImColor(0.5f, 0.6f, 0.0f, 1.0f));
	if (hp > 49) ImGui::GetCurrentWindow()->DrawList->AddRect({ x - 7, y + h / 100.f * 50.f }, { x - 1, y + h / 100.f * 60.f - 1 }, ImColor(0.6f, 0.5f, 0.0f, 1.0f));
	if (hp > 39) ImGui::GetCurrentWindow()->DrawList->AddRect({ x - 7, y + h / 100.f * 60.f }, { x - 1, y + h / 100.f * 70.f - 1 }, ImColor(0.7f, 0.4f, 0.0f, 1.0f));
	if (hp > 29) ImGui::GetCurrentWindow()->DrawList->AddRect({ x - 7, y + h / 100.f * 70.f }, { x - 1, y + h / 100.f * 80.f - 1 }, ImColor(0.8f, 0.3f, 0.0f, 1.0f));
	if (hp > 19) ImGui::GetCurrentWindow()->DrawList->AddRect({ x - 7, y + h / 100.f * 80.f }, { x - 1, y + h / 100.f * 90.f - 1 }, ImColor(0.9f, 0.2f, 0.0f, 1.0f));
	if (hp > 9)  ImGui::GetCurrentWindow()->DrawList->AddRect({ x - 7, y + h / 100.f * 90.f }, { x - 1, y + h - 1                }, ImColor(1.0f, 0.1f, 0.0f, 1.0f));
}

bool Reload(cl_entity_s* ent, float x, float y, ImU32 team, ImU32 green)
{
	int seqinfo = Cstrike_SequenceInfo[ent->curstate.sequence];
	if (!cvar.visual_reload_bar || seqinfo != 2) return false;
	int label_size = ImGui::CalcTextSize("Reloading", NULL, true).x;
	ImGui::GetCurrentWindow()->DrawList->AddRect({ x - label_size / 2 - 2, y - 15 }, { x + label_size / 2 + 3 , y - 1 }, team);
	ImGui::GetCurrentWindow()->DrawList->AddText({ x - label_size / 2, y - 16 }, green, "Reloading");
	return true;
}

bool Name(int id, float x, float y, ImU32 team, ImU32 white)
{
	if (!cvar.visual_name) return false;
	player_info_s* player = g_Studio.PlayerInfo(id - 1);
	if (!player || !(lstrlenA(player->name) > 0)) return false;
	int label_size = ImGui::CalcTextSize(player->name, NULL, true).x;
	ImGui::GetCurrentWindow()->DrawList->AddRect({ x - label_size / 2 - 2, y - 15 }, { x + label_size / 2 + 3 , y - 1 }, team);
	ImGui::GetCurrentWindow()->DrawList->AddText({ x - label_size / 2, y - 16 }, white, player->name);
	return true;
}

bool Model(int id, float x, float y, ImU32 team, ImU32 white)
{
	if (!cvar.visual_model) return false;
	player_info_s* player = g_Studio.PlayerInfo(id - 1);
	if (!player || !(lstrlenA(player->model) > 0)) return false;
	int label_size = ImGui::CalcTextSize(player->model, NULL, true).x;
	ImGui::GetCurrentWindow()->DrawList->AddRect({ x - label_size / 2 - 2, y - 15 }, { x + label_size / 2 + 3 , y - 1 }, team);
	ImGui::GetCurrentWindow()->DrawList->AddText({ x - label_size / 2, y - 16 }, white, player->model);
	return true;
}

bool Weapon(cl_entity_s* ent, float x, float y, ImU32 team, ImU32 white)
{
	model_s* mdl = g_Studio.GetModelByIndex(ent->curstate.weaponmodel);
	if (!cvar.visual_weapon || !mdl) return false;
	char weapon[256];
	sprintf(weapon, getfilename(mdl->name).c_str() + 2);
	int label_size = ImGui::CalcTextSize(weapon, NULL, true).x;
	ImGui::GetCurrentWindow()->DrawList->AddRect({ x - label_size / 2 - 2, y - 15 }, { x + label_size / 2 + 3 , y - 1 }, team);
	ImGui::GetCurrentWindow()->DrawList->AddText({ x - label_size / 2, y - 16 }, white, weapon);
	return true;
}

void Vip(int id, float x, float y, float w)
{
	if (!cvar.visual_vip || !g_Player[id].bVip) return;
	ImGui::GetCurrentWindow()->DrawList->AddImage((GLuint*)texture_id[VIP], { x, y - w }, { x + w, y });
}

bool bCalcScreen(cl_entity_s* ent, int& x, int& y, int& w, int& h, int& xo)
{
	float vOrigin[2];
	if (!WorldToScreen(ent->origin, vOrigin)) return false;
	xo = vOrigin[0];
	int x0 = vOrigin[0], x1 = vOrigin[0], y0 = vOrigin[1], y1 = vOrigin[1];
	for (playeresphitboxmulti_t Hitbox : PlayerEspHitboxMulti[ent->index])
	{
		for (unsigned int i = 0; i < 8; i++)
		{
			float vHitbox[2];
			if (!WorldToScreen(Hitbox.HitboxMulti[i], vHitbox)) return false;
			x0 = min(x0, vHitbox[0]);
			x1 = max(x1, vHitbox[0]);
			y0 = min(y0, vHitbox[1]);
			y1 = max(y1, vHitbox[1]);
		}
	}
	x = x0;
	y = y0;
	w = x1 - x0 + 2;
	h = y1 - y0 + 2;
	return true;
}

void DrawPlayerEsp()
{
	for (playeresp_t Esp : PlayerEsp)
	{
		if (cvar.visual_idhook_only && idhook.FirstKillPlayer[Esp.ent->index] != 1)
			continue;
		if (!cvar.visual_visual_team && g_Player[Esp.ent->index].iTeam == g_Local.iTeam)
			continue;
		if (!bAlive(Esp.ent))
			continue;
		int x, y, w, h, xo;
		if (bCalcScreen(Esp.ent, x, y, w, h, xo))
		{
			Box(x, y, w, h, Team(Esp.ent->index));
			Health(Esp.ent->index, x, y, h);
			if (Reload(Esp.ent, xo, y, Team(Esp.ent->index), Green()))
				y -= 15;
			if (Name(Esp.ent->index, xo, y, Team(Esp.ent->index), White()))
				y -= 15;
			if (Model(Esp.ent->index, xo, y, Team(Esp.ent->index), White()))
				y -= 15;
			if (Weapon(Esp.ent, xo, y, Team(Esp.ent->index), White()))
				y -= 15;
			Vip(Esp.ent->index, x, y, w);
		}
	}
}

void DrawPlayerSoundIndexEsp()
{
	for (player_sound_index_t sound_index : Sound_Index)
	{
		cl_entity_s* ent = g_Engine.GetEntityByIndex(sound_index.index);
		if (!ent)
			continue;
		if (cvar.visual_idhook_only && idhook.FirstKillPlayer[sound_index.index] != 1)
			continue;
		if (!cvar.visual_visual_team && g_Player[sound_index.index].iTeam == g_Local.iTeam)
			continue;
		if (ent->curstate.messagenum == g_Engine.GetEntityByIndex(pmove->player_index + 1)->curstate.messagenum)
			continue;
		if (sound_index.index == pmove->player_index + 1)
			continue;

		if (cvar.visual_sound_steps)
		{
			float step = M_PI * 2.0f / 15;
			float radius = 13.0f * (1200 - (GetTickCount() - sound_index.timestamp)) / 1200;
			Vector position = Vector(sound_index.origin.x, sound_index.origin.y, sound_index.origin.z - 36);
			for (float i = 0; i < (IM_PI * 2.0f); i += step)
			{
				Vector vPointStart(radius * cosf(i) + position.x, radius * sinf(i) + position.y, position.z);
				Vector vPointEnd(radius * cosf(i + step) + position.x, radius * sinf(i + step) + position.y, position.z);
				float vStart[2], vEnd[2];
				if (WorldToScreen(vPointStart, vStart) && WorldToScreen(vPointEnd, vEnd))
					ImGui::GetCurrentWindow()->DrawList->AddLine(ImVec2(vStart[0], vStart[1]), ImVec2(vEnd[0], vEnd[1]), Team(sound_index.index));
			}
		}
		if (GetTickCount() - sound_index.timestamp >= 300)
			continue;

		Vector vPointTop = Vector(sound_index.origin.x, sound_index.origin.y, sound_index.origin.z + 10);
		Vector vPointBot = Vector(sound_index.origin.x, sound_index.origin.y, sound_index.origin.z - 10);

		float vTop[2], vBot[2];
		if (WorldToScreen(vPointTop, vTop) && WorldToScreen(vPointBot, vBot))
		{
			int h = vBot[1] - vTop[1], w = h, x = vTop[0] - w / 2, y = vTop[1];
			Box(x, y, w, h, Team(sound_index.index));
			Health(sound_index.index, x, y, h);
			if (Name(sound_index.index, x + w / 2, y, Team(sound_index.index), White()))
				y -= 15;
			Vip(sound_index.index, x, y, w);
		}
	}
}

void DrawPlayerSoundNoIndexEsp()
{
	for (player_sound_no_index_t sound_no_index : Sound_No_Index)
	{
		if (cvar.visual_sound_steps)
		{
			float step = IM_PI * 2.0f / 15;
			float radius = 13.0f * (1200 - (GetTickCount() - sound_no_index.timestamp)) / 1200;
			Vector position = Vector(sound_no_index.origin.x, sound_no_index.origin.y, sound_no_index.origin.z - 36);
			for (float i = 0; i < (IM_PI * 2.0f); i += step)
			{
				Vector vPointStart(radius * cosf(i) + position.x, radius * sinf(i) + position.y, position.z);
				Vector vPointEnd(radius * cosf(i + step) + position.x, radius * sinf(i + step) + position.y, position.z);
				float vStart[2], vEnd[2];
				if (WorldToScreen(vPointStart, vStart) && WorldToScreen(vPointEnd, vEnd))
					ImGui::GetCurrentWindow()->DrawList->AddLine(ImVec2(vStart[0], vStart[1]), ImVec2(vEnd[0], vEnd[1]), Green());
			}
		}
		if (GetTickCount() - sound_no_index.timestamp >= 300)
			continue;

		Vector vPointTop = Vector(sound_no_index.origin.x, sound_no_index.origin.y, sound_no_index.origin.z + 10);
		Vector vPointBot = Vector(sound_no_index.origin.x, sound_no_index.origin.y, sound_no_index.origin.z - 10);

		float vTop[2], vBot[2];
		if (WorldToScreen(vPointTop, vTop) && WorldToScreen(vPointBot, vBot))
		{
			int h = vBot[1] - vTop[1], w = h, x = vTop[0] - w / 2, y = vTop[1];
			Box(x, y, w, h, Green());
		}
	}
}