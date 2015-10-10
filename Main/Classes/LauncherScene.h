#ifndef __LAUNCHER_SCENE_H__
#define __LAUNCHER_SCENE_H__

#include "BaseScene.h"

class LauncherScene : public XUtility::BaseScene<LauncherScene>
{
protected:
	virtual const char* GetSceneCsb() const override;
};

#endif // __LAUNCHER_SCENE_H__
