#define _CRT_SECURE_NO_WARNINGS
#include "Screen.h"
#include "BaseObject.h"
#include "Menu.h"
#include "Monster.h"
#include "Machine.h"
#include "CommonFunction.h"
#include "User.h"
#include "Music.h"
#define DEBUG_PATH false
#define DEBUG_TOWER false

Screen* BaseObject::MainScreen = nullptr;
Music* BaseObject::musicControl = nullptr;
int Time::time = 0;
int main(int argc, char* argv[])     
{
	if(displayMenu() == 0){
		return 0;
	}
	 beginGame:
	// -------------------- Music init --------------------
	BaseObject::musicControl = new Music;
	BaseObject::musicControl->Init();
	BaseObject::musicControl->PlaybackgroundMusic();
	// -------------------- Game init --------------------
	BaseObject::MainScreen = new Screen;
	BaseObject::MainScreen->InitData();
	bool is_quit = false;
	Time::time = 0;
	// -------------------- Background init --------------------
	BaseObject g_background;
	g_background.LoadImg("map.png");
	// -------------------- Text init --------------------
	TextObject Print_text;
	TextObject Money_text;
	Money_text.SetColor(TextObject::WHITE_TEXT);
	Print_text.SetColor(TextObject::RED_TEXT);
	// -------------------- Tower init --------------------
	Tower tow(1000);
	// -------------------- Bullet init --------------------
	BulletList *bulletList = new BulletList;
	// -------------------- Machine init --------------------
	MachineList *machineList = new MachineList;
	//---------------------- User ------------------------
	User *player = new User(1000);
	// -------------------- Monster init --------------------
	MonsterList *monsterList = new MonsterList(20); //Monster(int _HP, int _speed, int _spawnTime, string _file_path)
	// -------------------- Path init --------------------
	vector<Point> pathBorder;
	vector<Point> path;
	getPath(path, pathBorder, machineList->pathRect);
	// -------------------- Start Game --------------------
	int wait_time = 100;
	while (!is_quit) {
		// -------------------- Victory --------------------
		if (monsterList->allDied == true)
		{
			if(wait_time == 100)
			{
				BaseObject::musicControl->PlaySoundEffect("win_game");
				g_background.LoadImg("Victory Screen.png");
				wait_time--;
			}
			
			if (BaseObject::MainScreen->g_event.type == SDL_QUIT) {
				is_quit = true;
			}
			g_background.Render(NULL);
			SDL_RenderPresent(BaseObject::MainScreen->g_screen);
			SDL_Delay(100);
			
			while (SDL_PollEvent(&BaseObject::MainScreen->g_event) != 0) {
				if (BaseObject::MainScreen->g_event.type == SDL_KEYDOWN) {
					if (BaseObject::MainScreen->g_event.key.keysym.sym == SDLK_r) {
						g_background.Free();
						BaseObject::MainScreen->close();
						BaseObject::musicControl->Free();
						goto beginGame;
					};
				}
			}
			continue;
		}
		// -------------------- Defeated --------------------
		if (!tow.isLife()) {
			if(wait_time == 100)
			{
				BaseObject::musicControl->PlaySoundEffect("lose_game");
				g_background.LoadImg("Defeated Screen.png");
				wait_time--;
			}
			if (BaseObject::MainScreen->g_event.type == SDL_QUIT) {
				is_quit = true;
			}
			g_background.Render(NULL);
			SDL_RenderPresent(BaseObject::MainScreen->g_screen);
			SDL_Delay(100);
			
			while (SDL_PollEvent(&BaseObject::MainScreen->g_event) != 0) {
				if (BaseObject::MainScreen->g_event.type == SDL_KEYDOWN) {
					if (BaseObject::MainScreen->g_event.key.keysym.sym == SDLK_r) {
						g_background.Free();
						BaseObject::MainScreen->close();
						BaseObject::musicControl->Free();
						goto beginGame;
					};
				}
			}
			continue;
		}

		// -------------------- Event Handle --------------------
		while (SDL_PollEvent(&BaseObject::MainScreen->g_event) != 0) {
			if (BaseObject::MainScreen->g_event.type == SDL_QUIT) {
				is_quit = true;
			}
			//setPath(path, *BaseObject::MainScreen);
			player->HandleEvent(machineList, monsterList);
		}
		// -------------------- Game Process --------------------
		SDL_SetRenderDrawColor(BaseObject::MainScreen->g_screen, 255, 255, 255, 255);
		SDL_RenderClear(BaseObject::MainScreen->g_screen);
		//set background
		g_background.Render(NULL);
		player->RenderMenu();
		// -------------------- Money Process --------------------
		Money_text.SetText(player->MoneyText());
		Money_text.LoadFromRenderText(BaseObject::MainScreen->g_font_text);
		Money_text.RenderText(30, 30);
		// -------------------- Tower Process --------------------
		Print_text.SetText(tow.Text());
		Print_text.LoadFromRenderText(BaseObject::MainScreen->g_font_text);
		Print_text.RenderText(BaseObject::MainScreen->SCREEN_WIDTH - 110,15);
		// -------------------- Machine Process --------------------
		machineList->ShotMonster(monsterList, bulletList);
		machineList->Render();
		// -------------------- Bullet Process --------------------
		bulletList->Update();
		// -------------------- Monster Process --------------------
		monsterList->Update(path);
		monsterList->AttackTower(&tow);
		monsterList->AddUserMoney(player);
		// -------------------- System Process --------------------
		//cout << "Here" << endl;
		Time::Increase();
	    SDL_RenderPresent(BaseObject::MainScreen->g_screen);
		if(!DEBUG_PATH) SDL_Delay(100);
	}
	// -------------------- Free Memory --------------------
	cout << "FINISH";
	g_background.Free();
	BaseObject::MainScreen->close();
	BaseObject::musicControl->Free();
	//if(DEBUG_PATH) savePath(path);
	return 0;
}

