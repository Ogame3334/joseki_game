﻿# include <Siv3D.hpp>

constexpr int size_00 = 15;
constexpr int size_01 = 30;
constexpr int size_02 = 40;
constexpr int size_03 = 60;
constexpr int size_04 = 90;
constexpr int size_05 = 110;
constexpr int size_06 = 130;

constexpr int Width = 470;

constexpr Color BACKGROUND_COLOR{ 240, 255, 240 };

String getText(int size) {
	String text = U"";
	switch (size) {
	case size_00:
		text = U"絶起";
		break;
	case size_01:
		text = U"遅刻";
		break;
	case size_02:
		text = U"欠席";
		break;
	case size_03:
		text = U"落単";
		break;
	case size_04:
		text = U"留年";
		break;
	case size_05:
		text = U"退学";
		break;
	case size_06:
		text = U"除籍";
		break;
	}
	return text;
}

int getNextCircleSize(int size) {
	int output = 0;
	switch (size)
	{
	case size_00:
		output = size_01;
		break;
	case size_01:
		output = size_02;
		break;
	case size_02:
		output = size_03;
		break;
	case size_03:
		output = size_04;
		break;
	case size_04:
		output = size_05;
		break;
	case size_05:
		output = size_06;
		break;
	default:
		break;
	}

	return output;
}

void updateSummonPos(Vec2& pos, int now_size) {
	pos = Cursor::PosF();
	pos.clamp(RectF(-Width / 2 + now_size + 1, -600, Width - now_size * 2 - 2, 0));
}

P2Body& getNowCircle(Array<P2Body>& bodies, P2BodyID id) {
	for (auto& b : bodies) {
		if (b.id() == id) return b;
	}
	return bodies.back();
}

using App = SceneManager<String>;

// タイトルシーン
class Game : public App::Scene
{
private:
	Font font{ 100, Typeface::CJK_Regular_JP };
	Font verfont{ 30, Typeface::CJK_Regular_JP };
	// 2D 物理演算のシミュレーションステップ（秒）
	static constexpr double StepTime = (1.0 / 200.0);
	// 2D 物理演算のシミュレーション蓄積時間（秒）
	double accumulatedTime = 0.0;
	// 重力加速度 (cm/s^2)
	static constexpr double Gravity = 980;
	// 2D 物理演算のワールド
	P2World world{ Gravity };
	// [|_|] 地面 (幅 1200 cm の床）
	P2Body ground;
	// 物体
	Array<P2Body> bodies;

	Vec2 summonPos = { 0, -600 };
	int now_size = size_00;
	int next_size = size_00;
	P2Material material{};
	P2BodyID now_id;
	P2BodyID next_id;

	std::array<int, 3> start_size{ size_00, size_01, size_02 };
	bool clicked = false;

	int score = 0;
	bool isGameover = false;

	bool isFocused = false;

	// 2D カメラ
	Camera2D camera{ Vec2{ 0, -300 }, 1, CameraControl::None_ };
public:
	// コンストラクタ（必ず実装）
	Game(const InitData& init)
		: IScene{ init }
	{
		material.density = 0.5;
		material.restitution = 0.0;
		material.friction = 0.1;
		material.restitutionThreshold = 10000;
		ground = world.createLineString(P2Static, Vec2{ 0, 0 }, LineString{ {-Width / 2, -500}, {-Width / 2, 0}, {Width / 2, 0}, {Width / 2, -500} }, YesNo<OneSided_tag>::No, material);
		bodies << world.createCircle(P2Static, summonPos, now_size, material);
		now_id = bodies.back().id();
		bodies << world.createCircle(P2Static, Vec2{ 400, -400 }, next_size, material);
		next_id = bodies.back().id();
		Camera2DParameters param;
		param.controlScaleFactor = 10000;
		param.controlSpeedFactor = 1;
		param.grabSpeedFactor = 0;
		param.wheelScaleFactor = 0;
		param.minScale = 1;
		param.maxScale = 1;
		camera.setParameters(param);
	}

	// 更新関数（オプション）
	void update() override
	{
		for (accumulatedTime += Scene::DeltaTime(); StepTime <= accumulatedTime; accumulatedTime -= StepTime)
		{
			ClearPrint();

			// 2D 物理演算のワールドを更新する
			world.update(StepTime);

			// 接触が発生しているボディの ID を表示する
			for (auto&& [pair, collision] : world.getCollisions())
			{
				if (pair.a == now_id or pair.b == now_id) {
					now_id = next_id;
					bodies << world.createCircle(P2Static, Vec2{ 400, -400 }, now_size);
					next_id = bodies.back().id();
					clicked = false;
				}
				if (pair.a == 1) continue;
				P2Body first, second;
				for (const auto& b : bodies) {
					if (pair.a == b.id()) first = b;
					if (pair.b == b.id()) second = b;
				}
				P2Circle& circle_01 = static_cast<P2Circle&>(first.shape(0));
				P2Circle& circle_02 = static_cast<P2Circle&>(second.shape(0));
				if (circle_01.getCircle().r == circle_02.getCircle().r) {
					int size = getNextCircleSize(static_cast<int>(circle_02.getCircle().r));
					score += static_cast<int>(circle_02.getCircle().r);
					for (int i = 0; i < bodies.size(); i++) {
						if (pair.a == bodies[i].id()) {
							bodies.erase(bodies.begin() + i);
							break;
						}
					}
					for (int i = 0; i < bodies.size(); i++) {
						if (pair.b == bodies[i].id()) {
							bodies.erase(bodies.begin() + i);
							break;
						}
					}
					if(size != 0){
						bodies << world.createCircle(P2Dynamic, circle_01.getCircle().center.lerp(circle_02.getCircle().center, 0.5), size, material);
						//auto temp = bodies.back();
						//Vec2 pos = temp.getPos();
						////Console << bodies.size();
						//for (auto& body : bodies) {
						//	if (body.id() == temp.id() or body.id() == pair.a or body.id() == pair.b) continue;
						//	auto dist = pos.distanceFrom(body.getPos()) - size;
						//	//if (dist <= static_cast<const P2Circle&>(body.shape(0)).getCircle().r + 5)
						//		//body.setVelocity(pos.lerp(body.getPos(), 1.001));
						//}
					}
				}
			}
		}

		// 地面より下に落ちた物体は削除する
		bodies.remove_if([](const P2Body& b) { return (200 < b.getPos().y); });

		// 2D カメラを更新する
		camera.update();
		{
			// 2D カメラから Transformer2D を作成する
			const auto t = camera.createTransformer();

			font(U"Score: " + Format(score)).draw(30, -500, -500, Palette::Black);

			if (isFocused and !clicked and !isGameover) {
				auto circle = getNowCircle(bodies, now_id);
				updateSummonPos(summonPos, static_cast<const P2Circle&>(circle.shape(0)).getCircle().r);
				circle.setPos(summonPos);
			}

			// 左クリックしたら
			if (MouseL.down() and isFocused and !clicked and !isGameover)
			{
				getNowCircle(bodies, now_id).setBodyType(P2Dynamic);
				now_size = next_size;
				next_size = start_size[Random(2)];
				clicked = true;
				score += 10;
			}

			for (const auto& body : bodies)
			{
				if (body.id() != now_id and body.getPos().y < -500)	isGameover = true;

				// すべてのボディを描画する
				const P2Circle& circle = static_cast<const P2Circle&>(body.shape(0));
				int size = static_cast<int>(circle.getCircle().r);
				body.draw(HSV{ size * 2 });
				body.drawFrame((2, 2), Palette::Gray);
				{
					const Transformer2D tr{ Mat3x2::Rotate(body.getAngle(), body.getPos()) };

					font(getText(size)).drawAt(20, body.getPos(), Palette::Black);
				}
			}
			if (isGameover) for (auto& body : bodies) body.setBodyType(P2Static);

			// 地面を描画する
			ground.draw(Palette::Skyblue);
			RoundRect{ 325, -475, 150, 150, 10 }.drawFrame(1, 0, Palette::Black);
			font(U"Next").drawAt(20, Vec2{ 400, -500 }, Palette::Black);
			if (isGameover) {
				if (SimpleGUI::ButtonAt(U"Restart", Vec2{ 500, 0 })) {
					changeScene(U"Game", 0.1s);
				}
			}
		}

		// 2D カメラの操作を描画する
		camera.draw(Palette::Orange);
		if (isGameover)
		{
			Rect{ Point{0, 0}, Scene::Size() }.draw(ColorF(0, 0, 0, 0.2));
			font(U"GameOver!!\nScore: " + Format(score)).drawAt(100, Scene::CenterF(), Palette::Black);
		}
		if (MouseL.down() and !isFocused) isFocused = true;
		if (KeyEscape.down() and isFocused) isFocused = false;

		auto temp = verfont(U"beta_v1.1");
		temp.draw(Scene::Width() - temp.region().w, Scene::Height() - temp.region().h, Palette::Black);
	}

	// 描画関数（オプション）
	void draw() const override
	{

	}
};

void Main()
{
	// ウィンドウを 1280x720 にリサイズする
	Window::Resize(1280, 720);
	Window::SetStyle(WindowStyle::Sizable);
	Scene::SetBackground(BACKGROUND_COLOR);
	Window::SetTitle(U"除籍ゲーム");
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);

	// シーンマネージャーを作成
	App manager;

	// タイトルシーン（名前は "Title"）を登録
	manager.add<Game>(U"Game");
	manager.init(U"Game", 0.1s);

	while (System::Update())
	{
		// 現在のシーンを実行
		// シーンに実装した .update() と .draw() が実行される
		if (not manager.update())
		{
			break;
		}
	}
}
