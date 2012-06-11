#pragma once
#include "app.h"
class GLApp :
	public App
{
public:
	GLApp(void);
	~GLApp(void);

	bool onInit();
	void onRender();
};

