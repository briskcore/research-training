#include "../Source/Application/application.h"
#include "../Source/Direct3D9/Direct3D9.h"

#include <string>
#include <map>
#include <list>

#include <sstream>

bool display(double totalTime,double deltaTime,IDirect3DDevice9 *device,HWND windowHandle);


int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE previousDeprecatedInstanceHandle,LPSTR lpCmdLine,int nShowCommand)
{
	IDirect3DDevice9 *device;
	HWND windowHandle;
	if(!Direct3D::v9::initialization::initialize(
		hInstance, 
		(WNDPROC) Direct3D::v9::initialization::defaultWindowProc,
		800, 800, false, D3DDEVTYPE_HAL, &device,&windowHandle
		)
		)
	{
		MessageBoxA(0,"initial failed",0,0);
		return 0;
	}

	//定义摄像机
	Direct3D::v9::configuration::setStaticCamera(device,D3DXVECTOR3(0.0f,0.0f,0.0f),D3DXVECTOR3(0.0f,0.0f,1.0f),D3DX_PI*0.5f,600,600,0.0f,1048576.0f);
	//Direct3D::v9::configuration::setStaticCamera(device,D3DXVECTOR3(0.0f,0.0f,-1.0f),D3DXVECTOR3(0.0f,0.0f,0.0f),D3DX_PI*0.5f,800,800,0.0f,1048576.0f);

	//设置线框模型
	//device->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	//不设置 默认按面填充

	device->SetRenderState(D3DRS_SHADEMODE,D3DSHADE_GOURAUD);//高洛德着色模式
	device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS,TRUE); //全屏抗锯齿


	//现在启用了光照，因为有了3D字体
	//设置白光
	D3DCOLORVALUE colorValue;
	colorValue.a = 1.0f;
	colorValue.r = 1.0f;
	colorValue.g = 1.0f;
	colorValue.b = 1.0f;

	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_DIRECTIONAL;
	light.Ambient   = colorValue;
	light.Diffuse   = colorValue;
	light.Specular  = colorValue;
	light.Direction = D3DXVECTOR3(0.0f,0.0f,1.0f);

	device->SetLight(0,&light);
	device->LightEnable(0,true);
	//在渲染中需要计算光照
	device->SetRenderState(D3DRS_LIGHTING,true);


	//启动子线程
	unsigned short int serverPort = 941;

	std::map<std::string, void *> parameters;
	parameters["ServerPort"] = (void *)&serverPort;
	DWORD threadId;
	//HANDLE threadHandle = CreateThread(NULL, 0, Application::HUD::Thread::receiveMessageBySocket, (LPVOID *)(&parameters), 0, &threadId);
	HANDLE threadHandle = CreateThread(NULL,0,Application::HUD::Thread::receiveDataFrom1553B,NULL,0,&threadId);		//在我的笔记本电脑上进行调试时千万不能…………否则就……



	//进入主循环
	Direct3D::v9::enterMessageLoop(display,device,windowHandle);

	//释放设备
	device->Release();
	return 0;
}


bool display(double totalTime,double deltaTime,IDirect3DDevice9 *device,HWND windowHandle)
{
	if(device)
	{
		//VertexBuffer应该是每一次显示单独创建的
		//因为图元的数量可能改变
		//但是应该有一个全局变量保存所有的DisplayComponent
		/////////////////////////////////////////////
		using Direct3D::v9::resource::primitive::vertex::VertexWithColor;

		///////////////////////////////////
		///////////////////////////////////
		device->Clear(0,0,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
			D3DCOLOR_XRGB(0x80,0x80,0x80),1.0f,0
			);
		device->BeginScene();
		//开始绘制


		/*
		struct VertexWithColor tempVertex1,tempVertex2;
		tempVertex1.color = D3DCOLOR_XRGB(0,0xff,0);
		tempVertex1.x = -500;
		tempVertex1.y = 0;
		tempVertex1.z = 1000;
	
		tempVertex2.color = D3DCOLOR_XRGB(0,0xff,0);
		tempVertex2.x = 500;
		tempVertex2.y = 0;
		tempVertex2.z = 1000;
		std::pair<VertexWithColor,VertexWithColor> testLine1 = std::pair<VertexWithColor,VertexWithColor>(tempVertex1,tempVertex2);

		std::list<std::pair<VertexWithColor,VertexWithColor>> testLineList2;
		testLineList2.push_back(testLine1);

		Direct3D::v9::resource::primitive::LineList<VertexWithColor> testLineList = 
			Direct3D::v9::resource::primitive::LineList<VertexWithColor>(device,testLineList2);
		testLineList.render();
		*/

		//设置材质
		//////////////////////////////////////////
		D3DCOLORVALUE colorValue;
		colorValue.a = 1.0f;
		colorValue.r = 0.0f;
		colorValue.g = 1.0f;
		colorValue.b = 0.0f;

		D3DMATERIAL9 material;
		material.Ambient = colorValue;
		material.Diffuse = colorValue;
		material.Specular = colorValue;

		colorValue.r = 0;
		colorValue.g = 0;
		colorValue.b = 0;

		material.Emissive = colorValue;
		material.Power = 2.0f;
		device->SetMaterial(&material);

		/*
		//绘制3D文本
		//下面的部分测试绘制3D字体
		Direct3D::v9::resource::mesh::text::TextMesh testText = 
			Direct3D::v9::resource::mesh::MeshFactory::createTextMesh(device,std::string("NWPU"),std::string("Microsoft YaHei"));
		testText.setTranslation(D3DXVECTOR3(-250,250,500.0f));
		//testText.setRotation(D3DXVECTOR3(0,0,0.25*D3DX_PI));
		//testText.setScale(D3DXVECTOR3(0.5f,0.5f,0.5f));
		//设置边长
		testText.setSideLength(D3DXVECTOR3(500,500,0.001f));
		testText.render();
		*/

		Application::HUD::ParameterStore *paramaterStore = Application::HUD::ParameterStore::getInstance();

		//姿态指示器
		//其实可以用建造器模式的
		Application::HUD::AttitudeIndicator a = Application::HUD::AttitudeIndicator(20,5);
		a.setPitch(paramaterStore->getPitch());
		a.setRoll(paramaterStore->getRoll());
		a.render(device);

		//朝向指示器
		Application::HUD::HeadingIndicator headingIndicator =
			Application::HUD::HeadingIndicator(20,4,D3DXVECTOR3(0,850,1000),1200,200);
		headingIndicator.setDegrees((float)paramaterStore->getYaw());
		headingIndicator.render(device);

		//右侧的高度指示器和左侧的速度指示器
		Application::HUD::HeightIndicator heightIndicator = 
			Application::HUD::HeightIndicator(200,6,D3DXVECTOR3(800,0,1000),300,1200);
		heightIndicator.setMeters((float)paramaterStore->getHeight());
		heightIndicator.render(device);

		Application::HUD::SpeedIndicator speedIndicator =
			Application::HUD::SpeedIndicator(10,6,D3DXVECTOR3(-800,0,1000),300,1200);
		speedIndicator.setSpeed((unsigned short int)paramaterStore->getSpeed());
		speedIndicator.render(device);
		//

		//马赫数指示器

		Application::HUD::MachIndicator machIndicator = 
			Application::HUD::MachIndicator(D3DXVECTOR3(-850,750,1000.0f));
		//machIndicator.display(device,1+(float)cos(0.001*totalTime));
		machIndicator.display(device,(float)paramaterStore->getMach());

		//无线电高度指示器
		Application::HUD::AltitudeIndicator altitudeIndicator =
			Application::HUD::AltitudeIndicator(D3DXVECTOR3(600,-600,1000));
		altitudeIndicator.display(device,(float)paramaterStore->getAltitude());


		//////////////////////////////////////////
		//下面的部分显示文字
		/*
		char buffer[64] = {0};

		sprintf_s(buffer,"%.2lf",paramaterStore->getPitch());
		std::string pitchText1 = std::string("俯仰(Pitch):");
		pitchText1.append(buffer);

		sprintf_s(buffer,"%.2lf",paramaterStore->getRoll());
		std::string rollText1 = std::string("滚转(Roll):");
		rollText1.append(buffer);

		Direct3D::v9::resource::mesh::text::TextMesh pitchText2 = 
			Direct3D::v9::resource::mesh::MeshFactory::createTextMesh(device,pitchText1,std::string("Microsoft YaHei"));
		pitchText2.setTranslation(D3DXVECTOR3(-400,475.0f,500.0f));
		//设置边长
		pitchText2.setSideLength(D3DXVECTOR3(200,50,0.001f));
		pitchText2.render();

		Direct3D::v9::resource::mesh::text::TextMesh rollText2 = 
			Direct3D::v9::resource::mesh::MeshFactory::createTextMesh(device,rollText1,std::string("Microsoft YaHei"));
		rollText2.setTranslation(D3DXVECTOR3(-400,425.0f,500.0f));
		//设置边长
		rollText2.setSideLength(D3DXVECTOR3(200,50,0.001f));
		rollText2.render();
		*/

		//////////////////////////////////////////
		device->EndScene();

		//将后台缓冲区的内容投射上前台
		device->Present(0,0,0,0);

		//在Present之后应当释放资源
		//但是已经封装交由MessageLoop统一处理，所以不需要再操这个心
	}
	return true;
}