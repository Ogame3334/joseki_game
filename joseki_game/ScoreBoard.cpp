#include "ScoreBoard.hpp"

void ScorePanel::update() {
	
}
void ScorePanel::draw(Vec2 pos, int lank) const {
	auto point = pos.asPoint();
	auto area = RoundRect{ point.x + 4, point.y + 4, Width - 8, Height - 8, 6 };
	area.draw();
	area.drawFrame((1, 1), Palette::Forestgreen);
	font(lank).draw(pos + Vec2{ 10, 0 }, Palette::Black);
	font(this->name).draw(pos + Vec2{ 100, 0 }, Palette::Black);
	auto score_text = font(this->score);
	score_text.draw(pos + Vec2{ Width - score_text.region().w - 10, 0}, Palette::Black);
}

void ScoreBoard::update() {
	if (this->view_area.mouseOver()) {
		this->addPanelsPos(-Mouse::Wheel() * 20);
	}
	if (this->response.isReady()) {
		if (not this->response.isFailed()) {
			auto result = this->response.getAsJSON();
			auto hoge = this->response.getResponse();
			if (not result[U"lanking"].isNull()) {
				//Console << U"ここ";
				for (int i = 0; i < result[U"lanking"].size(); i++) {
					auto lanking = result[U"lanking"];
					this->score_panels.push_back(
						ScorePanel{ lanking[i][U"name"].getString(), lanking[i][U"score"].get<int>() }
					);
				}
			}
		}
	}
	for (auto& panel : this->score_panels) {
		panel.update();
	}
}
void ScoreBoard::draw() const {
	this->view_area.draw();
	this->view_area.drawFrame((1, 1), Palette::Forestgreen);
	{
		RasterizerState rs = RasterizerState::Default2D;
		// シザー矩形を有効化
		rs.scissorEnable = true;
		const ScopedRenderStates2D rasterizer{ rs };
		auto pos = this->view_area.rect.pos;
		//Console << this->score_panels.size();
		int lank = 1;
		int pre_score = 0;
		for (int i = 0; const auto& panel : this->score_panels) {
			panel.draw(pos + this->panels_pos + Vec2{ 0, (ScorePanel::Height - 4) * i }, lank);
			++lank;
			if (pre_score == panel.getScore()) --lank;
			pre_score = panel.getScore();
			++i;
		}
	}
}
