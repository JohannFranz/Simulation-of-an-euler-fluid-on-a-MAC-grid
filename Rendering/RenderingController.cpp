#include "RenderingController.h"
#include "../Fluid/SimulationController.h"
#include "VelocityRenderer.h"
#include "DensityRenderer.h"
#include "Definitions.h"

//#include <QtGui/QOpenGLShaderProgram>
#include <QTGui/QOpenGLVertexArrayObject>
#include <QTGui/QOpenGLBuffer>

RenderingController::RenderingController(QWidget *parent, glm::vec2 windowSize, SimulationController* simCon)
: QOpenGLWidget(parent) 
, m_WindowSize(windowSize)
, m_SimCon(simCon)
, m_Mode(FluidMode::showDensities)
{
	m_Format = new QSurfaceFormat();
	m_Format->setVersion(4, 3);
	m_Format->setProfile(QSurfaceFormat::CoreProfile);
	m_Format->setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	m_Format->setSwapInterval(1);
    QSurfaceFormat::setDefaultFormat(*m_Format);
	this->setFormat(*m_Format);

	m_Context = new QOpenGLContext(this);
    m_Context->setFormat(*m_Format);
    m_Context->create();

	setMinimumSize(windowSize.x, windowSize.y);

	m_VelRenderer = new VelocityRenderer(m_SimCon);
	m_DensityRenderer = new DensityRenderer(m_SimCon);
}	

RenderingController::~RenderingController(void)
{
	delete m_Format;
	delete m_Context;
	delete m_VelRenderer;
	delete m_DensityRenderer;
}

void RenderingController::initializeGL()
{
	initializeOpenGLFunctions();

	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
	f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//create buffers
	m_Vao = new QOpenGLVertexArrayObject();
	m_Vao->create();
	m_Vao->bind();
	m_VelRenderer->init();
	m_DensityRenderer->init();
	m_Vao->release();
}

void RenderingController::resizeGL(int w, int h) 
{

}

void RenderingController::paintGL()
{
	QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
	f->glClear(GL_COLOR_BUFFER_BIT);

	m_Vao->bind();
	if (m_Mode == FluidMode::showDensities) m_DensityRenderer->render(f);
	else if (m_Mode == FluidMode::showVelocities) m_VelRenderer->render(f);
	m_Vao->release();
}

void RenderingController::setMode(UpdateStruct& data)
{
	m_Mode = data.fluidMode;
	m_DensityRenderer->setColor(data.color);
}
