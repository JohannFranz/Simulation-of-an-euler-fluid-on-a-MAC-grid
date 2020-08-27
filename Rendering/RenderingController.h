#pragma once
#include "glm.hpp"
#include "..\Definitions.h"
#include <QTWidgets/QOpenGLWidget>
#include <QTGui/QOpenGLFunctions>

class SimulationController;
class VelocityRenderer;
class DensityRenderer;
class QOpenGLVertexArrayObject;
class QSurfaceFormat;
class QOpenGLContext;

class RenderingController : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
								RenderingController(QWidget *parent, glm::vec2 windowSize, SimulationController* simCon);
								~RenderingController(void);

	void						initializeGL() override;
    void						paintGL() override;
	void						resizeGL(int w, int h) override;
	
	void						setMode(UpdateStruct& data);

private:
	QOpenGLVertexArrayObject*	m_Vao;

	glm::vec2					m_WindowSize;
	SimulationController*		m_SimCon;
	VelocityRenderer*			m_VelRenderer;
	DensityRenderer*			m_DensityRenderer;

	QSurfaceFormat*				m_Format;
	QOpenGLContext*				m_Context;

	FluidMode					m_Mode;
};

