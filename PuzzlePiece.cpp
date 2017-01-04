#include "PuzzlePiece.h"

PuzzlePiece::PuzzlePiece( vector<vector<vector<int>>> layers, string id, string partnerId )
	: m_layers( layers ), m_id( id ), m_partnerId( partnerId )
{
	m_color = { (GLfloat) 0.0, (GLfloat) 0.0, (GLfloat) 1.0, (GLfloat) 0.3 };

	m_transform = hduMatrix::createTranslation( 0.0, 0.0, 0.0 );

	m_centerpoint = hduVector3Df(2.0, 1.0, 2.0);
}


PuzzlePiece::~PuzzlePiece()
{
}
