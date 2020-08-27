#define _USE_MATH_DEFINES

#include "Grid3D.h"

Grid3D::Grid3D(uint32_t cellCountX, uint32_t cellCountY, uint32_t cellCountZ)
: BaseGrid(cellCountX, cellCountY), m_CellCountZ(cellCountZ)
{
	assert(m_CellCountX > 0);
	assert(m_CellCountY > 0);
	assert(m_CellCountZ > 0);

	uint32_t arraySize = m_CellCountX*m_CellCountY*m_CellCountZ;

	m_Pressure = new float[arraySize];
	memset(m_Pressure, 0, sizeof(float) * arraySize);

	m_Position = new vec3[(m_CellCountX+1)*(m_CellCountY+1)*(m_CellCountZ+1)];
}

Grid3D::~Grid3D(void)
{
	delete[] m_Pressure;
	delete[] m_Velocity;
	delete[] m_Position;
}

void Grid3D::advectVelocities(UpdateStruct& data)
{
	
}

void Grid3D::switchQuantities()
{

}

void Grid3D::initTemperature()
{

}

void Grid3D::initVelocityField()
{

}

void Grid3D::initGridPositions()
{

}