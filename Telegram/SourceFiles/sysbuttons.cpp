/*
This file is part of Telegram Desktop,
the official desktop version of Telegram messaging app, see https://telegram.org

Telegram Desktop is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

It is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

Full license: https://github.com/telegramdesktop/tdesktop/blob/master/LICENSE
Copyright (c) 2014 John Preston, https://desktop.telegram.org
*/
#include "stdafx.h"
#include "style.h"
#include "lang.h"

#include "sysbuttons.h"
#include "window.h"
#include "application.h"

SysBtn::SysBtn(QWidget *parent, const style::sysButton &st, const QString &text) : Button(parent),
_st(st), a_color(_st.color->c), _overLevel(0), _text(text) {
	int32 w = _st.size.width() + (_text.isEmpty() ? 0 : ((_st.size.width() - _st.img.pxWidth()) / 2 + st::titleTextButton.font->m.width(_text)));
	resize(w, _st.size.height());
	setCursor(style::cur_default);
	connect(this, SIGNAL(stateChanged(int, ButtonStateChangeSource)), this, SLOT(onStateChange(int, ButtonStateChangeSource)));
}

void SysBtn::setText(const QString &text) {
	_text = text;
	int32 w = _st.size.width() + (_text.isEmpty() ? 0 : ((_st.size.width() - _st.img.pxWidth()) / 2 + st::titleTextButton.font->m.width(_text)));
	resize(w, _st.size.height());
}

void SysBtn::setOverLevel(float64 level) {
	_overLevel = level;
	update();
}

void SysBtn::onStateChange(int oldState, ButtonStateChangeSource source) {
	a_color.start((_state & StateOver ? _st.overColor : _st.color)->c);

	if (source == ButtonByUser || source == ButtonByPress) {
		anim::stop(this);
		a_color.finish();
		update();
	} else {
		anim::start(this);
	}
}

void SysBtn::paintEvent(QPaintEvent *e) {
	QPainter p(this);

	int x = width() - ((_st.size.width() + _st.img.pxWidth()) / 2), y = (height() - _st.img.pxHeight()) / 2;
	QColor c = a_color.current();
	if (_overLevel > 0) {
		if (_overLevel >= 1) {
			c = _st.overColor->c;
		} else {
			c.setRedF(c.redF() * (1 - _overLevel) + _st.overColor->c.redF() * _overLevel);
			c.setGreenF(c.greenF() * (1 - _overLevel) + _st.overColor->c.greenF() * _overLevel);
			c.setBlueF(c.blueF() * (1 - _overLevel) + _st.overColor->c.blueF() * _overLevel);
		}
	}
	p.fillRect(x, y, _st.img.pxWidth(), _st.img.pxHeight(), c);
	p.drawPixmap(QPoint(x, y), App::sprite(), _st.img);

	if (!_text.isEmpty()) {
		p.setFont(st::titleTextButton.font->f);
		p.setPen(c);
		p.drawText((_st.size.width() - _st.img.pxWidth()) / 2, st::titleTextButton.textTop + st::titleTextButton.font->ascent, _text);
	}
}

HitTestType SysBtn::hitTest(const QPoint &p) const {
	int x(p.x()), y(p.y()), w(width()), h(height());
	if (x >= 0 && y >= 0 && x < w && y < h && isVisible()) {
		return HitTestSysButton;
	}
	return HitTestNone;
}

bool SysBtn::animStep(float64 ms) {
	float64 dt = ms / _st.duration;
	bool res = true;
	if (dt >= 1) {
		a_color.finish();
		res = false;
	} else {
		a_color.update(dt, anim::linear);
	}
	update();
	return res;
}

MinimizeBtn::MinimizeBtn(QWidget *parent, Window *window) : SysBtn(parent, st::sysMin), wnd(window) {
	connect(this, SIGNAL(clicked()), this, SLOT(onClick()));
}

void MinimizeBtn::onClick() {
	wnd->setWindowState(Qt::WindowMinimized);
}

MaximizeBtn::MaximizeBtn(QWidget *parent, Window *window) : SysBtn(parent, st::sysMax), wnd(window) {
	connect(this, SIGNAL(clicked()), this, SLOT(onClick()));
}

void MaximizeBtn::onClick() {
	wnd->setWindowState(Qt::WindowMaximized);
}

RestoreBtn::RestoreBtn(QWidget *parent, Window *window) : SysBtn(parent, st::sysRes), wnd(window) {
	connect(this, SIGNAL(clicked()), this, SLOT(onClick()));
}

void RestoreBtn::onClick() {
	wnd->setWindowState(Qt::WindowNoState);
}

CloseBtn::CloseBtn(QWidget *parent, Window *window) : SysBtn(parent, st::sysCls), wnd(window) {
	connect(this, SIGNAL(clicked()), this, SLOT(onClick()));
}

void CloseBtn::onClick() {
	wnd->close();
}

UpdateBtn::UpdateBtn(QWidget *parent, Window *window, const QString &text) : SysBtn(parent, st::sysUpd, text), wnd(window) {
	connect(this, SIGNAL(clicked()), this, SLOT(onClick()));
}

void UpdateBtn::onClick() {
	psCheckReadyUpdate();
	if (App::app()->updatingState() == Application::UpdatingReady) {
		cSetRestartingUpdate(true);
	} else {
		cSetRestarting(true);
		cSetRestartingToSettings(false);
	}
	App::quit();
}
