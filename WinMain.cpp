// =======================================
//  ライブラリ読込
// =======================================
#include <ctime>		// 標準C ライブラリ ヘッダー <time.h> をインクルードし、関連する名前を std 名前空間に追加します。

#include "DxLib.h"		// DxLib

//	定数の宣言
#define WINDOW_W 1920	// 画面サイズ　横
#define WINDOW_H 1080	// 画面サイズ　縦


// ------------------------------
// WinMain
// ------------------------------
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{

	// =======================================
	//  初期化処理
	// =======================================
	// ログの書き出しを無効化
	SetOutApplicationLogValidFlag(FALSE);	// ログの書き出しを無効化

	// ウィンドウモードの設定
	ChangeWindowMode(TRUE);					// ウインドウモードを有効化

	// ウィンドウサイズとカラービットの設定
	SetGraphMode(WINDOW_W, WINDOW_H, 32);

	// ウィンドウのタイトル
	SetMainWindowText("farm action");

	// 背景色の設定
	SetBackgroundColor(128, 128, 128);

	// ＤＸライブラリ初期化処理
	if (DxLib_Init() == -1)
	{
		return -1;							// エラーが起きたら直ちに終了
	}

	// ウィンドウ描画モード設定
	SetDrawScreen(DX_SCREEN_BACK);			// 裏画面に描画する



	// ------------------------------
	//  ゲームループ
	// ------------------------------
	while (ProcessMessage() == 0)
	{
		// リフレッシュレートを設定するための処理
		clock_t check_fps = clock() + CLOCKS_PER_SEC / 60;

		// マウスカーソル表示設定
		SetMouseDispFlag(FALSE);

		// printfDxの初期化
		clsDx();

		// 画面上の描画を初期化（画面を消去）
		ClearDrawScreen();


		// =======================================
		//  入力処理
		// =======================================





		// =======================================
		//  画面描画処理
		// =======================================





		// =======================================
		//  音声再生処理
		// =======================================






		// リフレッシュレートが一定になるまで待つ処理
		while (clock() < check_fps) {}

		// 裏画面の描画を表に反映
		ScreenFlip();


		// ESCキーでループから抜ける
		if (CheckHitKey(KEY_INPUT_ESCAPE)) {
			break;
		}

	}

	// =======================================
	//  終了処理
	// =======================================
	// ＤＸライブラリ使用の終了処理
	DxLib_End();							// ＤＸライブラリ使用の終了処理

	// ソフトの終了
	return 0;
}