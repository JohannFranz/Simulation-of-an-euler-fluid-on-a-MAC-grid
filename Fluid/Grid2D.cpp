#define _USE_MATH_DEFINES

#include "Grid2D.h"
#include "gtx/compatibility.hpp"
#include "../Definitions.h"

//cellCountX is the number of horizontal cells (not gridpoints)
//cellCountY is the number of vertical cells
Grid2D::Grid2D(uint32_t cellCountX, uint32_t cellCountY)
: BaseGrid(cellCountX, cellCountY)
{
	assert(m_CellCountX > 0);
	assert(m_CellCountY > 0);

	uint32_t size = m_RealCountX*m_RealCountY;


	//initialize central quantities
	m_Pressure = new float[size];
	memset(m_Pressure, 0, sizeof(float) * size);

	m_Density = new float[size];
	memset(m_Density, 0, sizeof(float) * size);

	m_DensityPrev = new float[size];
	memset(m_DensityPrev, 0, sizeof(float) * size);

	//initialize u component of velocity 
	size = (m_RealCountX + 1)*m_RealCountY;
	m_Velocity_U = new float[size];
	memset(m_Velocity_U, 0, sizeof(float) * size);
	
	m_Velocity_U_Prev = new float[size];
	memset(m_Velocity_U_Prev, 0, sizeof(float) * size);

	m_Velocity_U_Backup = new float[size];
	memset(m_Velocity_U_Backup, 0, sizeof(float) * size);

	//initialize v component of velocity 
	size = m_RealCountX*(m_RealCountY + 1);
	m_Velocity_V = new float[size];
	memset(m_Velocity_V, 0, sizeof(float) * size);
	
	m_Velocity_V_Prev = new float[size];
	memset(m_Velocity_V_Prev, 0, sizeof(float) * size);

	m_Velocity_V_Backup = new float[size];
	memset(m_Velocity_V_Backup, 0, sizeof(float) * size);

	//allocate positions
	//Positions for grid points and cell centers are saved in one array to speed up
	//caching and implementation processes. The first block are positions for cell centers
	//The second block are positions for actual grid points.
	//cell center positions are used for force and quantity advection
	//grid point positions are used for vector advection
	m_CenterPositionOffset = m_RealCountX*m_RealCountY;
	size = m_CenterPositionOffset + (m_RealCountX + 1)*(m_RealCountY + 1);
	m_Position = new vec2[size];
}

Grid2D::~Grid2D(void)
{
	delete[] m_Velocity_U;
	delete[] m_Velocity_U_Prev;
	delete[] m_Velocity_U_Backup;

	delete[] m_Velocity_V;
	delete[] m_Velocity_V_Prev;
	delete[] m_Velocity_V_Backup;

	delete[] m_Position;
}

void Grid2D::initTemperature()
{
	
}

//Just for debug purposes
void Grid2D::initDensity()
{
	/*float component = 1.00f;
	for (uint32_t j = 0; j < m_RealCountY; j++)
	{
		for (uint32_t i = 0; i < m_RealCountX; i++)
		{
			m_Density[i + m_RealCountX*j] = component;
			m_DensityPrev[i + m_RealCountX*j] = component;
		}
	}*/
}

//Just for debug purposes
void Grid2D::initVelocityField()
{
	//init u components
	/*float component = -1.0f;
	for (uint32_t j = 0; j < m_RealCountY; j++)
	{
		for (uint32_t i = 0; i < m_RealCountX; i++)
		{
			m_Velocity_V[i + m_RealCountX*j] = component;
			m_Velocity_V_Prev[i + m_RealCountX*j] = component;
			m_Velocity_V_Backup[i + m_RealCountX*j] = component;
		}
	}*/
	//m_Velocity_U_Backup[10] = component;
	
}

void Grid2D::initGridPositions()
{
	int indexCountValidator = 0;
	vec2 pos = vec2(0.5f, 0.5f);
	for (uint32_t j = 0; j < m_RealCountY; j++)
	{
		for (uint32_t i = 0; i < m_RealCountX; i++)
		{
			m_Position[i + m_RealCountX*j] = pos;
			pos.x++;
			indexCountValidator++;
		}
		pos.y++;
		pos.x = 0.5f;
	}


	pos.y = 0.0f;
	for (uint32_t j = 0; j < m_RealCountY+1; j++)
	{
		pos.x = 0.0f;
		for (uint32_t i = 0; i < m_RealCountX+1; i++)
		{
			m_Position[m_CenterPositionOffset + i + (m_RealCountX+1)*j] = pos;
			pos.x++;
			indexCountValidator++;
		}
		pos.y++;
	}

	assert(indexCountValidator == m_RealCountX*m_RealCountY + (m_RealCountX+1)*(m_RealCountY+1));
}

void Grid2D::reset()
{
	uint32_t size = m_RealCountX*m_RealCountY;
	memset(m_Density, 0, sizeof(float) * size);
	memset(m_DensityPrev, 0, sizeof(float) * size);

	size = (m_RealCountX + 1)*m_RealCountY;
	memset(m_Velocity_V, 0, sizeof(float) * size);
	memset(m_Velocity_V_Prev, 0, sizeof(float) * size);
	memset(m_Velocity_V_Backup, 0, sizeof(float) * size);

	size = m_RealCountX*(m_RealCountY + 1);
	memset(m_Velocity_U, 0, sizeof(float) * size);
	memset(m_Velocity_U_Prev, 0, sizeof(float) * size);
	memset(m_Velocity_U_Backup, 0, sizeof(float) * size);
}

void Grid2D::addDensity(UpdateStruct& data)
{
	uint32_t index;
	float nominator;
	glm::vec2 diff, gridPos;
	for (uint32_t j = 1; j <= m_CellCountY; j++)
	{
		for (uint32_t i = 1; i <= m_CellCountX; i++)
		{
			index = i+j*m_RealCountX;

			gridPos = m_Position[index];
			gridPos.x /= m_RealCountX;
			gridPos.y /= m_RealCountY;
			
			diff = gridPos - data.mousePos;

			nominator = glm::dot(diff, diff);
			m_Density[index] += exp(-nominator/data.splatRadiusDensity);
		}
	}
}

void Grid2D::addVelocity(UpdateStruct& data)
{
	uint32_t index;
	float nominator;
	glm::vec2 velocityPos, diff, dir;
	dir = data.mouseMove * data.velocityMultiplicator;
	for (uint32_t j = 1; j <= m_CellCountY; j++)
	{
		for (uint32_t i = 1; i <= m_CellCountX+1; i++)
		{
			index = i+j*(m_RealCountX+1);
			velocityPos = m_Position[m_CenterPositionOffset + index]
						+ m_Position[m_CenterPositionOffset + index + m_RealCountX+1];
			velocityPos *= 0.5f;

			velocityPos.x /= m_RealCountX;
			velocityPos.y /= m_RealCountY;
			
			diff = velocityPos-data.mousePos;
			nominator = glm::dot(diff, diff);
			m_Velocity_U_Backup[index] += data.timeDelta * dir.x * exp(-nominator/data.splatRadiusVelocity);
		}
	}

	for (uint32_t j = 1; j <= m_CellCountY+1; j++)
	{
		for (uint32_t i = 1; i <= m_CellCountX; i++)
		{
			index = i+j*(m_RealCountX+1);
			velocityPos = m_Position[m_CenterPositionOffset + index]
						+ m_Position[m_CenterPositionOffset + index + 1];
			velocityPos *= 0.5f;
			velocityPos.x /= m_RealCountX;
			velocityPos.y /= m_RealCountY;
			diff = velocityPos-data.mousePos;
			nominator = glm::dot(diff, diff);
			m_Velocity_V_Backup[i+j*m_RealCountX] += data.timeDelta * dir.y * exp(-nominator/data.splatRadiusVelocity);
		}
	}
}

void Grid2D::advectVelocities(UpdateStruct& data)
{
	uint32_t loopSize = (m_CellCountX+1);
	for (uint32_t j = 1; j <= m_CellCountY; j++)
	{
		for (uint32_t i = 1; i <= loopSize; i++)
		{
			advectVelocityU(i,j, data);
		}
	}

	loopSize = (m_CellCountY+1);
	for (uint32_t j = 1; j <= loopSize; j++)
	{
		for (uint32_t i = 1; i <= m_CellCountX; i++)
		{
			
			advectVelocityV(i,j, data);
		}
	}
}

void Grid2D::advectCentralQuantities(UpdateStruct& uData)
{
	QuantityData data;
	data.quantity = m_Density;
	data.prevQuantity = m_DensityPrev;

	for (uint32_t j = 1; j <= m_CellCountY; j++)
	{
		for (uint32_t i = 1; i <= m_CellCountX; i++)
		{
			data.i = i;
			data.j = j;
			advectCentralQuantity(data, uData);
		}
	}
}

glm::vec2 Grid2D::integrateParticlePosition(UpdateStruct& data, vec2& position, vec2& velocity)
{
	if (data.intMode == IntegrationMode::useEuler)
	{
		return position - (float)data.timeDelta *velocity;
	}

	uint32_t iNew = UINT_MAX, jNew = UINT_MAX;
	//Runge Kutta step 1:
	vec2 particlePos = position - (float)data.timeDelta*velocity*0.5f;

	//get velocity for temporary position
	getIndicesForCell(particlePos, iNew, jNew);
	assert(iNew != UINT_MAX);
	assert(jNew != UINT_MAX);
	velocity.x = getPrevParticleVelocityU(particlePos, iNew, jNew);
	velocity.y = getPrevParticleVelocityV(particlePos, iNew, jNew);

	//Runge Kutta step 2:
	return position - (float)data.timeDelta*velocity;
}

//different quantities are stored inside the center of a cell, but they are advected
//in the same way
void Grid2D::advectCentralQuantity(QuantityData& qData, UpdateStruct& uData)
{
	if (m_Density[qData.i + qData.j*m_RealCountX] > 0.0)
	{
		int test = 0;
	}
	assert(qData.i > 0 && qData.i < m_RealCountX);
	assert(qData.j > 0 && qData.j < m_RealCountY);

	vec2 centralPos;
	vec2 velocity;
	vec2 particlePos;
	uint32_t iNew, jNew, iOld, jOld;
	iNew = jNew = UINT_MAX;
	//1. calculate central position (cp)
	getCentralPosition(centralPos, qData.i, qData.j);

	//2. get velocity for cp
	getVelocityForCentralPosition(velocity, qData.i, qData.j);
	

	//3. back trace from cp to former position (fp)
	particlePos = integrateParticlePosition(uData, centralPos, velocity); 

	qData.particlePos = particlePos;

	//4. get indices for new cell
	getIndicesForCell(particlePos, iNew, jNew);
	assert(iNew != UINT_MAX);
	assert(jNew != UINT_MAX);
	iOld = qData.i;
	jOld = qData.j;
	qData.i = iNew;
	qData.j = jNew;

	//5. interpolate quantity value in fp
	float value = getPrevParticleQuantity(qData);
	qData.quantity[iOld + m_RealCountX*jOld] = uData.dissipationDensity * value;
}

void Grid2D::advectVelocityU(uint32_t i, uint32_t j, UpdateStruct& uData)
{
	assert(i > 0 && i < m_RealCountX);
	assert(j > 0 && j < m_RealCountY);

	vec2 velocityPos;
	vec2 velocity;
	vec2 particlePos;
	uint32_t index = j*(m_RealCountX+1)+i + m_CenterPositionOffset;
	uint32_t iNew, jNew;
	iNew = jNew = UINT_MAX;
	
	//1. calculate position of left vertical grid face center (gfc) 
	velocityPos = (m_Position[index] + m_Position[index+(m_RealCountX+1)]) * 0.5f;

	//2. get velocity for gfc
	getVelocityForGridFaceX(velocity, i, j);

	//3. back trace from cp to former position (fp)
	particlePos = integrateParticlePosition(uData, velocityPos, velocity); 

	//4. get indices for new cell
	getIndicesForCell(particlePos, iNew, jNew);
	assert(iNew != UINT_MAX);
	assert(jNew != UINT_MAX);

	//5. interpolate velocity value in fp
	velocity.x = getPrevParticleVelocityU(particlePos, iNew, jNew);
	m_Velocity_U_Backup[i + (m_RealCountX+1)*j] = uData.dissipationVelocity * velocity.x;
}

void Grid2D::advectVelocityV(uint32_t i, uint32_t j, UpdateStruct& uData)
{
	assert(i > 0 && i < m_RealCountX);
	assert(j > 0 && j < m_RealCountY);

	vec2 velocityPos;
	vec2 velocity;
	vec2 particlePos;
	uint32_t index = j*(m_RealCountX+1)+i + m_CenterPositionOffset;
	uint32_t iNew, jNew;
	iNew = jNew = UINT_MAX;
	
	//1. calculate position of lower horizontal grid face center (gfc)
	velocityPos = (m_Position[index] + m_Position[index+1]) * 0.5f;

	//2. get velocity for gfc
	getVelocityForGridFaceY(velocity, i, j);

	//3. back trace from cp to former position (fp)
	particlePos = integrateParticlePosition(uData, velocityPos, velocity); 

	//4. get indices for new cell
	getIndicesForCell(particlePos, iNew, jNew);
	assert(iNew != UINT_MAX);
	assert(jNew != UINT_MAX);

	//5. interpolate velocity value in fp
	velocity.y = getPrevParticleVelocityV(particlePos, iNew, jNew);
	m_Velocity_V_Backup[i + (m_RealCountX)*j] = uData.dissipationVelocity * velocity.y;
}

void Grid2D::getCentralPosition(vec2& centralPos, uint32_t i, uint32_t j)
{
	centralPos = m_Position[i + j * m_RealCountX];
}

void Grid2D::getVelocityForCentralPosition(vec2& velocity, uint32_t i, uint32_t j)
{
	assert(i > 0 && i < m_RealCountX);
	assert(j > 0 && j < m_RealCountY);

	int index = i + j * (m_RealCountX + 1);
	velocity.x = m_Velocity_U[index] + m_Velocity_U[index+1];

	index = i + j * m_RealCountX;
	velocity.y = m_Velocity_V[index] + m_Velocity_V[index+m_RealCountX];

	velocity *= 0.5f;
}

void Grid2D::getVelocityForGridFaceX(vec2& velocity, uint32_t i, uint32_t j)
{
	assert(i > 0 && i < m_RealCountX);
	assert(j > 0 && j < m_RealCountY);

	//set x component of velocity vector
	int index = i + j * (m_RealCountX + 1);
	velocity.x = m_Velocity_U[index];
	if (velocity.x > 999999999.0f)
	{
		float test = 4.0f;
	} 

	//interpolate y coordinates
	index = i + j * m_RealCountX;
	float lerpVal1 = m_Velocity_V[index] + m_Velocity_V[index + m_RealCountX];
	lerpVal1 *= 0.5f;
		
	//bilinear interpolation
	float lerpVal2 = m_Velocity_V[index - 1] + m_Velocity_V[index + m_RealCountX - 1];
	lerpVal2 *= 0.5f;
	velocity.y = lerpVal1 + lerpVal2;
	velocity.y *= 0.5f;
}

void Grid2D::getVelocityForGridFaceY(vec2& velocity, uint32_t i, uint32_t j)
{
	assert(i > 0 && i < m_RealCountX);
	assert(j > 0 && j < m_RealCountY);

	//set y component of velocity vector
	int index = i + j * (m_RealCountX);
	velocity.y = m_Velocity_V[index];
	
	//interpolate x coordinates
	index = i + j * (m_RealCountX+1);
	float lerpVal1 = m_Velocity_U[index] + m_Velocity_U[index + 1];
	lerpVal1 *= 0.5f;
		
	//bilinear interpolation
	float lerpVal2 = m_Velocity_U[index - (m_RealCountX+1)] + m_Velocity_U[index - m_RealCountX];
	lerpVal2 *= 0.5f;
	velocity.x = lerpVal1 + lerpVal2;
	velocity.x *= 0.5f;
}

void Grid2D::getIndicesForCell(const vec2& pos, uint32_t& i, uint32_t& j)
{
	if (pos.x <= 0.0f) 
	{
		i = 0;
	} else if (pos.x >= m_Position[m_RealCountX+m_CenterPositionOffset].x)
	{
		i = m_RealCountX-1; 
	} else {
		for (uint32_t x = 0; x <= m_RealCountX; x++)
		{
			if (m_Position[x+m_CenterPositionOffset].x <= pos.x) continue;

			i = --x; break;
		}
	}

	if (pos.y <= 0.0f) 
	{
		j = 0; 
		return;
	} else if (pos.y >= m_Position[m_CenterPositionOffset + m_RealCountY * (m_RealCountX+1)].y)
	{
		j = m_RealCountY-1; 
		return;
	} 

	uint32_t index = 0;
	for (uint32_t y = 0; y <= m_RealCountY; y++, index += m_RealCountX+1)
	{
		if (m_Position[m_CenterPositionOffset + index].y <= pos.y) continue;

		j = --y; break;
	}
}

float Grid2D::lerpHorizontalQuantity(QuantityData& data)
{
	vec2 horizontalNeighbour(0.0f, 0.0f);
	float alpha;

	//check if current cell is at a border
	if (data.indexIncrease == 0)
	{
		//no lerp possible, so the current quantity stays the way it is
		return data.centralQuantity;
	}

	//1. check if lerp with right neighbour is necessary
	if (data.indexIncrease > 0)
	{
		uint32_t neighbourI = data.i + data.indexIncrease;
		getCentralPosition(horizontalNeighbour, neighbourI, data.j);
		alpha = getLinearWeight(data.particlePos.x, horizontalNeighbour.x);
		return glm::lerp(getCentralQuantity(neighbourI, data.j, data.quantity), data.centralQuantity, alpha);
	} 

	//2. lerp with left neighbour
	uint32_t neighbourI = data.i + data.indexIncrease;
	getCentralPosition(horizontalNeighbour, neighbourI, data.j);
	alpha = getLinearWeight(horizontalNeighbour.x, data.particlePos.x);
	return glm::lerp(getCentralQuantity(neighbourI, data.j, data.quantity), data.centralQuantity, alpha);
}

float Grid2D::lerpVerticalQuantity(QuantityData& data)
{
	vec2 verticalNeighbour(0.0f, 0.0f);
	float alpha;

	//check if current cell is at a border
	if (data.indexIncrease == 0)
	{
		//no lerp possible, so the current quantity stays the way it is
		return data.centralQuantity;
	}

	//1. check if lerp with above neighbour is necessary
	if (data.indexIncrease > 0)
	{
		uint32_t neighbourJ = data.j + data.indexIncrease;
		getCentralPosition(verticalNeighbour, data.i, neighbourJ);
		alpha = getLinearWeight(data.particlePos.y, verticalNeighbour.y);
		return glm::lerp(getCentralQuantity(data.i, neighbourJ, data.quantity), data.centralQuantity, alpha);
	} 

	//2. lerp with below neighbour
	uint32_t neighbourJ = data.j + data.indexIncrease;
	getCentralPosition(verticalNeighbour, data.i, neighbourJ);
	alpha = getLinearWeight(verticalNeighbour.y, data.particlePos.y);
	return glm::lerp(getCentralQuantity(data.i, neighbourJ, data.quantity), data.centralQuantity, alpha);
}

//float Grid2D::getPrevParticleQuantity(vec2 centralPos, vec2 particlePos, uint32_t i, uint32_t j, float* quantity)
float Grid2D::getPrevParticleQuantity(QuantityData& data)
{
	float horizontalLerp1, horizontalLerp2, beta;
	vec2 verticalNeighbour, diagonalNeighbour, centralPos;
	uint32_t i = data.i, j = data.j;

	//calculate former central position (cp)
	getCentralPosition(centralPos, data.i, data.j);

	//1. get the quantity of the central point in the cell the previous particle is in
	data.centralQuantity = getCentralQuantity(data.i, data.j, data.prevQuantity);

	//2. get the horizontal neighbour (left or right to current cell) and its central position
	//additionally calculate the horizontal interpolation weight and value
	data.indexIncrease = 0;
	if (centralPos.x < data.particlePos.x)
	{
		if (i < m_RealCountX-1) 
		{
			data.indexIncrease = 1;
			++i;
		}
	} else 
	{
		if (i > 0)
		{
			data.indexIncrease = -1;
			--i;
		}
	}
	horizontalLerp1 = lerpHorizontalQuantity(data);
	
	//3. get the vertical neighbour (above or underneath current cell) and its central position
	//additionally calculate the vertical interpolation weight
	if (centralPos.y < data.particlePos.y)
	{
		if (j < m_RealCountY-1) ++j; 
	} else 
	{
		if (j > 0) --j;
	}
	getCentralPosition(verticalNeighbour, i, j);
	beta = getLinearWeight(data.particlePos.y, verticalNeighbour.y);
	if (beta < 0.0f) beta *= -1.0f;
	
	//4. get the diagonal neighbour with its central position and quantity
	getCentralPosition(diagonalNeighbour, i, j);
	float diagonalQuantity = getCentralQuantity(i, j, data.prevQuantity);
	
	//5. interpolate horizontally between vertical and diagonal neighbour
	//the values in data struct have to be changed to represent the vertical neighbour.
	//starting with the vertical neighbour a horizontal lerp is done with the diagonal neighbour
	int tempJ = data.j;
	data.j = j;
	data.centralQuantity = getCentralQuantity(data.i, j, data.prevQuantity);
	horizontalLerp2 = lerpHorizontalQuantity(data);

	//6. get final quantity
	/*if ((int)data.j - tempJ > 0)
		return glm::lerp(horizontalLerp1, horizontalLerp2, beta);
*/
	return glm::lerp(horizontalLerp2, horizontalLerp1, beta);
}

float Grid2D::getPrevParticleVelocityU(const vec2& particlePos, uint32_t i, uint32_t j)
{
	assert(i < m_RealCountX);
	assert(j < m_RealCountY);
	float alpha = 0.0f, beta, lerpVal1, lerpVal2;
	float yCoordUComponent;
	uint32_t cellIndex = i + j*(m_RealCountX+1);
	
	vec2 gridPointUpperLeftCorner = m_Position[m_CenterPositionOffset + cellIndex + (m_RealCountX+1)];
	//calculate y component of the center position in the left face
	yCoordUComponent = gridPointUpperLeftCorner.y - m_Width*0.5f;
	
	if (particlePos.x < 0.0f) 
	{
		lerpVal1 = m_Velocity_U_Prev[cellIndex];
	} else if (particlePos.x > m_Position[m_CenterPositionOffset + m_RealCountX].x)
	{
		lerpVal1 = m_Velocity_U_Prev[(j+1)*(m_RealCountX+1) - 1];
		alpha = 1.0f;
	} else 
	{
		alpha = getLinearWeight(gridPointUpperLeftCorner.x, particlePos.x);
		lerpVal1 = glm::lerp(m_Velocity_U_Prev[cellIndex], m_Velocity_U_Prev[cellIndex + 1], alpha);
	}
	
	beta = getLinearWeight(particlePos.y, yCoordUComponent);

	//the u component needs a bilinear interpolation with either a cell above or below the 
	//current one. If beta value is below 0 then the particle is above the center position
	//and the bilinear interpolation is done with the upper cell
	if (beta < 0.0f)
	{
		//check if point is at upper border and only simple lerp is possible
		if (j == (m_RealCountY-1))
		{
			return lerpVal1;
		}

		//interpolate between the cell above and the diagonal cell
		beta *= -1.0f;
		cellIndex = i + (j+1)*(m_RealCountX+1);
		lerpVal2 = glm::lerp(m_Velocity_U_Prev[cellIndex], m_Velocity_U_Prev[cellIndex + 1], alpha);
		return glm::lerp( lerpVal1, lerpVal2, beta);
	} else
	{
		//check if point is at lower border and only simple lerp is possible
		if (j == 0)
		{
			return lerpVal1;
		}

		//interpolate between the cell below and the diagonal cell
		cellIndex = i + (j-1)*(m_RealCountX+1);
		lerpVal2 = glm::lerp(m_Velocity_U_Prev[cellIndex], m_Velocity_U_Prev[cellIndex + 1], alpha);
		return glm::lerp(lerpVal1, lerpVal2, beta);
	}
}

float Grid2D::getPrevParticleVelocityV(const vec2& particlePos, uint32_t i, uint32_t j)
{
	assert(i < m_RealCountX);
	assert(j < m_RealCountY);
	float alpha = 0.0f, beta, lerpVal1, lerpVal2;
	float xCoordVComponent;
	uint32_t index = i + j*m_RealCountX;
	uint32_t indexAbove = i + (j+1)*m_RealCountX;
	
	vec2 gridPointLowerLeftCorner = m_Position[m_CenterPositionOffset + i + j*(m_RealCountX+1)];
	//calculate x component of the center position in the bottom face
	xCoordVComponent = gridPointLowerLeftCorner.x + m_Width*0.5f;
	
	//1. get vertical lerp value in current cell between both v components
	if (particlePos.y < 0.0f) 
	{
		lerpVal1 = m_Velocity_V_Prev[index];
	} else if (particlePos.y > m_Position[m_CenterPositionOffset + (m_RealCountX+1)*m_RealCountY].y)
	{
		lerpVal1 = m_Velocity_V_Prev[(j+1)*(m_RealCountX+1) - 1];
		alpha = 1.0f;
	} else 
	{
		alpha = getLinearWeight(gridPointLowerLeftCorner.y, particlePos.y);
		lerpVal1 = glm::lerp(m_Velocity_V_Prev[index], m_Velocity_V_Prev[indexAbove] , alpha);
	}
	
	beta = getLinearWeight(particlePos.x, xCoordVComponent);

	//2. get vertical lerp value in neighbour cell. Either inside the right or left one.
	//afterwards do a bilinear interpolation between both cells
	//If beta value is below 0 then the particle is one the right side of the center 
	//position and the bilinear interpolation is done with the right cell
	if (beta < 0.0f)
	{
		//check if point is at right border and only simple lerp is possible
		if (i == (m_RealCountX-1))
		{
			return lerpVal1;
		}

		//interpolate between the two right cells
		beta *= -1.0f;
		index = i + j*m_RealCountX + 1; 
		indexAbove = i + (j+1)*m_RealCountX + 1;
		lerpVal2 = glm::lerp(m_Velocity_V_Prev[index], m_Velocity_V_Prev[indexAbove], alpha);
		return glm::lerp( lerpVal1, lerpVal2, beta);
	} else
	{
		//check if point is at left border and only simple lerp is possible
		if (i == 0)
		{
			return lerpVal1;
		}

		index = i + j*m_RealCountX - 1; 
		indexAbove = i + (j+1)*m_RealCountX - 1;
		//interpolate between the two left cells
		lerpVal2 = glm::lerp(m_Velocity_V_Prev[index], m_Velocity_V_Prev[indexAbove], alpha);
		return glm::lerp(lerpVal1, lerpVal2, beta);
	}
}

float Grid2D::getCentralQuantity(uint32_t i, uint32_t j, float* quantity)
{
	return quantity[i + m_RealCountX*j];
}

float* Grid2D::getVelocityU()
{
	return m_Velocity_U;
}

float* Grid2D::getVelocityV()
{
	return m_Velocity_V;
}

float* Grid2D::getVelocityBackupU() 
{
	return m_Velocity_U_Backup;
}

float* Grid2D::getVelocityBackupV() 
{
	return m_Velocity_V_Backup;
}

uint32_t Grid2D::getVelocityUCount()
{
	return m_RealCountX + 1;
}

uint32_t Grid2D::getVelocityVCount()
{
	return m_RealCountY + 1;
}

void Grid2D::switchQuantities()
{
	//switch central quantities
	float* temp = m_Temperature;
	m_Temperature = m_TemperaturePrev;
	m_TemperaturePrev = temp;

	temp = m_Density;
	m_Density = m_DensityPrev;
	m_DensityPrev = temp;

	//switch vector quantities
	temp = m_Velocity_U_Prev;
	m_Velocity_U_Prev = m_Velocity_U;
	m_Velocity_U = m_Velocity_U_Backup;
	m_Velocity_U_Backup = temp;

	temp = m_Velocity_V_Prev;
	m_Velocity_V_Prev = m_Velocity_V;
	m_Velocity_V = m_Velocity_V_Backup;
	m_Velocity_V_Backup = temp;
}