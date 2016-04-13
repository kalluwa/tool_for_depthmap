//2015/11/13

#include "ApplicationCenter.h"
#include "DrawingWrapper.h"
#include "CDotRenderer.h"
#include "CDataToFloatProcessor.h"

#include "CMarchingCubeProcessor.h"
#include "CMarchingCubeRenderer.h"

#include "CRayCastRenderer.h"
#include "CCpuDepthProcessor.h"
#include "CCpuDepthRenderer.h"

std::string g_filePath = "";

ApplicationCenter::ApplicationCenter()
	:m_bPauseDrawing(false),m_loadFirstTime(true),m_cam(0)
{
	m_dataRenderer = 0;
	m_dataProcessor = 0;
}

ApplicationCenter::~ApplicationCenter()
{
	//if(m_volumeData)
	//	m_volumeData->Drop();
	//if(m_dataProcessor)
	//	m_dataProcessor->Drop();
	//if(m_dataRenderer)
	//	m_dataRenderer->Drop();

	for(auto i=0;i<(int)m_processorList.size();i++)
	{
		m_processorList[i]->Drop();
	}
	for(auto i=0;i<(int)m_rendererList.size();i++)
	{
		m_rendererList[i]->Drop();
	}

	if(m_volumeData)
		m_volumeData->Drop();
	//�ͷ���Ⱦ�豸
	DrawingWrapper::Drop();
}

void ApplicationCenter::SetCamera(kk::scene::CArcCameraNode* cam)
{
	m_cam = cam;
}

bool ApplicationCenter::Initialize()
{
	if(!m_cam) 
	{
		ShowMessage("�����û���趨��Application����");
		return false;
	}
	//��ʼ����Ⱦ�豸
	DrawingWrapper::Initialize();

	m_volumeData = new VolumeData<unsigned char>();

	//��һ��
	auto dataToFloatProcessor = new CDataToFloatProcessor();
	auto dotRenderer = new CDotRenderer();
	//dot
	m_processorList.push_back(dataToFloatProcessor);
	m_rendererList.push_back(dotRenderer);
	

	//marching cube
	m_processorList.push_back(new CMarchingCubeProcessor());
	m_rendererList.push_back(new CMarchingCubeRenderer());

	//raycast type1
	m_processorList.push_back(dataToFloatProcessor);
	m_rendererList.push_back(new CRayCastRenderer(0));
	dataToFloatProcessor->Grab();

	//raycast type2
	m_processorList.push_back(dataToFloatProcessor);
	m_rendererList.push_back(new CRayCastRenderer(1));
	dataToFloatProcessor->Grab();

	//CPU Depth
	//dot
	m_processorList.push_back(new CCpuDepthProcessor());
	m_rendererList.push_back(new CCpuDepthRenderer());


	//m_dataProcessor = new CMarchingCubeProcessor();
	//m_dataRenderer = new CMarchingCubeRenderer();

	//m_dataRenderer = new CRayCastRenderer(0);

	for(int i=0;i<(int)m_rendererList.size();i++)
	{
		m_rendererList[i]->SetCamera(m_cam);
	}
	return true;
}
bool ApplicationCenter::Reinitialize()
{
	m_volumeData->Drop();// = new VolumeData<unsigned char>();

	for(int i=0;i<(int)m_processorList.size();i++)
	{
		m_processorList[i]->Drop();
	}
	m_processorList.clear();
	for(int i=0;i<(int)m_rendererList.size();i++)
	{
		m_rendererList[i]->Drop();
	}
	m_rendererList.clear();

#if 0
	//re initialize
	m_volumeData = new VolumeData<unsigned char>();
	//��һ��
	auto dataToFloatProcessor = new CDataToFloatProcessor();
	auto dotRenderer = new CDotRenderer();
	//dot
	m_processorList.push_back(dataToFloatProcessor);
	m_rendererList.push_back(dotRenderer);
	

	//marching cube
	m_processorList.push_back(new CMarchingCubeProcessor());
	m_rendererList.push_back(new CMarchingCubeRenderer());

	//raycast type1
	m_processorList.push_back(dataToFloatProcessor);
	m_rendererList.push_back(new CRayCastRenderer(0));
	dataToFloatProcessor->Grab();

	//raycast type2
	m_processorList.push_back(dataToFloatProcessor);
	m_rendererList.push_back(new CRayCastRenderer(1));
	dataToFloatProcessor->Grab();

	//CPU Depth
	//dot
	m_processorList.push_back(new CCpuDepthProcessor());
	m_rendererList.push_back(dotRenderer);
	dotRenderer->Grab();
#else
	Initialize();
#endif

	return true;
}

void ApplicationCenter::SetRendererAndProcessor(int index)
{
	m_bPauseDrawing = true;

	m_dataProcessor = m_processorList[index];
	m_dataRenderer = m_rendererList[index];

	m_iCurrentIndex = index;
	//TODO
	//������
	m_dataProcessor->Process();//ֻ�е�һ�γ�ʼ������
	
	m_dataRenderer->SetData(m_dataProcessor->GetProcessDataArray());
	m_dataRenderer->Process();//ֻ�е�һ�γ�ʼ������

	m_bPauseDrawing = false;

}
bool ApplicationCenter::LoadContent(const std::string& filePath,int nMaxSize)
{
	m_bPauseDrawing = true;

	g_filePath = filePath;
	
	if(!m_loadFirstTime)
		Reinitialize();

	if(m_processorList.size() < 0)
	{
		ShowMessage("m_processorListΪ��");
		return false;
	}

	m_volumeData->SetNMaxSize(nMaxSize);
	if(!m_volumeData->LoadDataFromFile(g_filePath.c_str()))
	{
		ShowMessage("��ȡ�ļ�����");
	}
	//�ڶ���
	//�������ݣ�ֻ��һ�����ã�������attach��
	m_processorList[0]->SetData(m_volumeData->GetDataArray());
	for(int i=1;i<(int)m_processorList.size();i++)
	{
		m_processorList[i]->AttachArray(m_processorList[0]->GetOriginalData(),
			m_processorList[0]->GetProcessDataArray());
	}
	//for(int i=0;i<m_processorList.size();i++)
	//{
		//m_processorList[i]->Process();
	//}
	//������glewinit��Ҫ�ȴ���
	SetRendererAndProcessor(0);

	m_bPauseDrawing = false;
	m_loadFirstTime = false;

	return true;
}
void ApplicationCenter::Update()
{
	//TODO:mouse move
}

void ApplicationCenter::Draw()
{
	m_cam->setUpMatrices();
	if(m_dataRenderer == NULL)
	{
		static int index = 0;
		index++;
		//if(index==1)
			//ShowMessage("��Ⱦ������Ϊ��");

		return;
	}
	if(!m_bPauseDrawing)
		//draw volume/Triangles
		m_dataRenderer->Draw();

}

