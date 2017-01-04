#pragma once

#include <stdlib.h>
#include <vector>
#include <map>
#include <gl/glut.h>
#include <sstream>

#include <HDU/hduVector.h>
#include <HDU/hduMatrix.h>
#include <HDU/hduQuaternion.h>

using namespace std;

struct RGBa {
	GLfloat red;
	GLfloat green;
	GLfloat blue;
	GLfloat alpha;
};

class PuzzlePiece
{
public:
	PuzzlePiece( vector<vector<vector<int>>> layers, string id, string partnerId );
	virtual ~PuzzlePiece();

	vector<vector<vector<int>>> GetLayers() { return m_layers; }

	RGBa GetColor() { return m_color; }
	void SetColor(RGBa color) { m_color = color; }

	bool GetIsMatched() { return b_isMatched; }
	void SetIsMatched( bool val ) { b_isMatched = val; }

	hduMatrix* GetTransform() { return &m_transform; }
	hduVector3Dd GetCenter() { return m_centerpoint * m_transform; }	// returns transformed center point for this piece

	string GetID() { return m_id; }
	string GetPartnerId() { return m_partnerId; }

private:

	vector<vector<vector<int>>> m_layers;
	RGBa m_color;

	hduMatrix m_transform;
	hduVector3Df m_centerpoint;

	bool b_isMatched = false;

	string m_id;
	string m_partnerId;
};

