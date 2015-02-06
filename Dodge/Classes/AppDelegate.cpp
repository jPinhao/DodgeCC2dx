#include "AppDelegate.h"
#include "LevelManager.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
	setupRenderSettings();

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
	LevelManager *levelManager = LevelManager::getInstance();
	levelManager->initializeLevels();

	if (levelManager->loadMainMenu())
	{
		return true;
	}
	else return false;
}

bool AppDelegate::setupRenderSettings()
{
	auto director = Director::getInstance();
	auto glView = director->getOpenGLView();
	if (!glView) {
		glView = GLViewImpl::create("My Game");//createWithRect("My Game",Rect(0, 0, 480, 320));
		director->setOpenGLView(glView);
	}

	Size screenSize = glView->getFrameSize();
	FileUtils *fileUtils = FileUtils::getInstance();
	std::vector<std::string> searchPaths;

	Size designSize = Size(480, 320);
	Size resourceSize;

	searchPaths.push_back("Resources");
	searchPaths.push_back("../res");
	// if the device is iPad
	if (screenSize.height >= 768) 
	{
		searchPaths.push_back("hd");
		resourceSize = Size(1024, 768);
		designSize = Size(1024, 768);
	}
	// if the device is iPhone
	else
	{
		// for retina iPhone
		if (screenSize.height > 320) 
		{
			searchPaths.push_back("hd");
			resourceSize = Size(960, 640);
		}
		else
		{
			searchPaths.push_back("sd");
			resourceSize = Size(480, 320);
		}
	}
	fileUtils->setSearchPaths(searchPaths);
	
	director->setContentScaleFactor(resourceSize.width / designSize.width);
	glView->setDesignResolutionSize(designSize.width, designSize.height, kResolutionFixedWidth);

	return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
