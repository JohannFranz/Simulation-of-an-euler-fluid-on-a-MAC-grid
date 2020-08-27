//#include "Bachelorarbeit.h"
//
//Bachelorarbeit::Bachelorarbeit(QWidget *parent)
//	: QMainWindow(parent)
//{
//	ui.setupUi(this);
//}

#include "Bachelorarbeit.h"
#include "Fluid/SimulationController.h"
#include "Rendering\RenderingController.h"
#include "Definitions.h"

#include <sys/timeb.h>
#include <time.h>
#include <cstdio>

#include <QTGui/QMouseEvent>
#include <QTWidgets\QColorDialog>

Bachelorarbeit::Bachelorarbeit(QWidget *parent)
: QMainWindow(parent)
, m_FrameTimer(0.0)
, m_FrameCount(0)
, m_Initialized(false)
, m_Paused(false)
{
	m_UpdateData.addInk = false;
	m_UpdateData.addForce = false;
	m_UpdateData.projectMode = ProjectMode::useGaussSeidel;
	m_UpdateData.intMode = IntegrationMode::useRK;
	m_UpdateData.solverIterations = 50;

	m_UpdateData.splatRadiusDensity = 0.005f;
	m_UpdateData.splatRadiusVelocity = 0.005f;
	m_UpdateData.dissipationVelocity = 0.99f;
	m_UpdateData.dissipationDensity = 0.99f;

	m_UpdateData.color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	m_UpdateData.velocityMultiplicator = 50000.0f;

	initGUI();
	
	m_WindowSize = glm::vec2(m_Ui->oglContainer->width(), m_Ui->oglContainer->height());
	m_SimCon = new SimulationController(32, 32);
	m_RenderCon = new RenderingController(m_Ui->oglContainer, m_WindowSize, m_SimCon);

	// Setup the timer
    m_Timer.setInterval(MIN_TIME_DELTA_IN_MILLISEC);
	m_Timer.setTimerType(Qt::PreciseTimer);
    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(update()));
    m_Timer.start();
	m_Initialized = true;
}

Bachelorarbeit::~Bachelorarbeit(void)
{
	delete m_RenderCon;
	delete m_SimCon;
}

void Bachelorarbeit::initGUI()
{
	m_Ui = new Ui::BachelorarbeitClass();
	m_Ui->setupUi(this);
	setWindowTitle("Fluid Simulator");

	m_Ui->rbDensity->setChecked(true);
	m_Ui->rbRK2->setChecked(true);
	m_Ui->rbGauss->setChecked(true);

	m_Ui->hsDensityRadius->setValue(m_UpdateData.splatRadiusDensity * 10000.0f);
	m_Ui->hsVelocityRadius->setValue(m_UpdateData.splatRadiusVelocity * 10000.0f);

	m_Ui->hsDensityDissipation->setValue(m_UpdateData.dissipationDensity * 100.0f);
	m_Ui->hsVelocityDissipation->setValue(m_UpdateData.dissipationVelocity * 100.0f);

	m_Ui->hsMultiplicator->setValue(m_UpdateData.velocityMultiplicator / 100.0f);
	m_Ui->hsIterations->setValue(m_UpdateData.solverIterations);

	connect(m_Ui->pbReset, SIGNAL(clicked()), this, SLOT(reset()));
	connect(m_Ui->pbColor, SIGNAL(clicked()), this, SLOT(color()));
	connect(m_Ui->pbPause, SIGNAL(clicked()), this, SLOT(pause()));
}

void Bachelorarbeit::pause()
{
	if (!m_Paused) m_Ui->pbPause->setText("Unpause");
	else m_Ui->pbPause->setText("Pause"); 

	m_Paused = !m_Paused;
}

void Bachelorarbeit::reset()
{
	m_SimCon->reset();
}

void Bachelorarbeit::color()
{
	QColor col = QColorDialog::getColor(Qt::blue, this);

	m_UpdateData.color.r = col.redF();
	m_UpdateData.color.g = col.greenF();
	m_UpdateData.color.b = col.blueF();
	m_UpdateData.color.a = col.alphaF();
}

void Bachelorarbeit::update() 
{
	if (!m_Initialized) return;
	
	#ifdef _DEBUG
	setWindowTitle(QString("Fluid Simulator - Mouse Pos: x = " + 
					QString::number(m_CurrentMousePosition.x) + 
					" y = " + QString::number(m_CurrentMousePosition.y)));
	#endif

	m_UpdateData.timeDelta += getTimeDelta();

	
	if ( m_UpdateData.timeDelta > MAX_TIME_DELTA_IN_SEC ) m_UpdateData.timeDelta = 0.1;

	if (m_Ui->rbDensity->isChecked()) m_UpdateData.fluidMode = FluidMode::showDensities;
	else m_UpdateData.fluidMode = FluidMode::showVelocities;
	
	if (m_Ui->rbEuler->isChecked()) m_UpdateData.intMode = IntegrationMode::useEuler;
	else m_UpdateData.intMode = IntegrationMode::useRK;

	if (m_Ui->rbGauss->isChecked()) m_UpdateData.projectMode = ProjectMode::useGaussSeidel;
	else if (m_Ui->rbJacobi->isChecked()) m_UpdateData.projectMode = ProjectMode::useJacobi;
	else m_UpdateData.projectMode = ProjectMode::noProject;

	m_UpdateData.splatRadiusDensity = m_Ui->hsDensityRadius->value() / 10000.0f;
	m_UpdateData.splatRadiusVelocity = m_Ui->hsVelocityRadius->value() / 10000.0f;
	m_Ui->lblDensityRadius->setText(QString::number(m_UpdateData.splatRadiusDensity));
	m_Ui->lblVelocityRadius->setText(QString::number(m_UpdateData.splatRadiusVelocity));

	m_UpdateData.dissipationDensity = m_Ui->hsDensityDissipation->value() / 100.0f;
	m_UpdateData.dissipationVelocity = m_Ui->hsVelocityDissipation->value() / 100.0f;
	m_Ui->lblDensityDissipation->setText(QString::number(m_UpdateData.dissipationDensity));
	m_Ui->lblVelocityDissipation->setText(QString::number(m_UpdateData.dissipationVelocity));

	m_UpdateData.solverIterations = m_Ui->hsIterations->value();
	m_Ui->lblIterations->setText(QString::number(m_UpdateData.solverIterations));
	
	m_UpdateData.velocityMultiplicator = m_Ui->hsMultiplicator->value() * 100.0f;
	m_Ui->lblMultiplicator->setText(QString::number(m_Ui->hsMultiplicator->value()));
	
	glm::vec2 pos;
	if (m_UpdateData.addInk)
	{
		//convert mouse position to normalized interval [0,1]
		pos.x = m_CurrentMousePosition.x / m_WindowSize.x;
		pos.y = 1.0f - m_CurrentMousePosition.y/m_WindowSize.y;
		m_UpdateData.mousePos = pos;
	}

	if (!m_Paused) 
	{
		m_SimCon->update(m_UpdateData);
	}

	m_UpdateData.timeDelta = 0.0;
	
	m_FrameCount++;
	if (m_FrameCount == 2)
	{
		m_RenderCon->setMode(m_UpdateData);
		m_RenderCon->update();
		m_FrameCount = 0;
	}
}

void Bachelorarbeit::mousePressEvent(QMouseEvent* event)
{
	if (event->MouseButtonPress == QEvent::MouseButtonPress)
	{
		m_CurrentMousePosition.x = event->localPos().x();
		m_CurrentMousePosition.y = event->localPos().y()-10.0f;
		m_FormerMousePosition = m_CurrentMousePosition;
		m_UpdateData.addInk = true;
	}
}

void Bachelorarbeit::mouseReleaseEvent(QMouseEvent* event)
{
	m_CurrentMousePosition.x = 0;
	m_CurrentMousePosition.y = 0;
	m_FormerMousePosition = m_CurrentMousePosition;
	m_UpdateData.addInk = false;
	m_UpdateData.addForce = false;
}

void Bachelorarbeit::mouseMoveEvent(QMouseEvent* event)
{
	m_FormerMousePosition = m_CurrentMousePosition;

	m_CurrentMousePosition.x = event->localPos().x();
	m_CurrentMousePosition.y = event->localPos().y()-10.0f;

	//set Movement-vector to Interval [-1, 1]
	m_MoveDifference = m_CurrentMousePosition - m_FormerMousePosition;
	m_MoveDifference.x /= m_WindowSize.x;
	m_MoveDifference.y /= -m_WindowSize.y;
	m_UpdateData.mouseMove = m_MoveDifference;

	m_UpdateData.addForce = true;
}

//method created by Christof Rezk-Salama
//Künstliche Intelligenz Übung 2 - Musterlösung
double Bachelorarbeit::getTimeDelta() {

	__time64_t ltime;
	struct __timeb64 tstruct;

	static bool bFirstTime = true;
	static long nStartMilliSeconds;
	static long nMilliSeconds = 0;
	static long nStartCount   = 0;

	if (bFirstTime) {
		_time64( &ltime );
		nStartMilliSeconds = (unsigned long) ltime;
		nStartMilliSeconds *= 1000;
		_ftime64_s( &tstruct );
		nStartMilliSeconds += tstruct.millitm;        
		bFirstTime = false;
		nStartCount = nStartMilliSeconds;
	}

	_time64( &ltime );
	nMilliSeconds = (unsigned long) ltime;
	nMilliSeconds *= 1000;
	_ftime64_s( &tstruct );
	nMilliSeconds += tstruct.millitm;        
	bFirstTime = false;

	//nMilliSeconds -= nStartMilliSeconds;
	//printf( "Plus milliseconds:%u\n",nMilliSeconds-nStartCount);

	double deltaTime = double(nMilliSeconds-nStartCount)/1000.0;
	nStartCount = nMilliSeconds;

	if ( deltaTime < 0 )	return 0.0;

	return deltaTime;
}
