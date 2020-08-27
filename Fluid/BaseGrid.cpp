#include "BaseGrid.h"

BaseGrid::BaseGrid(uint32_t cellCountX, uint32_t cellCountY)
: m_CellCountX(cellCountX)
, m_CellCountY(cellCountY)
, m_Width(1.0f)
, m_RealCountX(m_CellCountX+2)
, m_RealCountY(m_CellCountY+2)
{
}

BaseGrid::~BaseGrid(void)
{
	delete[] m_Pressure;
	//delete[] m_PressurePrev;

	/*delete[] m_Temperature;
	delete[] m_TemperaturePrev;*/

	delete[] m_Density;
	delete[] m_DensityPrev;
}

//the linear weight is used for linear interpolation along each axis
//min represents the minimum value in the cell along x, y or z axis
//max represents the maximum value
float BaseGrid::getLinearWeight(float min, float max)
{
	if (m_Width == 1.0f) return max - min;

	return (max - min) / m_Width;
}

uint32_t BaseGrid::getHorizontalCellCount()
{
	return m_CellCountX;
}

uint32_t BaseGrid::getVerticalCellCount()
{
	return m_CellCountY;
}

float* BaseGrid::getPressure()
{
	return m_Pressure;
}

float* BaseGrid::getTemperature()
{
	return m_Temperature;
}

float* BaseGrid::getDensity()
{ 
	return m_Density;
}