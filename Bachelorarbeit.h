//#pragma once
//
//#include <QtWidgets/QMainWindow>
//#include "ui_Bachelorarbeit.h"
//
//class Bachelorarbeit : public QMainWindow
//{
//	Q_OBJECT
//
//public:
//	Bachelorarbeit(QWidget *parent = Q_NULLPTR);
//
//private:
//	Ui::BachelorarbeitClass ui;
//};

#pragma once
#include "ui_Bachelorarbeit.h"
#include "glm.hpp"
#include "Definitions.h"

#include <QtWidgets/QMainWindow>
#include <QtCore\qtimer.h>

class SimulationController;
class RenderingController;

class Bachelorarbeit : public QMainWindow
{
	Q_OBJECT

public:
								Bachelorarbeit(QWidget *parent = Q_NULLPTR);
								~Bachelorarbeit(void);
	
	void						initGUI();

public slots:
	void						update();
	void						reset();
	void						color();
	void						pause();

protected:
	double						getTimeDelta();

	void						mousePressEvent(QMouseEvent *event) override;
	void						mouseReleaseEvent(QMouseEvent *event) override;
	void						mouseMoveEvent(QMouseEvent *) override;

private:
	SimulationController*		m_SimCon;
	RenderingController*		m_RenderCon;
	double						m_TimeDelta;
	QTimer						m_Timer;

	Ui::BachelorarbeitClass*	m_Ui;

	glm::vec2					m_WindowSize;
	double						m_FrameTimer;
	int							m_FrameCount;

	UpdateStruct				m_UpdateData;
	glm::vec2					m_CurrentMousePosition;
	glm::vec2					m_FormerMousePosition;
	glm::vec2					m_MoveDifference; //in Interval [-1, 1]

	bool						m_Initialized;
	bool						m_Paused;
};

