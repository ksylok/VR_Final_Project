// UPDATED: 2012-12-05  (Y. Hu: Microsoft Visual Studio 2010; System configuration - Windows 7 + 16:9 screen ratio) 
// UPDATED: 2013-07-30  (Y. Hu: Microsoft Visual Studio 2012 + Platform toolset in Visual Studio 2010 due to OpenHatics-V3.10.5; System configuration - Windows 7 + 16:9 screen ratio) 
// UPDATED: 2014-05-30  (Y. Hu: Microsoft Visual Studio 2012 + Platform toolset in Visual Studio 2010 due to OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio) 
// UPDATED: 2015-07-21  (Y. Hu: Microsoft Visual Studio 2013 + Platform toolset in Visual Studio 2013; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)
// UPDATED: 2016-08-25  (Y. Hu: Microsoft Visual Studio 2015 + Platform toolset in Visual Studio 2015; OpenHatics-V3.2.2; System configuration - Windows 7 + 16:9 screen ratio)

#pragma once

#include "caveMain.h"

#include "FaceManager.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <float.h>
#include <assert.h>
#include <vector>
#include <gl/glut.h>
#include <math.h>
#include <algorithm>

#define M_PI 3.14159265358979323846 /* Pi */

using namespace std;

class FaceManager : public IFaceManager
{
public:

    FaceManager();
    ~FaceManager();

    void setup();
    void cleanup();

    void setDrawMode( drawMode newMode );
	void drawPuzzlePiece( string id );
	void drawAllPuzzlePieces();

	void drawCenters();

	map<string, PuzzlePiece*> getPieces() { return m_puzzlePieces; } // returns a copy of the owned 

private:

	void initPieces();
	void initPieceOffsets();
	void drawCube(vector<vector<vector<int>>> cubeLayers, RGBa color);

	map<string, PuzzlePiece*> m_puzzlePieces;	// Owns all puzzle pieces
	drawMode m_drawMode;
};

/******************************************************************************
 Factory method for creating an instance of PointManager.
******************************************************************************/
IFaceManager *IFaceManager::create(void)
{
    return new FaceManager;
}

/*******************************************************************************
 Factory method for destroying an instance of FaceManager.
*******************************************************************************/
void IFaceManager::destroy(IFaceManager *&fInterface)
{
    if (fInterface)
    {
        FaceManager *fImp = static_cast<FaceManager *>(fInterface);
        delete fImp;
        fInterface = 0;
    }
}

/*******************************************************************************
 PointManager constructor.
*******************************************************************************/
FaceManager::FaceManager()
{
}

/*******************************************************************************
 FaceManager destructor.
*******************************************************************************/
FaceManager::~FaceManager()
{
}

/*******************************************************************************
 Initialization of the manager
*******************************************************************************/
void FaceManager::setup()
{
	m_drawMode = solidWireframe;

	initPieces();
	initPieceOffsets();
}

/*******************************************************************************
Clean up function
*******************************************************************************/
void FaceManager::cleanup()
{
	auto iter = m_puzzlePieces.begin();

	while (iter != m_puzzlePieces.end())
	{
		delete iter->second;
		iter++;
	}

	m_puzzlePieces.clear();
}

/*******************************************************************************
setDrawMode selects how we will draw every cube; solidWireframe or translucent
*******************************************************************************/
void FaceManager::setDrawMode( drawMode newMode )
{
	m_drawMode = newMode;
}

/*******************************************************************************
initPieces generates the list of puzzle pieces that the game will be using.
*******************************************************************************/
void FaceManager::initPieces() {
	m_puzzlePieces.clear();

	//Puzzle 1 - Piece 1
	vector<vector<int>> p1p1_L1 =
	{
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 }
	};

	vector<vector<int>> p1p1_L2 =
	{
		{ 1, 1, 1, 1, 1 },
		{ 1, 0, 1, 1, 1 },
		{ 1, 0, 0, 0, 0 },
		{ 1, 1, 0, 1, 1 },
		{ 1, 1, 1, 1, 1 }
	};

	vector<vector<int>> p1p1_L3 =
	{
		{ 1, 0, 0, 0, 1 },
		{ 0, 0, 0, 1, 0 },
		{ 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0 },
		{ 1, 0, 0, 0, 1 }
	};

	PuzzlePiece* p1_p1 = new PuzzlePiece({ p1p1_L1, p1p1_L2, p1p1_L3 }, "1_1", "1_2");
	m_puzzlePieces.emplace(p1_p1->GetID(), p1_p1);

	//Puzzle 1 - Piece 2
	vector<vector<int>> p1p2_L1 =
	{
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 }
	};

	vector<vector<int>> p1p2_L2 =
	{
		{ 0, 1, 1, 1, 0 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 0, 1 },
		{ 0, 1, 1, 1, 0 }
	};

	vector<vector<int>> p1p2_L3 =
	{
		{ 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 0, 0 },
		{ 0, 1, 1, 1, 1 },
		{ 0, 1, 0, 0, 0 },
		{ 0, 0, 0, 0, 0 }
	};

	PuzzlePiece* p1_p2 = new PuzzlePiece({ p1p2_L1, p1p2_L2, p1p2_L3 }, "1_2", "1_1" );
	m_puzzlePieces.emplace(p1_p2->GetID(), p1_p2);

	//Puzzle 2 - Piece 1
	vector<vector<int>> p2p1_L1 =
	{
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 }
	};

	vector<vector<int>> p2p1_L2 =
	{
		{ 1, 1, 1, 1, 1 },
		{ 1, 0, 1, 1, 1 },
		{ 1, 0, 0, 1, 0 },
		{ 1, 1, 0, 0, 1 },
		{ 1, 1, 1, 1, 1 }
	};

	vector<vector<int>> p2p1_L3 =
	{
		{ 1, 0, 0, 0, 1 },
		{ 0, 0, 0, 1, 0 },
		{ 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0 },
		{ 1, 0, 0, 0, 1 }
	};

	PuzzlePiece* p2_p1 = new PuzzlePiece({ p2p1_L1, p2p1_L2, p2p1_L3 }, "2_1", "2_2" );
	m_puzzlePieces.emplace(p2_p1->GetID(), p2_p1);

	//Puzzle 2 - Piece 2
	vector<vector<int>> p2p2_L1 =
	{
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 }
	};

	vector<vector<int>> p2p2_L2 =
	{
		{ 0, 1, 1, 1, 0 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 0, 1 },
		{ 0, 1, 1, 1, 0 }
	};

	vector<vector<int>> p2p2_L3 =
	{
		{ 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 1, 0 },
		{ 0, 1, 1, 0, 1 },
		{ 0, 1, 0, 0, 0 },
		{ 0, 0, 0, 0, 0 }
	};

	PuzzlePiece* p2_p2 = new PuzzlePiece({ p2p2_L1, p2p2_L2, p2p2_L3 }, "2_2", "2_1" );
	m_puzzlePieces.emplace(p2_p2->GetID(), p2_p2);

	//Puzzle 3 - Piece 1
	vector<vector<int>> p3p1_L1 =
	{
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 }
	};

	vector<vector<int>> p3p1_L2 =
	{
		{ 1, 1, 0, 1, 1 },
		{ 1, 0, 1, 0, 1 },
		{ 1, 0, 0, 0, 1 },
		{ 1, 0, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 }
	};

	vector<vector<int>> p3p1_L3 =
	{
		{ 1, 0, 0, 0, 1 },
		{ 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0 },
		{ 1, 0, 0, 0, 1 }
	};

	PuzzlePiece* p3_p1 = new PuzzlePiece({ p3p1_L1, p3p1_L2, p3p1_L3 }, "3_1", "3_2");
	m_puzzlePieces.emplace(p3_p1->GetID(), p3_p1);

	//Puzzle 3 - Piece 2
	vector<vector<int>> p3p2_L1 =
	{
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 }
	};

	vector<vector<int>> p3p2_L2 =
	{
		{ 0, 1, 1, 1, 0 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 0, 1, 1, 1, 0 }
	};

	vector<vector<int>> p3p2_L3 =
	{
		{ 0, 0, 0, 0, 0 },
		{ 0, 1, 0, 0, 0 },
		{ 0, 1, 1, 1, 0 },
		{ 0, 1, 0, 1, 0 },
		{ 0, 0, 1, 0, 0 }
	};

	PuzzlePiece* p3_p2 = new PuzzlePiece({ p3p2_L1, p3p2_L2, p3p2_L3 }, "3_2", "3_1");
	m_puzzlePieces.emplace(p3_p2->GetID(), p3_p2);
	
	//Puzzle 4 - Piece 1
	vector<vector<int>> p4p1_L1 =
	{
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 }
	};

	vector<vector<int>> p4p1_L2 =
	{
		{ 1, 1, 0, 1, 1 },
		{ 1, 1, 0, 1, 1 },
		{ 1, 0, 0, 1, 1 },
		{ 1, 1, 0, 0, 1 },
		{ 1, 1, 1, 1, 1 }
	};

	vector<vector<int>> p4p1_L3 =
	{
		{ 1, 0, 0, 0, 1 },
		{ 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0 },
		{ 1, 0, 0, 0, 1 }
	};

	PuzzlePiece* p4_p1 = new PuzzlePiece({ p4p1_L1, p4p1_L2, p4p1_L3 }, "4_1", "4_2");
	m_puzzlePieces.emplace(p4_p1->GetID(), p4_p1);

	//Puzzle 4 - Piece 2
	vector<vector<int>> p4p2_L1 =
	{
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 }
	};

	vector<vector<int>> p4p2_L2 =
	{
		{ 0, 1, 1, 1, 0 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1 },
		{ 0, 1, 1, 1, 0 }
	};

	vector<vector<int>> p4p2_L3 =
	{
		{ 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 1, 0 },
		{ 0, 1, 1, 0, 0 },
		{ 0, 0, 1, 0, 0 },
		{ 0, 0, 1, 0, 0 }
	};

	PuzzlePiece* p4_p2 = new PuzzlePiece({ p4p2_L1, p4p2_L2, p4p2_L3 }, "4_2", "4_1");
	m_puzzlePieces.emplace(p4_p2->GetID(), p4_p2);
}

/*******************************************************************************
initPieceOffsets moves the pieces to their initial positions and orientations.
*******************************************************************************/
void FaceManager::initPieceOffsets()
{
	hduMatrix transformation;
	hduMatrix rotation = hduMatrix::createRotationAroundX(M_PI);

	hduMatrix* baseTransform;

	// hold x coordinate of pieces
	vector<float> posVector;
	posVector.push_back(10.0);
	posVector.push_back(2.0);
	posVector.push_back(-6.0);
	posVector.push_back(-14.0);

	// randomize x positions of pieces
	std::random_shuffle(posVector.begin(), posVector.end());

	float z_offset = -7.0;

	// Puzzle 1 - Piece 1
	baseTransform = m_puzzlePieces["1_1"]->GetTransform();

	transformation = hduMatrix::createTranslation(-14.0, 0.0, 0.0 + z_offset);
	baseTransform->multRight(transformation);

	// Puzzle 1 - Piece 2
	baseTransform = m_puzzlePieces["1_2"]->GetTransform();

	baseTransform->multRight(rotation);
	transformation = hduMatrix::createTranslation(posVector[0], 8.0, 4.0 + z_offset);
	baseTransform->multRight(transformation);

	// Puzzle 2 - Piece 1
	baseTransform = m_puzzlePieces["2_1"]->GetTransform();

	transformation = hduMatrix::createTranslation(-6.0, 0.0, 0.0 + z_offset);
	baseTransform->multRight(transformation);

	// Puzzle 2 - Piece 2
	baseTransform = m_puzzlePieces["2_2"]->GetTransform();

	baseTransform->multRight(rotation);
	transformation = hduMatrix::createTranslation(posVector[1], 8.0, 4.0 + z_offset);
	baseTransform->multRight(transformation);

	// Puzzle 3 - Piece 1
	baseTransform = m_puzzlePieces["3_1"]->GetTransform();

	transformation = hduMatrix::createTranslation(2.0, 0.0, 0.0 + z_offset);
	baseTransform->multRight(transformation);

	// Puzzle 3 - Piece 2
	baseTransform = m_puzzlePieces["3_2"]->GetTransform();

	baseTransform->multRight(rotation);
	transformation = hduMatrix::createTranslation(posVector[2], 8.0, 4.0 + z_offset);
	baseTransform->multRight(transformation);

	// Puzzle 4 - Piece 1
	baseTransform = m_puzzlePieces["4_1"]->GetTransform();

	transformation = hduMatrix::createTranslation(10.0, 0.0, 0.0 + z_offset);
	baseTransform->multRight(transformation);

	// Puzzle 4 - Piece 2
	baseTransform = m_puzzlePieces["4_2"]->GetTransform();

	baseTransform->multRight(rotation);
	transformation = hduMatrix::createTranslation(posVector[3], 8.0, 4.0 + z_offset);
	baseTransform->multRight(transformation);
}

/*******************************************************************************
drawPuzzlePiece will draw a piece given a correct id. The id must exist in the 
map of puzzle pieces, and is the format "X_Y" where X is the puzzle number, and 
Y is the piece number. If given an invalid id, nothing is drawn or deleted.
*******************************************************************************/
void FaceManager::drawPuzzlePiece( string id )
{
	PuzzlePiece* piece = m_puzzlePieces[id];

	if (!piece)
	{
		fprintf(stderr, "Bad pointer in puzzlePieces map: %s", id);
		return;
	}

	if ( m_drawMode == translucent )
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	if( m_drawMode == solidWireframe )
		glLineWidth(5.0);

	glPushMatrix();
	glMultMatrixd(*(piece->GetTransform()));

	drawCube(piece->GetLayers(), piece->GetColor());

	glPopMatrix();
	glFlush();

	if (m_drawMode == solidWireframe)
		glLineWidth(1.0);

	if (m_drawMode == translucent)
	{
		glDisable(GL_BLEND);
	}
}

/*******************************************************************************
drawAllPuzzlePieces is a convenience method that draws every piece that is in 
the map of puzzle pieces. Any invalid pointers are skipped, but not deleted.
*******************************************************************************/
void FaceManager::drawAllPuzzlePieces()
{
	for (auto piece : m_puzzlePieces)
	{
		if (!piece.second)
		{
			fprintf(stderr, "Bad pointer in puzzlePieces map: %s", piece.first);
			continue;
		}

		drawPuzzlePiece(piece.first);
	}
}

void FaceManager::drawCenters()
{
	for ( auto elem : m_puzzlePieces )
	{
		if ( !elem.second )
			continue;

		PuzzlePiece* piece = elem.second;
		auto center = piece->GetCenter();

		glPushMatrix();

		glTranslated( center[0], center[1], center[2] );
		glColor3f( 1.0, 1.0, 1.0 );

		glutSolidSphere( 0.3, 10, 10 );

		glPopMatrix();
	}
	glFlush();
}

/*******************************************************************************
 drawCube will draw a puzzle piece given a vector of 2d int vectors containing only 1s and 0s.
 The outermost vector represents layers, while each 2d int vector represents rows and columns.
 For each 1, drawPiece will draw a Cube. For any other value, drawPiece will skip that position.

 All translations, rotations, etc. must be done outside this method.
 *******************************************************************************/
void FaceManager::drawCube( vector<vector<vector<int>>> cubeLayers, RGBa color )
{
	glColor4f(color.red, color.green, color.blue, color.alpha);
	for (auto layer : cubeLayers)
	{
		for (int i = 0; i < 5; ++i) // draw each column
		{
			for (int j = 0; j < 5; ++j) // draw each row
			{
				if (layer.at(i).at(j) == 1)
					glutSolidCube(1.0);

				glTranslatef(1.0, 0.0, 0.0);
			}
			glTranslatef(-5.0, 0.0, 1.0); // reset current column position; move to next row ( x - 5, z + 1 )
		}
		glTranslatef(0.0, 1.0, -5.0); // reset current layer position; move to next layer ( y + 1, z - 5 )
	}

	glTranslatef(0.0, -3.0, 0.0);	// reset position for drawing
	glColor4f(0.0, 1.0, 0.0, 1.0);
	for (auto layer : cubeLayers)
	{
		for (int i = 0; i < 5; ++i) // draw each column
		{
			for (int j = 0; j < 5; ++j) // draw each row
			{
				if (layer.at(i).at(j) == 1)
					glutWireCube(1.001);

				glTranslatef(1.0, 0.0, 0.0);
			}
			glTranslatef(-5.0, 0.0, 1.0); // reset current column position; move to next row ( x - 5, z + 1 )
		}
		glTranslatef(0.0, 1.0, -5.0); // reset current layer position; move to next layer ( y + 1, z - 5 )
	}
}
