#version 430

layout(location = 0) out vec4 fColor;

in vec2 uv;

uniform int velocityUCount; //equals count horizontal cells +1
uniform int velocityVCount; //equals count vertical cells +1

uniform float velocityU[8000];
uniform float velocityV[8000];

//cell indices
int i, j;
int cellCountVert = velocityVCount-1;
int cellCountHoriz = velocityUCount-1;

//width and height in uv-coordinates
float width = 1.0f/float(cellCountHoriz); 
float height = 1.0f/float(cellCountVert);

//weight for linear interpolation
float alpha, beta;

//x component of velocity vectors for bilinear interpolation
float x, xRight, xVertical, xDiagonal;

//y component of velocity vectors for bilinear interpolation
float y, yTop, yHorizontal, yDiagonal;

bool leftEdge = false, rightEdge = false, topEdge = false, bottomEdge = false;

float getXComponent(int i, int j)
{
	float jMid = float(j)*height + height*0.5f;
	alpha = (uv.x - width*float(i)) * float(cellCountHoriz);
	beta = (uv.y - jMid) * float(cellCountVert);
	
	x = velocityU[i+j*velocityUCount];
	xRight = velocityU[i+j*velocityUCount + 1];

	int verticalIndex = 0;
	
	//check if fragment is above cell center
	if (beta < 0.0f)
	{
		beta*= -1.0f;
		if (j == 0) verticalIndex = i;
		else verticalIndex = i+(j-1)*velocityUCount;
	} else 
	{
		if (j == cellCountVert-1) verticalIndex = i+j*velocityUCount;
		else verticalIndex = i+(j+1)*velocityUCount;
	}
	
	xVertical = velocityU[verticalIndex];
	xDiagonal = velocityU[verticalIndex+1];
	
	float lin1 = mix(x, xRight, alpha);
	float lin2 = mix(xVertical, xDiagonal, alpha);
	return mix(lin1, lin2, beta);
}

float getYComponent(int i, int j)
{
	float iMid = float(i)*width + width*0.5f;
	alpha = (uv.y - height*float(j)) * float(cellCountVert);
	beta = (uv.x - iMid) * float(cellCountHoriz);
	
	y = velocityV[i+j*cellCountHoriz];
	yTop = velocityV[i+(j+1)*cellCountHoriz];

	int horizontalIndex = 0;
	
	//check if fragment is right to the cell center
	if (beta < 0.0f)
	{
		beta*= -1.0f;
		if (i == 0) horizontalIndex = j*cellCountHoriz;
		else horizontalIndex = i+j*cellCountHoriz - 1;
	} else 
	{
		if (i == cellCountHoriz-1) horizontalIndex = i+j*cellCountHoriz;
		else horizontalIndex = i+j*cellCountHoriz + 1;
	}
	
	yHorizontal = velocityV[horizontalIndex];
	yDiagonal = velocityV[horizontalIndex+cellCountHoriz];
	
	float lin1 = mix(y, yTop, alpha);
	float lin2 = mix(yHorizontal, yDiagonal, alpha);
	return mix(lin1, lin2, beta);
}

void main() {
	i = int(uv.x * cellCountHoriz);
	j = int(uv.y * cellCountVert);
	
	if (i == 0) leftEdge = true;
	if (i == cellCountHoriz-1) rightEdge = true;
	if (j == 0) bottomEdge = true;
	if (j == cellCountVert-1) topEdge = true;
	
	float x = getXComponent(i, j);
	float y = getYComponent(i, j);
	float negativeDir = 0.0f;
	
	if (y < 0.0f && x < 0.0f)
	{
		negativeDir -= x;
		negativeDir -= y;
		fColor = vec4(-x, -y, negativeDir, 1.0f);
	} 
	else if (y < 0.0f)
	{
		fColor = vec4(x, 0.0f, -y, 1.0f);
	}
	else if  (x < 0.0f)
	{
		fColor = vec4(0.0f, y, -x, 1.0f);
	} else
	{
		fColor = vec4(x, y, 0.0f, 1.0f);
	}
}