#include "BSPMap.hpp"
#include "typedef.hpp"

#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

using namespace std;

BSPMap* g_pBSP = new BSPMap;

BSPMap::~BSPMap()
{}

void BSPMap::unload()
{
	delete[]   m_data;

	*m_path    = NULL;
	*m_mapName = NULL;
	m_header   = NULL;
	m_plane    = NULL;
	m_node     = NULL;
	m_leaf     = NULL;
}

bool BSPMap::IsNull()
{
	if( m_data == NULL )
		return true;
	if( *m_path == NULL )
		return true;
	if( *m_mapName == NULL )
		return true;
	if( m_header == NULL )
		return true;
	if( m_plane == NULL )
		return true;
	if( m_node == NULL )
		return true;
	if( m_leaf == NULL )
		return true;

	return false;
}



bool BSPMap::load( const char* path, const char* mapName )
{
	strcpy( m_path, path );
	strcpy( m_mapName, mapName );

	std::string fPath( m_path );
	fPath += "/csgo/maps/";
	fPath += m_mapName;
	// shit man you really want this shit?
	// HANDLE hFile = CreateFile( fPath.c_str(), GENERIC_READ, NULL, NULL, OPEN_ALWAYS, NULL, NULL ); // cout << "Path:" << fPath << endl;
	FILE* hFile = fopen(fPath.c_str(), "r");
	if( hFile == NULL ) {
    cout << "no handle" << endl;
		return false;
	}

	// DWORD dwSize = GetFileSize( hFile, NULL );
  struct stat filestatus;
  stat(fPath.c_str(), &filestatus );
  unsigned long dwSize = filestatus.st_size;

	if( !dwSize ) {
		fclose(hFile);
    cout << "no size" << endl;
		return false;
	}

	m_data = new char[ dwSize ];

  if(!fread(m_data, dwSize, 1, hFile)) {
		fclose(hFile);
		delete[] m_data;
    cout << "no read" << endl;
		return false;
	}

	// lol jk don't include this, trust me on this you don't need to close the handle
  fclose(hFile);

	m_header	= ( dheader_t* )m_data;

	m_node		= ( dnode_t*  )( m_data + m_header->lumps[ LUMP_NODES ].fileofs );
	m_plane		= ( dplane_t* )( m_data + m_header->lumps[ LUMP_PLANES ].fileofs );
	m_leaf		= ( dleaf_t*  )( m_data + m_header->lumps[ LUMP_LEAVES ].fileofs );

	return true;
}

void BSPMap::DisplayInfo() {
  cout << "map version  : " << m_header->version << endl;
  cout << "map name     : " << m_mapName << endl;
  cout << "map Revision : " << m_header->mapRevision << endl;
  cout << endl << endl;
}

void BSPMap::setPath( const char* path )
{
	strcpy( m_path, path );
}

int BSPMap::getVersion()
{
	return m_header->version;
}

int BSPMap::getRevision()
{
	return m_header->mapRevision;
}

const char* BSPMap::getPath()
{
	return m_path;
}

const char* BSPMap::getName()
{
	return m_mapName;
}

dnode_t* BSPMap::getNodeLump()
{
	return m_node;
}

dleaf_t* BSPMap::getLeafLump()
{
	return m_leaf;
}

dplane_t* BSPMap::getPlaneLump()
{
	return m_plane;
}

dleaf_t* BSPMap::GetLeafFromPoint(Vector point) {
	int nodenum = 0;
	dnode_t* node;
	dplane_t* plane;

	float d = 0.0f;

	while( nodenum >= 0 ) {
		node = &m_node[ nodenum ];
		plane = &m_plane[ node->planenum ];
		d = (point * plane->normal) - plane->dist;
		if( d > 0 )
			nodenum = node->children[ 0 ];
		else
			nodenum = node->children[ 1 ];
	}

	return &m_leaf[ -nodenum - 1 ];
}

bool BSPMap::Visible(Vector &vStart, Vector &vEnd ) {
	if( IsNull() ) {
    cout << "isnull" << endl;
		return false;
  }

	Vector direction = vEnd - vStart;
	Vector point = vStart;

  int steps = static_cast<int>(len(direction));

  cout << "steps:" << steps << endl;

	if( steps > 4000 ){
    // performence issue when checking long distances, 2000 too short
    cout << "too far" << endl;
		return false;   // we'll assume we can't see someone at great lengths
  }

	direction = direction / (float)steps;

	dleaf_t* leaf = nullptr;

	while( steps ) {
		point += direction;
		leaf = GetLeafFromPoint( point );
    printf("point x=%f y=%f z=%f\n", point.x, point.y, point.z);
		// Tried differenent masks, none seem to work :/ becauce ur a bitch tbh tbf fam
    cout << "contents: " << hex << leaf->contents << endl;
    cout << "water:" << leaf->leafWaterDataID << endl;
		if(leaf->contents & CONTENTS_SOLID) {
			return false;
    }

		--steps;
	}
	return true;
}
