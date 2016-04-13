//2015/11/13
#ifndef _APPLICATION_CENTER_H_
#define _APPLICATION_CENTER_H_

#include "IBase.h"

//data
#include "VolumeData.h"
//processor
#include "IDataProcessor.h"
//renderer
#include "IDataRenderer.h"
//camera
#include "helper/CArcCamera.h"

class ApplicationCenter : public virtual IBase
{
public:

	ApplicationCenter();

	~ApplicationCenter();

	bool Initialize();

	bool Reinitialize();
	
	bool LoadContent(const std::string& filePath="C:\\Users\\kk\\Desktop\\pi\\1\\bottle.3ddat",int nMaxSize=50);

	void Update();

	void Draw();

	void SetRendererAndProcessor(int index);

	void SetNextRendererAndProcessor()
	{
		int newIndex = (m_iCurrentIndex+1) % (int)m_processorList.size();
		SetRendererAndProcessor(newIndex);
	}

	void SetPreviousRendererAndProcessor()
	{
		int newIndex = (m_iCurrentIndex-1 + (int)m_processorList.size()) % (int)m_processorList.size();
		SetRendererAndProcessor(newIndex);
	}
	void SetCamera(kk::scene::CArcCameraNode* cam);
	//gets
	IDataRenderer* GetCurrentRenderer() { return m_dataRenderer;};
	kk::scene::CArcCameraNode* GetCamera() { return m_cam; }
private:

	VolumeData<unsigned char>* m_volumeData;
	IDataProcessor* m_dataProcessor;
	IDataRenderer* m_dataRenderer;
	std::vector<IDataProcessor*> m_processorList;
	std::vector<IDataRenderer*> m_rendererList;
	int m_iCurrentIndex;
	bool m_bPauseDrawing;
	bool m_loadFirstTime;
	//camera
	kk::scene::CArcCameraNode* m_cam;
};
#endif