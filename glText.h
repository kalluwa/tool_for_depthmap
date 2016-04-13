#ifndef	__GLFONT_32167_H__
#define	__GLFONT_32167_H__

#include <Windows.h>
#include <xstring>
#include <map>
using namespace std;

struct FontDesc
{
	//�б���
	int iList;
	//��������
	GLYPHMETRICSFLOAT flyfloat;
};

class CGlFont                                                      
{
protected:
	HFONT m_hFont;
	float m_fExtrusion;
public:
	CGlFont();
	~CGlFont();

	void DeleteFont();
	bool InitFont(WCHAR *fontName);
	bool InitFont(LOGFONT &lf);

	HFONT GetFontHandle(){return m_hFont;}
	LOGFONT GetLogFont();

	// Get font bitmap use wglUseFontBitmaps, then draw in the scene
	void Draw2DText(char *string);
	// Use wglUseFontOutlines,return the length of the string
	void Draw3DText(const WCHAR* string);
	void SetColor(int r, int g, int b);

	float	GetExtrusion() {return m_fExtrusion;}
	void	SetExtrusion(float extrusion) {m_fExtrusion = extrusion;}
public:	
	double m_X;			//�ַ���λ��
	double m_Y;			//�ַ���λ��	

	double m_dHeight;	//���ָ�	

	std::string m_str;		//��������	

	double m_rotateTheta;//��ת�Ƕ�

	int m_iDisplayList;	//��ʾ�б�ID

	 static map<UINT,FontDesc>  m_sglFontList;	 

	 int m_red;		//��
	 int m_green;	//��	 
	 int m_blue;	//��
};

#endif    // __GLFONT_32167_H__ 
