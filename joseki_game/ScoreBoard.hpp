#pragma once
#include "version_id.hpp"

class ScorePanel {
public:
	inline static int Width = 0;
	inline static int Height = 0;
private:
	String name;
	int score;

	Font font{ 30, Typeface::CJK_Regular_JP };
public:
	ScorePanel() = default;
	ScorePanel(String name, int score) : name(name), score(score) {}
	constexpr int getScore() const noexcept { return this->score; }
	void update();
	void draw(Vec2 pos, int lank) const;
};

class ScoreBoard {
private:
	RoundRect view_area;
	Array<ScorePanel> score_panels;
	Vec2 panels_pos{ 0, 0 };

	const URL url = U"https://joseki.ogmgre.com/score/lanking/";
	const HashTable<String, String> headers{};
	AsyncHTTPTask response;
public:
	ScoreBoard(Point point, Size size, RoundRect::value_type r) {
		this->view_area = RoundRect{ point, size, r };
		Graphics2D::SetScissorRect(this->view_area.rect.asRect());
		this->response = SimpleHTTP::GetAsync(this->url + Format(VERSION_ID), headers);
		ScorePanel::Width = this->view_area.w;
		ScorePanel::Height = 50;
	}
	void addPanelsPos(double dif) {
		this->panels_pos.y += dif;
		if (this->panels_pos.y < -((ScorePanel::Height - 4) * this->score_panels.size() - this->view_area.h + 4))
			this->panels_pos.y = -((ScorePanel::Height - 4) * this->score_panels.size() - this->view_area.h + 4);
		if (this->panels_pos.y > 0) this->panels_pos.y = 0;
	}
	void update();
	void draw() const;
};
